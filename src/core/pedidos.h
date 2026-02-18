#ifndef PEDIDOS_H
#define PEDIDOS_H

#include "receitas.h"
#include "estoque.h"

typedef struct NoPedido {
    int id_pedido;
    Receita* receita;
    struct NoPedido* prox;
} NoPedido;

typedef struct {
    NoPedido* inicio;
    NoPedido* fim;
    int contador_pedidos;
} FilaPedidos;

FilaPedidos* ped_inicializar();
void ped_liberar(FilaPedidos* fila);

void ped_adicionar(FilaPedidos* fila, Receita* receita);
void ped_listar(const FilaPedidos* fila);

// Processa o pedido mais antigo com lógica de rollback
// Retorna 1 sucesso / 0 falha
int ped_processar_proximo(FilaPedidos* fila, Estoque* est);

#endif
