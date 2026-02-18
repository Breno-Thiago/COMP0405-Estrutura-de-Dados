#include "app_context.h"
#include <stdlib.h>

AppContext* app_criar() {
    AppContext* app = (AppContext*) malloc(sizeof(AppContext));
    if (!app) return NULL;

    if (!cat_inicializar(&app->cat)) {
        free(app);
        return NULL;
    }

    app->banco = rec_inicializar();
    app->estoque = est_inicializar();
    app->fila = ped_inicializar();

    if (!app->banco || !app->estoque || !app->fila) {
        app_destruir(app);
        return NULL;
    }

    return app;
}

void app_destruir(AppContext* app) {
    if (!app) return;
    if (app->cat) cat_liberar(app->cat);
    if (app->banco) rec_liberar_tudo(app->banco);
    if (app->estoque) est_liberar(app->estoque);
    if (app->fila) ped_liberar(app->fila);
    free(app);
}
