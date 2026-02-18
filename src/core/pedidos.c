#include <stdio.h>
#include <stdlib.h>
#include "pedidos.h"
#include "rollback.h"

/* Inicializa a fila de pedidos */
FilaPedidos* ped_inicializar() {
    FilaPedidos* f = (FilaPedidos*) malloc(sizeof(FilaPedidos));
    if (f) {
        f->inicio = NULL;
        f->fim = NULL;
        f->contador_pedidos = 0;
    }
    return f;
}

/* Adiciona um pedido (receita) ao fim da fila */
void ped_adicionar(FilaPedidos* fila, Receita* receita) {
    if (!fila || !receita) return;
    NoPedido* novo = (NoPedido*) malloc(sizeof(NoPedido));
    if (!novo) return;
    
    fila->contador_pedidos++;
    novo->id_pedido = fila->contador_pedidos;
    novo->receita = receita;
    novo->prox = NULL;

    if (fila->fim == NULL) {
        fila->inicio = novo;
        fila->fim = novo;
    } else {
        fila->fim->prox = novo;
        fila->fim = novo;
    }
}

/* Lista os pedidos pendentes na fila */
void ped_listar(const FilaPedidos* fila) {
    if (!fila || !fila->inicio) {
        printf("Fila de pedidos vazia.\n");
        return;
    }
    printf("\n--- FILA DE PEDIDOS (%d pendentes) ---\n", fila->contador_pedidos);
    NoPedido* atual = fila->inicio;
    while (atual) {
        printf("Pedido #%d: %s\n", atual->id_pedido, atual->receita->nome);
        atual = atual->prox;
    }
    printf("--------------------------------------\n");
}

/* 
   Processa o proximo pedido com logica de rollback (transacional).
   Tenta retirar todos os ingredientes; se faltar um, devolve tudo.
*/
int ped_processar_proximo(FilaPedidos* fila, Estoque* est) {
    if (!fila || !fila->inicio || !est) return 0;

    NoPedido* pedido = fila->inicio;
    Receita* r = pedido->receita;
    
    PilhaRollback* rb = rb_criar();
    NoIngrediente* ing = r->ingredientes;
    int sucesso = 1;

    // Transacao: tenta retirar cada ingrediente do estoque
    while (ing) {
        if (est_remover(est, ing->id_ingrediente, ing->quantidade)) {
            // Se retirou, registra na pilha para caso precise desfazer
            rb_push(rb, ing->id_ingrediente, ing->quantidade);
        } else {
            sucesso = 0; // Faltou ingrediente!
            break;
        }
        ing = ing->prox;
    }

    if (!sucesso) {
        // Logica de Rollback: se falhou, devolve o que foi retirado
        printf("Estoque insuficiente para o Pedido #%d (%s).\n", pedido->id_pedido, r->nome);
        rb_desfazer(rb, est);
    } else {
        // Sucesso: pedido concluido, remove da fila
        printf("Pedido #%d (%s) processado com sucesso!\n", pedido->id_pedido, r->nome);
        
        fila->inicio = pedido->prox;
        if (fila->inicio == NULL) {
            fila->fim = NULL;
        }
        free(pedido);
        fila->contador_pedidos--;
    }

    rb_liberar(rb); // Limpa a pilha temporaria
    return sucesso;
}

/* Limpa toda a fila e libera memoria */
void ped_liberar(FilaPedidos* fila) {
    if (!fila) return;
    NoPedido* atual = fila->inicio;
    while (atual) {
        NoPedido* temp = atual;
        atual = atual->prox;
        free(temp);
    }
    free(fila);
}
