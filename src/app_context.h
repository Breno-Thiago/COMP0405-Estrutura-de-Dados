#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include "core/catalogo.h"
#include "core/receitas.h"
#include "core/estoque.h"
#include "core/pedidos.h"

typedef struct {
    CatalogoIngredientes* cat;
    BancoReceitas* banco;
    Estoque* estoque;
    FilaPedidos* fila;
} AppContext;

AppContext* app_criar();
void app_destruir(AppContext* app);

#endif
