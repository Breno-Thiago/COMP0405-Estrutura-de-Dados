#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core/catalogo.h"
#include "core/estoque.h"
#include "core/receitas.h"
#include "core/pedidos.h"
#include "core/rollback.h"
#include "core/persistencia.h"
#include "core/utils.h"
#include "app_context.h"

/*
 * api.c — Camada de API para o Dashboard Web
 *
 * IMPORTANTE: toda saída para o FRONTEND vai por stdout (1 linha JSON).
 * Mensagens de debug/log internas do C vão para stderr (não chegam no Node).
 */

static AppContext *app = NULL;

/* ─── JSON helpers ────────────────────────────────────────────────────────── */
static void json_str(const char *s) {
    if (!s) { printf("\"\""); return; }
    printf("\"");
    for (; *s; s++) {
        unsigned char c = (unsigned char)*s;
        if (c < 0x20) continue;          /* ignora chars de controle */
        if (*s == '"')  { printf("\\\""); continue; }
        if (*s == '\\') { printf("\\\\"); continue; }
        printf("%c", *s);
    }
    printf("\"");
}

static void respond_ok()         { printf("{\"ok\":true}\n");  fflush(stdout); }
static void respond_ok_id(int id){ printf("{\"ok\":true,\"id\":%d}\n", id); fflush(stdout); }
static void respond_fail(const char *msg) {
    printf("{\"ok\":false,\"error\":");
    json_str(msg);
    printf("}\n");
    fflush(stdout);
}

/* ─── Verificações de dependência ─────────────────────────────────────────── */

/* Verifica se algum ingrediente de alguma receita usa este id_ingrediente */
static int ingrediente_usado_em_receita(int id_ing) {
    for (int i = 0; i < app->banco->qtd_atual; i++) {
        NoIngrediente *n = app->banco->vetor[i]->ingredientes;
        while (n) {
            if (n->id_ingrediente == id_ing) return 1;
            n = n->prox;
        }
    }
    return 0;
}

/* Verifica se algum pedido na fila usa esta receita */
static int receita_usada_em_pedido(int id_rec) {
    NoPedido *p = app->fila->inicio;
    while (p) {
        if (p->receita && p->receita->id == id_rec) return 1;
        p = p->prox;
    }
    return 0;
}

/* ─── Impressão de coleções ───────────────────────────────────────────────── */
static void print_catalog() {
    printf("[");
    for (size_t i = 0; i < app->cat->qtd_atual; i++) {
        if (i) printf(",");
        printf("{\"id\":%d,\"name\":", app->cat->itens[i].id);
        json_str(app->cat->itens[i].nome);
        printf(",\"unit\":");
        json_str(app->cat->itens[i].unidade);
        printf("}");
    }
    printf("]");
}

static void print_stock() {
    printf("[");
    for (int i = 0; i < app->estoque->qtd_atual; i++) {
        if (i) printf(",");
        printf("{\"id_ingrediente\":%d,\"quantity\":%.2f}",
            app->estoque->itens[i].id_ingrediente,
            app->estoque->itens[i].quantidade);
    }
    printf("]");
}

static void print_recipes() {
    printf("[");
    for (int i = 0; i < app->banco->qtd_atual; i++) {
        if (i) printf(",");
        Receita *r = app->banco->vetor[i];
        printf("{\"id\":%d,\"name\":", r->id);
        json_str(r->nome);
        printf(",\"preparo\":");
        json_str(r->modo_preparo);
        printf(",\"ingredients\":[");
        NoIngrediente *ing = r->ingredientes;
        int first = 1;
        while (ing) {
            if (!first) printf(",");
            printf("{\"id\":%d,\"qtd\":%.2f}", ing->id_ingrediente, ing->quantidade);
            first = 0;
            ing = ing->prox;
        }
        printf("]}");
    }
    printf("]");
}

static void print_orders() {
    printf("[");
    NoPedido *p = app->fila->inicio;
    int first = 1;
    while (p) {
        if (!first) printf(",");
        /* Segurança: verificar se o ponteiro receita é válido */
        if (p->receita) {
            printf("{\"id_pedido\":%d,\"id_receita\":%d,\"nome_receita\":",
                p->id_pedido, p->receita->id);
            json_str(p->receita->nome);
            printf("}");
        } else {
            printf("{\"id_pedido\":%d,\"id_receita\":0,\"nome_receita\":\"[Receita removida]\"}",
                p->id_pedido);
        }
        first = 0;
        p = p->prox;
    }
    printf("]");
}

/* ─── Handlers ────────────────────────────────────────────────────────────── */
static void cmd_get_all() {
    printf("{\"catalog\":");
    print_catalog();
    printf(",\"inventory\":");
    print_stock();
    printf(",\"recipes\":");
    print_recipes();
    printf(",\"orders\":");
    print_orders();
    printf("}\n");
    fflush(stdout);
}

static void cmd_add_catalogo(const char *nome, const char *unidade) {
    int id = cat_cadastrar(app->cat, nome, unidade);
    pers_salvar_catalogo(app->cat);
    respond_ok_id(id);
}

static void cmd_del_catalogo(int id) {
    /* Verifica estoque */
    if (est_buscar_indice(app->estoque, id) != -1) {
        respond_fail("Remova do estoque antes de excluir do catalogo");
        return;
    }
    /* Verifica receitas */
    if (ingrediente_usado_em_receita(id)) {
        respond_fail("Ingrediente usado em receita. Remova das receitas primeiro");
        return;
    }
    int ok = cat_remover(app->cat, id);
    if (ok) pers_salvar_catalogo(app->cat);
    if (ok) respond_ok(); else respond_fail("Item nao encontrado");
}

static void cmd_add_estoque(int id_ing, float qtd) {
    /* Verifica se ingrediente existe no catálogo */
    if (!cat_buscar_id(app->cat, id_ing)) {
        respond_fail("Ingrediente nao existe no catalogo");
        return;
    }
    est_adicionar(app->estoque, id_ing, qtd);
    pers_salvar_estoque(app->estoque);
    respond_ok();
}

static void cmd_del_estoque(int id_ing) {
    int ok = est_deletar_item(app->estoque, id_ing);
    if (ok) pers_salvar_estoque(app->estoque);
    if (ok) respond_ok(); else respond_fail("Item nao encontrado no estoque");
}

static void cmd_add_receita(const char *nome, const char *preparo) {
    int id = rec_cadastrar(app->banco, nome, preparo);
    pers_salvar_receitas(app->banco);
    respond_ok_id(id);
}

static void cmd_del_receita(int id) {
    /* Verifica se há pedidos usando esta receita */
    if (receita_usada_em_pedido(id)) {
        respond_fail("Receita tem pedidos na fila. Cancele os pedidos antes");
        return;
    }
    int ok = rec_remover(app->banco, id);
    if (ok) pers_salvar_receitas(app->banco);
    if (ok) respond_ok(); else respond_fail("Receita nao encontrada");
}

static void cmd_add_ing_receita(int id_rec, int id_ing, float qtd) {
    int ok = rec_add_ingrediente(app->banco, id_rec, id_ing, qtd);
    if (ok) pers_salvar_receitas(app->banco);
    if (ok) respond_ok(); else respond_fail("Falha ao adicionar ingrediente");
}

static void cmd_add_pedido(int id_rec) {
    Receita *r = rec_buscar_id(app->banco, id_rec);
    if (!r) { respond_fail("Receita nao encontrada"); return; }

    if (!r->ingredientes) {
        respond_fail("Receita sem ingredientes cadastrados");
        return;
    }

    /*
     * O pedido é aceito na fila SEM verificar estoque.
     * A verificação real acontece na hora de PROCESSAR,
     * usando a Pilha de Rollback (transação com desfazimento).
     */
    ped_adicionar(app->fila, r);
    pers_salvar_pedidos(app->fila);
    respond_ok();
}

static void cmd_del_pedido(int id_pedido) {
    NoPedido *atual = app->fila->inicio;
    NoPedido *anterior = NULL;
    while (atual) {
        if (atual->id_pedido == id_pedido) {
            if (anterior) anterior->prox = atual->prox;
            else          app->fila->inicio = atual->prox;
            if (app->fila->fim == atual) app->fila->fim = anterior;
            free(atual);
            pers_salvar_pedidos(app->fila);
            respond_ok();
            return;
        }
        anterior = atual;
        atual = atual->prox;
    }
    respond_fail("Pedido nao encontrado");
}

static void cmd_processar_pedido() {
    if (!app->fila->inicio) {
        respond_fail("Fila vazia");
        return;
    }

    NoPedido *pedido = app->fila->inicio;
    Receita *r = pedido->receita;

    if (!r) {
        app->fila->inicio = pedido->prox;
        if (!app->fila->inicio) app->fila->fim = NULL;
        free(pedido);
        pers_salvar_pedidos(app->fila);
        respond_fail("Pedido com receita invalida descartado");
        return;
    }

    /*
     * Lógica transacional com Pilha de Rollback:
     *
     * Para cada ingrediente da receita:
     *   1. Tenta remover a quantidade do estoque
     *   2. Se conseguiu → rb_push(id, qtd) empilha o registro
     *   3. Se falhou → ROLLBACK: rb_pop() desempilha cada item e devolve ao estoque
     *
     * O JSON de resposta inclui o log detalhado de cada operação da pilha.
     */
    PilhaRollback *rb = rb_criar();
    NoIngrediente *ing = r->ingredientes;
    int sucesso = 1;
    int pushCount = 0;
    int falhou_id = -1;
    float falhou_necessaria = 0;

    /* Array para log das operações da pilha (max 50 ingredientes) */
    typedef struct { int id; float qtd; const char *nome; const char *op; } PilhaLog;
    PilhaLog logs[100];
    int logCount = 0;

    /* Fase 1: Tentativa — retira cada ingrediente e empilha */
    while (ing) {
        if (est_remover(app->estoque, ing->id_ingrediente, ing->quantidade)) {
            rb_push(rb, ing->id_ingrediente, ing->quantidade);
            pushCount++;
            if (logCount < 100) {
                logs[logCount].id = ing->id_ingrediente;
                logs[logCount].qtd = ing->quantidade;
                logs[logCount].nome = cat_get_nome(app->cat, ing->id_ingrediente);
                logs[logCount].op = "PUSH";
                logCount++;
            }
        } else {
            sucesso = 0;
            falhou_id = ing->id_ingrediente;
            falhou_necessaria = ing->quantidade;
            break;
        }
        ing = ing->prox;
    }

    if (!sucesso) {
        /* Fase 2: Rollback — desempilha e devolve ao estoque */
        int pop_id; float pop_qtd;
        while (rb_pop(rb, &pop_id, &pop_qtd)) {
            est_adicionar(app->estoque, pop_id, pop_qtd);
            if (logCount < 100) {
                logs[logCount].id = pop_id;
                logs[logCount].qtd = pop_qtd;
                logs[logCount].nome = cat_get_nome(app->cat, pop_id);
                logs[logCount].op = "POP_ROLLBACK";
                logCount++;
            }
        }
        rb_liberar(rb);

        /* Info do ingrediente que falhou */
        const char *falhou_nome = cat_get_nome(app->cat, falhou_id);
        int falhou_idx = est_buscar_indice(app->estoque, falhou_id);
        float falhou_disponivel = (falhou_idx != -1) ? app->estoque->itens[falhou_idx].quantidade : 0;

        /* Monta JSON com log detalhado do rollback + info do que faltou */
        printf("{\"ok\":false,\"error\":\"Estoque insuficiente para: %s\",\"rollback\":true,",
            falhou_nome ? falhou_nome : "???");
        printf("\"falhou\":{\"id\":%d,\"nome\":", falhou_id);
        json_str(falhou_nome);
        printf(",\"necessario\":%.2f,\"disponivel\":%.2f},", falhou_necessaria, falhou_disponivel);
        printf("\"pilha_ops\":[");
        for (int i = 0; i < logCount; i++) {
            if (i) printf(",");
            printf("{\"op\":\"%s\",\"id\":%d,\"nome\":", logs[i].op, logs[i].id);
            json_str(logs[i].nome);
            printf(",\"qtd\":%.2f}", logs[i].qtd);
        }
        printf("]}\n");
        fflush(stdout);
        return;
    }

    /* Sucesso: remove pedido da fila */
    app->fila->inicio = pedido->prox;
    if (!app->fila->inicio) app->fila->fim = NULL;
    free(pedido);

    rb_liberar(rb);
    pers_salvar_estoque(app->estoque);
    pers_salvar_pedidos(app->fila);

    /* JSON de sucesso com log das operações da pilha */
    printf("{\"ok\":true,\"pilha_ops\":[");
    for (int i = 0; i < logCount; i++) {
        if (i) printf(",");
        printf("{\"op\":\"%s\",\"id\":%d,\"nome\":", logs[i].op, logs[i].id);
        json_str(logs[i].nome);
        printf(",\"qtd\":%.2f}", logs[i].qtd);
    }
    printf("]}\n");
    fflush(stdout);
}

/* ─── Parsing ─────────────────────────────────────────────────────────────── */
static int split_pipe(const char *src, char *a, int sa, char *b, int sb) {
    const char *pipe = strchr(src, '|');
    if (!pipe) return 0;
    int la = (int)(pipe - src);
    if (la >= sa) la = sa - 1;
    strncpy(a, src, la); a[la] = '\0';
    strncpy(b, pipe + 1, sb - 1); b[sb - 1] = '\0';
    /* Trim trailing whitespace */
    int lb = (int)strlen(b);
    while (lb > 0 && (b[lb-1] == '\n' || b[lb-1] == '\r' || b[lb-1] == ' ')) b[--lb] = '\0';
    return 1;
}

/* ─── Main loop ───────────────────────────────────────────────────────────── */
int main() {
    app = app_criar();
    if (!app) { fprintf(stderr, "Erro ao inicializar\n"); return 1; }

    pers_carregar_catalogo(app->cat);
    pers_carregar_receitas(app->banco);
    pers_carregar_estoque(app->estoque);
    pers_carregar_pedidos(app->fila, app->banco);

    /* Redireciona stdout dos módulos internos: usamos setvbuf para flush imediato */
    setvbuf(stdout, NULL, _IONBF, 0);

    char linha[2048];
    while (fgets(linha, sizeof(linha), stdin)) {
        utl_chomp(linha);
        if (!strlen(linha)) continue;

        char cmd[32];
        sscanf(linha, "%31s", cmd);
        const char *args = linha + strlen(cmd);
        while (*args == ' ') args++;

        if      (!strcmp(cmd, "GET_ALL"))          cmd_get_all();
        else if (!strcmp(cmd, "ADD_CATALOGO")) {
            char nome[128], unidade[32];
            if (split_pipe(args, nome, sizeof(nome), unidade, sizeof(unidade)))
                cmd_add_catalogo(nome, unidade);
            else respond_fail("Formato invalido: nome|unidade");
        }
        else if (!strcmp(cmd, "DEL_CATALOGO")) {
            int id; if (sscanf(args, "%d", &id) == 1) cmd_del_catalogo(id);
            else respond_fail("ID invalido");
        }
        else if (!strcmp(cmd, "ADD_ESTOQUE")) {
            int id; float qtd;
            if (sscanf(args, "%d %f", &id, &qtd) == 2) cmd_add_estoque(id, qtd);
            else respond_fail("Formato: id quantidade");
        }
        else if (!strcmp(cmd, "DEL_ESTOQUE")) {
            int id; if (sscanf(args, "%d", &id) == 1) cmd_del_estoque(id);
            else respond_fail("ID invalido");
        }
        else if (!strcmp(cmd, "ADD_RECEITA")) {
            char nome[128], preparo[512];
            if (split_pipe(args, nome, sizeof(nome), preparo, sizeof(preparo)))
                cmd_add_receita(nome, preparo);
            else respond_fail("Formato: nome|preparo");
        }
        else if (!strcmp(cmd, "DEL_RECEITA")) {
            int id; if (sscanf(args, "%d", &id) == 1) cmd_del_receita(id);
            else respond_fail("ID invalido");
        }
        else if (!strcmp(cmd, "ADD_ING_RECEITA")) {
            int id_rec, id_ing; float qtd;
            if (sscanf(args, "%d %d %f", &id_rec, &id_ing, &qtd) == 3)
                cmd_add_ing_receita(id_rec, id_ing, qtd);
            else respond_fail("Formato: id_rec id_ing qtd");
        }
        else if (!strcmp(cmd, "ADD_PEDIDO")) {
            int id; if (sscanf(args, "%d", &id) == 1) cmd_add_pedido(id);
            else respond_fail("ID invalido");
        }
        else if (!strcmp(cmd, "DEL_PEDIDO")) {
            int id; if (sscanf(args, "%d", &id) == 1) cmd_del_pedido(id);
            else respond_fail("ID invalido");
        }
        else if (!strcmp(cmd, "PROCESSAR_PEDIDO")) cmd_processar_pedido();
        else if (!strcmp(cmd, "QUIT"))             break;
        else respond_fail("Comando desconhecido");
    }

    app_destruir(app);
    return 0;
}
