#include "persistencia.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- CATALOGO --- */

int pers_salvar_catalogo(const CatalogoIngredientes* cat) {
    if (!cat) return 0;
    FILE* f = fopen(PATH_INGREDIENTES, "w");
    if (!f) return 0;

    for (size_t i = 0; i < cat->qtd_atual; i++) {
        fprintf(f, "%d;%s;%s\n", cat->itens[i].id, cat->itens[i].nome, cat->itens[i].unidade);
    }

    fclose(f);
    return 1;
}

int pers_carregar_catalogo(CatalogoIngredientes* cat) {
    if (!cat) return 0;
    FILE* f = fopen(PATH_INGREDIENTES, "r");
    if (!f) return 0;

    char linha[512];
    int max_id = 0;

    while (fgets(linha, sizeof(linha), f)) {
        utl_chomp(linha);
        if (strlen(linha) == 0) continue;

        char* id_str = strtok(linha, ";");
        char* nome = strtok(NULL, ";");
        char* unidade = strtok(NULL, ";");

        if (id_str && nome && unidade) {
            int id = atoi(id_str);
            cat_cadastrar(cat, nome, unidade);
            // Sobrescreve o ID gerado pelo automático para manter o do arquivo
            cat->itens[cat->qtd_atual - 1].id = id;
            if (id >= cat->prox_id) cat->prox_id = id + 1;
        }
    }

    fclose(f);
    return 1;
}

/* --- ESTOQUE --- */

int pers_salvar_estoque(const Estoque* estoque) {
    if (!estoque) return 0;
    FILE* f = fopen(PATH_ESTOQUE, "w");
    if (!f) return 0;

    for (int i = 0; i < estoque->qtd_atual; i++) {
        fprintf(f, "%d;%.2f\n", estoque->itens[i].id_ingrediente, estoque->itens[i].quantidade);
    }

    fclose(f);
    return 1;
}

int pers_carregar_estoque(Estoque* estoque) {
    if (!estoque) return 0;
    FILE* f = fopen(PATH_ESTOQUE, "r");
    if (!f) return 0;

    char linha[256];
    while (fgets(linha, sizeof(linha), f)) {
        utl_chomp(linha);
        int id;
        float qtd;
        if (sscanf(linha, "%d;%f", &id, &qtd) == 2) {
            est_adicionar(estoque, id, qtd);
        }
    }

    fclose(f);
    return 1;
}

/* --- RECEITAS --- */

int pers_salvar_receitas(const BancoReceitas* banco) {
    if (!banco) return 0;
    FILE* f = fopen(PATH_RECEITAS, "w");
    if (!f) return 0;

    for (int i = 0; i < banco->qtd_atual; i++) {
        Receita* r = banco->vetor[i];
        // Formato: [R];id;nome;preparo
        fprintf(f, "[R];%d;%s;%s\n", r->id, r->nome, r->modo_preparo);
        
        NoIngrediente* ing = r->ingredientes;
        while (ing) {
            // Formato: [I];id_ingrediente;quantidade
            fprintf(f, "[I];%d;%.2f\n", ing->id_ingrediente, ing->quantidade);
            ing = ing->prox;
        }
    }

    fclose(f);
    return 1;
}

int pers_carregar_receitas(BancoReceitas* banco) {
    if (!banco) return 0;
    FILE* f = fopen(PATH_RECEITAS, "r");
    if (!f) return 0;

    char linha[1024];
    Receita* receita_atual = NULL;

    while (fgets(linha, sizeof(linha), f)) {
        utl_chomp(linha);
        if (strlen(linha) == 0) continue;

        char* tipo = strtok(linha, ";");
        if (!tipo) continue;

        if (strcmp(tipo, "[R]") == 0) {
            char* id_str = strtok(NULL, ";");
            char* nome = strtok(NULL, ";");
            char* preparo = strtok(NULL, ";");

            if (id_str && nome) {
                int id = atoi(id_str);
                int novo_id = rec_cadastrar(banco, nome, preparo);
                receita_atual = rec_buscar_id(banco, novo_id);
                if (receita_atual) {
                    receita_atual->id = id;
                    if (id >= banco->prox_id) banco->prox_id = id + 1;
                }
            }
        } else if (strcmp(tipo, "[I]") == 0 && receita_atual) {
            char* id_ing_str = strtok(NULL, ";");
            char* qtd_str = strtok(NULL, ";");
            if (id_ing_str && qtd_str) {
                ing_adicionar(&receita_atual->ingredientes, atoi(id_ing_str), atof(qtd_str));
            }
        }
    }

    fclose(f);
    return 1;
}

/* --- PEDIDOS (Opcional) --- */

int pers_salvar_pedidos(const FilaPedidos* fila) {
    if (!fila) return 0;
    FILE* f = fopen(PATH_PEDIDOS, "w");
    if (!f) return 0;

    NoPedido* atual = fila->inicio;
    while (atual) {
        // Salva apenas o ID da receita na fila
        fprintf(f, "%d\n", atual->receita->id);
        atual = atual->prox;
    }

    fclose(f);
    return 1;
}

int pers_carregar_pedidos(FilaPedidos* fila, BancoReceitas* banco) {
    if (!fila || !banco) return 0;
    FILE* f = fopen(PATH_PEDIDOS, "r");
    if (!f) return 0;

    char linha[64];
    while (fgets(linha, sizeof(linha), f)) {
        utl_chomp(linha);
        int id_rec = atoi(linha);
        Receita* r = rec_buscar_id(banco, id_rec);
        if (r) {
            ped_adicionar(fila, r);
        }
    }

    fclose(f);
    return 1;
}
