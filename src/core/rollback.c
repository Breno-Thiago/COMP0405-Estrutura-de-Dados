#include <stdlib.h>
#include "rollback.h"
#include "estoque.h"

/* Inicializa a pilha */
PilhaRollback* rb_criar() {
    PilhaRollback* rb = (PilhaRollback*) malloc(sizeof(PilhaRollback));
    if (rb) {
        rb->topo = NULL;
    }
    return rb;
}

/* Adiciona um item no topo da pilha */
void rb_push(PilhaRollback* rb, int id_ingrediente, float qtd) {
    if (!rb) return;
    NoRollback* novo = (NoRollback*) malloc(sizeof(NoRollback));
    if (novo) {
        novo->id_ingrediente = id_ingrediente;
        novo->qtd = qtd;
        novo->abaixo = rb->topo;
        rb->topo = novo;
    }
}

/* Remove e retorna o item do topo */
int rb_pop(PilhaRollback* rb, int* out_id, float* out_qtd) {
    if (!rb || !rb->topo) return 0;
    NoRollback* aux = rb->topo;
    if (out_id) *out_id = aux->id_ingrediente;
    if (out_qtd) *out_qtd = aux->qtd;
    rb->topo = aux->abaixo;
    free(aux);
    return 1;
}

/* Desfaz as operacoes: devolve tudo ao estoque e limpa a pilha */
void rb_desfazer(PilhaRollback* pilha, Estoque* est) {
    if (!pilha || !est) return;
    int id;
    float qtd;
    while (rb_pop(pilha, &id, &qtd)) {
        est_adicionar(est, id, qtd);
    }
}

/* Limpa a pilha sem mexer no estoque */
void rb_limpar(PilhaRollback* rb) {
    if (!rb) return;
    int id;
    float qtd;
    while (rb_pop(rb, &id, &qtd));
}

/* Libera a memoria da estrutura */
void rb_liberar(PilhaRollback* rb) {
    if (!rb) return;
    rb_limpar(rb);
    free(rb);
}
