#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "receitas.h"
#include "utils.h"

#define REC_INITIAL_CAPACITY 8

/* Inicializa o banco de receitas (Array Dinamico de ponteiros) */
BancoReceitas* rec_inicializar() {
    BancoReceitas* banco = (BancoReceitas*) malloc(sizeof(BancoReceitas));
    if (!banco) return NULL;
    
    banco->capacidade = REC_INITIAL_CAPACITY;
    banco->qtd_atual = 0;
    banco->prox_id = 1; // IDs gerados por prox_id 
    banco->vetor = (Receita**) malloc(sizeof(Receita*) * banco->capacidade);
    
    if (!banco->vetor) {
        free(banco);
        return NULL;
    }
    return banco;
}

/* Garante que o array tenha espaco, usando realloc se necessario */
static int ensure_capacity(BancoReceitas* banco) {
    if (banco->qtd_atual < banco->capacidade) return 1;
    int newcap = banco->capacidade * 2;
    Receita** newvetor = realloc(banco->vetor, sizeof(Receita*) * newcap);
    if (!newvetor) return 0;
    banco->vetor = newvetor;
    banco->capacidade = newcap;
    return 1;
}

/* Cadastrar: Cria uma nova receita e adiciona ao banco */
int rec_cadastrar(BancoReceitas* banco, const char* nome, const char* preparo) {
    if (!banco || !nome) return 0;
    if (!ensure_capacity(banco)) return 0;

    Receita* nova = (Receita*) malloc(sizeof(Receita));
    if (!nova) return 0;

    nova->id = banco->prox_id++;
    nova->nome = utl_strdup(nome);
    nova->modo_preparo = utl_strdup(preparo ? preparo : "");
    nova->ingredientes = NULL; // Inicializa a lista encadeada vazia

    if (!nova->nome || !nova->modo_preparo) {
        free(nova->nome);
        free(nova->modo_preparo);
        free(nova);
        return 0;
    }

    banco->vetor[banco->qtd_atual] = nova;
    banco->qtd_atual++;
    return nova->id;
}

/* Buscar por ID: Retorna o ponteiro para a receita ou NULL se nao encontrar */
Receita* rec_buscar_id(const BancoReceitas* banco, int id) {
    if (!banco) return NULL;
    for (int i = 0; i < banco->qtd_atual; i++) {
        if (banco->vetor[i]->id == id) {
            return banco->vetor[i];
        }
    }
    return NULL;
}

/* Listar: Percorre o banco e chama a listagem de ingredientes de cada receita */
void rec_listar(const BancoReceitas* banco, const CatalogoIngredientes* cat) {
    if (!banco) return;
    printf("\n--- BANCO DE RECEITAS (%d receitas) ---\n", banco->qtd_atual);
    for (int i = 0; i < banco->qtd_atual; i++) {
        Receita* r = banco->vetor[i];
        printf("ID: %d | Nome: %s\n", r->id, r->nome);
        printf("Modo de Preparo: %s\n", r->modo_preparo);
        // Chama a funcao do modulo de ingredientes
        ing_listar(r->ingredientes, cat);
        printf("----------------------------------------\n");
    }
}

/* Remover: Organiza o array e libera a lista encadeada e strings internas */
int rec_remover(BancoReceitas* banco, int id) {
    if (!banco) return 0;
    int idx = -1;
    for (int i = 0; i < banco->qtd_atual; i++) {
        if (banco->vetor[i]->id == id) {
            idx = i;
            break;
        }
    }
    if (idx == -1) return 0;

    Receita* r = banco->vetor[idx];
    free(r->nome);
    free(r->modo_preparo);
    ing_liberar_lista(&r->ingredientes);
    free(r);

    // Shift para fechar o buraco no array (compactacao)
    for (int j = idx; j < banco->qtd_atual - 1; j++) {
        banco->vetor[j] = banco->vetor[j + 1];
    }
    banco->qtd_atual--;
    return 1;
}

/* Edita o nome de uma receita existente */
int rec_editar_nome(BancoReceitas* banco, int id, const char* novo_nome) {
    Receita* r = rec_buscar_id(banco, id);
    if (!r || !novo_nome) return 0;
    char* n = utl_strdup(novo_nome);
    if (!n) return 0;
    free(r->nome);
    r->nome = n;
    return 1;
}

/* Edita o modo de preparo de uma receita existente */
int rec_editar_preparo(BancoReceitas* banco, int id, const char* novo_preparo) {
    Receita* r = rec_buscar_id(banco, id);
    if (!r || !novo_preparo) return 0;
    char* p = utl_strdup(novo_preparo);
    if (!p) return 0;
    free(r->modo_preparo);
    r->modo_preparo = p;
    return 1;
}

/* Adiciona um ingrediente a uma receita (usando o ID do catalogo) */
int rec_add_ingrediente(BancoReceitas* banco, int id_receita, int id_ingrediente, float qtd) {
    Receita* r = rec_buscar_id(banco, id_receita);
    if (!r) return 0;
    ing_adicionar(&r->ingredientes, id_ingrediente, qtd);
    return 1;
}

/* Remove um ingrediente de uma receita */
int rec_rem_ingrediente(BancoReceitas* banco, int id_receita, int id_ingrediente) {
    Receita* r = rec_buscar_id(banco, id_receita);
    if (!r) return 0;
    return ing_remover(&r->ingredientes, id_ingrediente);
}

/* Liberar Memoria Total do banco e de todas as receitas */
void rec_liberar_tudo(BancoReceitas* banco) {
    if (!banco) return;
    for (int i = 0; i < banco->qtd_atual; i++) {
        Receita* r = banco->vetor[i];
        free(r->nome);
        free(r->modo_preparo);
        ing_liberar_lista(&r->ingredientes);
        free(r);
    }
    free(banco->vetor);
    free(banco);
}
