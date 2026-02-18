#ifndef INGREDIENTES_H
#define INGREDIENTES_H

/* 
 * Dependencia: O modulo de ingredientes precisa conhecer o Catalogo 
 * para a funcao de listagem (para mostrar nomes em vez de apenas IDs).
 */
#include "catalogo.h"

/* 
 * Estrutura do No da Lista Encadeada
 * Armazena apenas o ID e a quantidade. O nome vem do catalogo.
 */
typedef struct NoIngrediente {
    int id_ingrediente;
    float quantidade;           // Quantidade necessaria para a receita
    struct NoIngrediente* prox; // Ponteiro para o proximo ingrediente
} NoIngrediente;

/* Cria um novo no isolado (funcao auxiliar) */
NoIngrediente* ing_criar_no(int id_ingrediente, float qtd);

/* 
 * Adiciona um ingrediente a lista.
 * Se o ID ja existir, ATUALIZA a quantidade.
 * Se nao existir, insere no inicio (mais eficiente).
 */
void ing_adicionar(NoIngrediente** cabeca, int id_ingrediente, float qtd);

/* 
 * Remove um ingrediente da lista pelo ID.
 * Retorna 1 se sucesso, 0 se nao encontrou.
 */
int ing_remover(NoIngrediente** cabeca, int id_ingrediente);

/* 
 * Busca um no especifico pelo ID.
 * Retorna o ponteiro ou NULL se nao achar.
 */
NoIngrediente* ing_buscar(NoIngrediente* cabeca, int id_ingrediente);

/* 
 * Lista os ingredientes da receita.
 * Usa o ponteiro 'cat' para buscar o nome e a unidade de cada ID.
 */
void ing_listar(NoIngrediente* cabeca, const CatalogoIngredientes* cat);

/* Libera toda a memoria da lista encadeada */
void ing_liberar_lista(NoIngrediente** cabeca);

#endif
