#include "estoque.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define ESTOQUE_INITIAL_CAPACITY 8

/*
    cria um espaço com malloc para o novo estoque
    se por algum motivo o estoque não carregra direito, retorna

    cria um espaço pros itens do estoque o tamanho é a capacidade inicial do estoque
    mesmo esquema vendo se não carregou direito depois

    dps registra a capacidade e a quantidade atual do estoque
*/
Estoque *est_inicializar() {
    Estoque *est = malloc(sizeof(Estoque));
    if (!est) return NULL;

    est->itens = malloc(sizeof(ItemEstoque) * ESTOQUE_INITIAL_CAPACITY);
    if (!est->itens) 
    {
        free(est);
        return NULL;
    }

    est->qtd_atual = 0;
    est->capacidade = ESTOQUE_INITIAL_CAPACITY;
    return est;
}

/*
    se o estoque não existir ignora

    libera o espaço dos itens do estoque, depois libera o estoque
*/
void est_liberar(Estoque *est) {
    if (!est) return;
    free(est->itens);
    free(est);
}

/*
    se o estoque não existir retorna -1

    usa um loop de for indo ate a quantidade atual de itens no estoque
    compara o id_igrediente com o que ta procurando, se achar retorna o indice, se não -1
*/
int est_buscar_indice(Estoque *est, int id_ingrediente) {
    if (!est) return -1;
    for (int i = 0; i < est->qtd_atual; ++i) {
        if (est->itens[i].id_ingrediente == id_ingrediente) return i;
    }
    return -1;
}

/* 
    ensure_capacity
        - Verifica se há espaço no array para pelo menos mais 1 item.
        - Se não houver, realoca o array dobrando a capacidade.
        - Retorna 1 em sucesso, 0 em falha (realloc falhou).
        
        Observação: função interna (static) para reduzir duplicação de código.
 */
static int ensure_capacity(Estoque *est) {
    if (est->qtd_atual < est->capacidade) return 1;
    size_t newcap = est->capacidade * 2;
    ItemEstoque *newitems = realloc(est->itens, sizeof(ItemEstoque) * newcap);
    if (!newitems) return 0;
    est->itens = newitems;
    est->capacidade = (int)newcap;
    return 1;
}

/*
    primeiro procura o indice do item
    se encontrar o item, quando o indice não é -1, soma a qtd a quantidade total do item 

    se não ajusta a capacidade se necessario, e coloca o item no proximo espaço e dps aumenta a quantidade atual
*/
void est_adicionar(Estoque *est, int id_ingrediente, float qtd) {
    if (!est) return;
    int indice = est_buscar_indice(est, id_ingrediente);
    if (indice != -1)
    {
        est->itens[indice].quantidade += qtd;
        return;
    }
    if (ensure_capacity(est))
    {
        est->itens[est->qtd_atual].id_ingrediente = id_ingrediente;
        est->itens[est->qtd_atual].quantidade = qtd;
        est->qtd_atual += 1;
    }
}

/*
    procura o indice do item
    se não encontrar ou a quantida a ser removida for negativa, retorna 0

    ve se a quantidade de items é maior ou igual do que vai ser removido
    se for subtrai e retorna 1

    se não retorna 0
*/
int est_remover(Estoque *est, int id_ingrediente, float qtd) {
    if (!est) return 0;
    int indice = est_buscar_indice(est, id_ingrediente);
    if (indice == -1 || qtd <= 0) return 0;

    if (est->itens[indice].quantidade >= qtd)
    {
        est->itens[indice].quantidade -= qtd;
        return 1;
    }
    
    return 0;
}

/* Remove completamente um item do estoque (compacta o array) */
int est_deletar_item(Estoque *est, int id_ingrediente) {
    if (!est) return 0;
    int indice = est_buscar_indice(est, id_ingrediente);
    if (indice == -1) return 0;
    for (int j = indice; j < est->qtd_atual - 1; j++) {
        est->itens[j] = est->itens[j + 1];
    }
    est->qtd_atual--;
    return 1;
}

/* 
    Itera sobre o estoque e cruza a informação do ID com o Catálogo.
    Imprime Nome, Quantidade e Unidade. Trata o caso onde o item
    pode ter sido apagado do catálogo mantendo-se no estoque.
 */
void est_listar(const Estoque *est, const CatalogoIngredientes *cat) {
    if (!est || !cat) return;
    
    printf("\n--- ESTOQUE ATUAL (%d tipos de itens) ---\n", est->qtd_atual);
    
    for (int i = 0; i < est->qtd_atual; ++i) {
        int id = est->itens[i].id_ingrediente;
        float qtd = est->itens[i].quantidade;
        
        const char *nome = cat_get_nome(cat, id);
        const char *unidade = cat_get_unidade(cat, id);
        
        const char *nome_exibicao = nome ? nome : "Desconhecido";
        const char *unid_exibicao = unidade ? unidade : "";

        printf("  [ID: %d] %s: %.2f %s\n", id, nome_exibicao, qtd, unid_exibicao);
    }
    printf("------------------------------------------\n");
}
