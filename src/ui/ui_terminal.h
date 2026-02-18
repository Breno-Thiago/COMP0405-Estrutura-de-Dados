#ifndef UI_TERMINAL_H
#define UI_TERMINAL_H

#include "../app_context.h"

void ui_loop(AppContext* app);

// Submenus e telas auxiliares
void ui_tela_catalogo(AppContext* app);
void ui_tela_estoque(AppContext* app);
void ui_tela_receitas(AppContext* app);
void ui_tela_pedidos(AppContext* app);

#endif
