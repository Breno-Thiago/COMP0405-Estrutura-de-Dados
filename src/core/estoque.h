#ifndef ESTOQUE_H
#define ESTOQUE_H

#include "catalogo.h"

typedef struct {
    int id_ingrediente;
    float quantidade;
} ItemEstoque;

typedef struct {
    ItemEstoque *itens;
    int qtd_atual;
    int capacidade;
} Estoque;

Estoque *est_inicializar();
void est_liberar(Estoque *est);

int est_buscar_indice(Estoque *est, int id_ingrediente);
void est_listar(const Estoque *est, const CatalogoIngredientes *cat);

void est_adicionar(Estoque *est, int id_ingrediente, float qtd);
int est_remover(Estoque *est, int id_ingrediente, float qtd);
int est_deletar_item(Estoque *est, int id_ingrediente); /* Remove o item completamente do estoque */

#endif
