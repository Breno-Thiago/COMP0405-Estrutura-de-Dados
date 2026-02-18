#include <stdio.h>
#include <stdlib.h>
#include "ingredientes.h"

/* Implementação de ing_criar_no */
NoIngrediente* ing_criar_no(int id_ingrediente, float qtd) {
    NoIngrediente* novo = (NoIngrediente*) malloc(sizeof(NoIngrediente));
    if (novo != NULL) {
        novo->id_ingrediente = id_ingrediente;
        novo->quantidade = qtd;
        novo->prox = NULL;
    }
    return novo;
}

/* Implementação de ing_adicionar */
void ing_adicionar(NoIngrediente** cabeca, int id_ingrediente, float qtd) {
    if (!cabeca) return;

    // 1. Verifica se o ingrediente já existe na lista (atualização)
    NoIngrediente* busca = ing_buscar(*cabeca, id_ingrediente);
    
    if (busca != NULL) {
        // Se já existe, apenas atualizamos a quantidade
        busca->quantidade = qtd;
        return;
    }

    // 2. Se não existe, cria um novo nó
    NoIngrediente* novo = ing_criar_no(id_ingrediente, qtd);
    if (novo == NULL) {
        fprintf(stderr, "Erro: Falha ao alocar memoria para ingrediente.\n");
        return;
    }

    // 3. Insere no início da lista (complexidade O(1))
    novo->prox = *cabeca;
    *cabeca = novo;
}

/* Implementação de ing_remover */
int ing_remover(NoIngrediente** cabeca, int id_ingrediente) {
    if (!cabeca || *cabeca == NULL) return 0; // Lista vazia

    NoIngrediente* atual = *cabeca;
    NoIngrediente* anterior = NULL;

    while (atual != NULL && atual->id_ingrediente != id_ingrediente) {
        anterior = atual;
        atual = atual->prox;
    }

    if (atual == NULL) return 0; // Não encontrou

    // Se for o primeiro da lista
    if (anterior == NULL) {
        *cabeca = atual->prox;
    } else {
        anterior->prox = atual->prox;
    }

    free(atual);
    return 1; // Sucesso
}

/* Implementação de ing_buscar */
NoIngrediente* ing_buscar(NoIngrediente* cabeca, int id_ingrediente) {
    NoIngrediente* atual = cabeca;
    while (atual != NULL) {
        if (atual->id_ingrediente == id_ingrediente) {
            return atual;
        }
        atual = atual->prox;
    }
    return NULL;
}

/* Implementação de ing_listar */
void ing_listar(NoIngrediente* cabeca, const CatalogoIngredientes* cat) {
    if (cabeca == NULL) {
        printf("   (Nenhum ingrediente cadastrado nesta receita)\n");
        return;
    }

    NoIngrediente* atual = cabeca;
    printf("   Ingredientes:\n");
    
    while (atual != NULL) {
        // Busca os detalhes do ingrediente no Catálogo
        IngredienteBase* info = cat_buscar_id(cat, atual->id_ingrediente);

        if (info != NULL) {
            printf("   - [ID: %d] %s: %.2f %s\n", 
                   atual->id_ingrediente, 
                   info->nome, 
                   atual->quantidade, 
                   info->unidade);
        } else {
            // Caso de borda: ID existe na receita mas foi apagado do catálogo
            printf("   - [ID: %d] (Ingrediente desconhecido): %.2f\n", 
                   atual->id_ingrediente, 
                   atual->quantidade);
        }

        atual = atual->prox;
    }
}

/* Implementação de ing_liberar_lista */
void ing_liberar_lista(NoIngrediente** cabeca) {
    if (!cabeca) return;
    NoIngrediente* atual = *cabeca;
    while (atual != NULL) {
        NoIngrediente* temp = atual;
        atual = atual->prox;
        free(temp);
    }
    *cabeca = NULL; // É boa prática zerar o ponteiro da cabeça
}
