#ifndef ROLLBACK_H
#define ROLLBACK_H

#include "estoque.h"

/* 
 * Estrutura do Nó da Pilha de Rollback.
 * Armazena a quantidade que foi retirada para caso de erro.
 */
typedef struct NoRollback {
    int id_ingrediente;
    float qtd;
    struct NoRollback* abaixo;
} NoRollback;

typedef struct {
    NoRollback* topo;
} PilhaRollback;

PilhaRollback* rb_criar();
void rb_liberar(PilhaRollback* rb);

void rb_push(PilhaRollback* rb, int id_ingrediente, float qtd);
int  rb_pop(PilhaRollback* rb, int* out_id, float* out_qtd);
void rb_limpar(PilhaRollback* rb);

/* 
 * Função original sugerida pelo grupo para desfazer as operações
 * percorrendo a pilha e devolvendo ao estoque.
 */
void rb_desfazer(PilhaRollback* pilha, Estoque* est);

#endif
