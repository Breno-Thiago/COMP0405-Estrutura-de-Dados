#include "app_context.h"
#include "ui/ui_terminal.h"
#include "core/persistencia.h"
#include <stdio.h>

int main() {
    AppContext* app = app_criar();
    
    if (!app) {
        printf("Erro ao inicializar o sistema.\n");
        return 1;
    }

    // Carrega dados existentes
    pers_carregar_catalogo(app->cat);
    pers_carregar_receitas(app->banco);
    pers_carregar_estoque(app->estoque);
    pers_carregar_pedidos(app->fila, app->banco);

    // Inicia o loop da interface
    ui_loop(app);

    // Salva antes de sair
    pers_salvar_catalogo(app->cat);
    pers_salvar_receitas(app->banco);
    pers_salvar_estoque(app->estoque);
    pers_salvar_pedidos(app->fila);

    // Destroi e libera memoria
    app_destruir(app);

    return 0;
}
