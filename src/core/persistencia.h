#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include "catalogo.h"
#include "receitas.h"
#include "estoque.h"
#include "pedidos.h"

#define PATH_INGREDIENTES "data/ingredientes.txt"
#define PATH_RECEITAS     "data/receitas.txt"
#define PATH_ESTOQUE      "data/estoque.txt"
#define PATH_PEDIDOS      "data/pedidos.txt"

int pers_salvar_catalogo(const CatalogoIngredientes* cat);
int pers_carregar_catalogo(CatalogoIngredientes* cat);

int pers_salvar_receitas(const BancoReceitas* banco);
int pers_carregar_receitas(BancoReceitas* banco);

int pers_salvar_estoque(const Estoque* estoque);
int pers_carregar_estoque(Estoque* estoque);

int pers_salvar_pedidos(const FilaPedidos* fila);
int pers_carregar_pedidos(FilaPedidos* fila, BancoReceitas* banco);

#endif
