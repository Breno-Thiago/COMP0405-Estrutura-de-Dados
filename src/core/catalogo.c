#include "catalogo.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define CATALOGO_INITIAL_CAPACITY 8

/* 
    utl_strdup (usado em substituição ao strdup_safe original)
        - Cria uma cópia alocada de uma string.
        - Retorna ponteiro alocado ou NULL em falha.
 */

/* 
    cat_inicializar
        - Inicializa a estrutura CatalogoIngredientes.
        - Aloca array inicial com capacidade CATALOGO_INITIAL_CAPACITY.
        - Inicializa contadores e o próximo id (prox_id começa em 1).
 */
int cat_inicializar(CatalogoIngredientes **cat) {
    if (!cat) return 0;
    *cat = malloc(sizeof(CatalogoIngredientes));
    if (!(*cat)) return 0;
    
    (*cat)->itens = malloc(sizeof(IngredienteBase) * CATALOGO_INITIAL_CAPACITY);
    if (!((*cat)->itens)) {
        free(*cat);
        return 0;
    }
    (*cat)->qtd_atual = 0;
    (*cat)->capacidade = CATALOGO_INITIAL_CAPACITY;
    (*cat)->prox_id = 1;
    return 1;
}

/* 
    ensure_capacity
        - Verifica se há espaço no array para pelo menos mais 1 item.
        - Se não houver, realoca o array dobrando a capacidade.
 */
static int ensure_capacity(CatalogoIngredientes *cat) {
    if (cat->qtd_atual < cat->capacidade) return 1;
    size_t newcap = cat->capacidade * 2;
    IngredienteBase *newitems = realloc(cat->itens, sizeof(IngredienteBase) * newcap);
    if (!newitems) return 0;
    cat->itens = newitems;
    cat->capacidade = newcap;
    return 1;
}

/* 
    cat_cadastrar
        - Adiciona um novo item (nome, unidade) ao catálogo.
        - Gera um id único (baseado em prox_id) e armazena nome/unidade
          como cópias alocadas dinamicamente.
 */
int cat_cadastrar(CatalogoIngredientes *cat, const char *nome, const char *unidade) {
    if (!cat || !nome) return 0;
    if (!ensure_capacity(cat)) return 0;
    
    int id = cat->prox_id++;
    IngredienteBase *it = &cat->itens[cat->qtd_atual];
    it->id = id;
    it->nome = utl_strdup(nome);
    it->unidade = utl_strdup(unidade ? unidade : "");
    
    if (!it->nome || !it->unidade) {
        free(it->nome);
        free(it->unidade);
        return 0;
    }
    cat->qtd_atual++;
    return id;
}

/* 
    cat_buscar_id
        - Busca linear pelo item com o id especificado.
        - Retorna ponteiro para o item ou NULL se não encontrado.
 */
IngredienteBase *cat_buscar_id(const CatalogoIngredientes *cat, int id) {
    if (!cat) return NULL;
    for (size_t i = 0; i < cat->qtd_atual; ++i) {
        if (cat->itens[i].id == id) return &cat->itens[i];
    }
    return NULL;
}

/* 
    cat_buscar_nome
        - Busca id de um item com nome exatamente igual (strcmp).
        - Retorna id (>0) se encontrado, ou -1 se não achar.
 */
int cat_buscar_nome(const CatalogoIngredientes *cat, const char *nome) {
    if (!cat || !nome) return -1;
    for (size_t i = 0; i < cat->qtd_atual; ++i) {
        if (strcmp(cat->itens[i].nome, nome) == 0) return cat->itens[i].id;
    }
    return -1;
}

/* 
    Acessores simples: retornam const char* ou NULL se id não existir.
 */
const char *cat_get_nome(const CatalogoIngredientes *cat, int id) {
    IngredienteBase *it = cat_buscar_id(cat, id);
    return it ? it->nome : NULL;
}

const char *cat_get_unidade(const CatalogoIngredientes *cat, int id) {
    IngredienteBase *it = cat_buscar_id(cat, id);
    return it ? it->unidade : NULL;
}

/* 
    cat_editar 
        - Edita um item existente (id).
        - Faz duplicação das novas strings primeiro para garantir segurança.
 */
int cat_editar(CatalogoIngredientes *cat, int id, const char *novo_nome, const char *nova_unidade) {
    IngredienteBase *it = cat_buscar_id(cat, id);
    if (!it) return 0;
    
    if (novo_nome) {
        char *n = utl_strdup(novo_nome);
        if (!n) return 0;
        free(it->nome);
        it->nome = n;
    }
    if (nova_unidade) {
        char *u = utl_strdup(nova_unidade);
        if (!u) return 0;
        free(it->unidade);
        it->unidade = u;
    }
    return 1;
}

/* 
    cat_remover
        - Remove item por id e compacta o array.
 */
int cat_remover(CatalogoIngredientes *cat, int id) {
    if (!cat) return 0;
    int idx = -1;
    for (size_t i = 0; i < cat->qtd_atual; ++i) {
        if (cat->itens[i].id == id) {
            idx = (int)i;
            break;
        }
    }
    if (idx == -1) return 0;
    
    free(cat->itens[idx].nome);
    free(cat->itens[idx].unidade);
    
    for (size_t j = idx; j + 1 < cat->qtd_atual; ++j) {
        cat->itens[j] = cat->itens[j + 1];
    }
    cat->qtd_atual--;
    return 1;
}

/* 
    cat_listar
        - Lista o catálogo no stdout em formato simples.
 */
void cat_listar(const CatalogoIngredientes *cat) {
    if (!cat) return;
    printf("--- CATALOGO DE INGREDIENTES (%zu itens) ---\n", cat->qtd_atual);
    for (size_t i = 0; i < cat->qtd_atual; ++i) {
        printf("[ID: %d] %s (%s)\n", cat->itens[i].id, cat->itens[i].nome, cat->itens[i].unidade);
    }
    printf("--------------------------------------------\n");
}

/* 
    cat_liberar
        - Libera toda a memória alocada internamente pelo catálogo.
 */
void cat_liberar(CatalogoIngredientes *cat) {
    if (!cat) return;
    for (size_t i = 0; i < cat->qtd_atual; ++i) {
        free(cat->itens[i].nome);
        free(cat->itens[i].unidade);
    }
    free(cat->itens);
    free(cat);
}
