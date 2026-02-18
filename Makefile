CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Isrc

# ── Terminal interativo ────────────────────────────────
SRCS_TERMINAL = src/main.c \
       src/app_context.c \
       src/core/utils.c \
       src/core/catalogo.c \
       src/core/ingredientes.c \
       src/core/receitas.c \
       src/core/estoque.c \
       src/core/pedidos.c \
       src/core/rollback.c \
       src/core/persistencia.c \
       src/ui/ui_terminal.c

# ── API Web (usada pelo server.js) ─────────────────────
SRCS_API = src/api.c \
       src/app_context.c \
       src/core/utils.c \
       src/core/catalogo.c \
       src/core/ingredientes.c \
       src/core/receitas.c \
       src/core/estoque.c \
       src/core/pedidos.c \
       src/core/rollback.c \
       src/core/persistencia.c

TARGET_TERMINAL = cozinha
TARGET_API = cozinha_api

all: $(TARGET_TERMINAL) $(TARGET_API)

$(TARGET_TERMINAL): $(SRCS_TERMINAL)
	$(CC) $(CFLAGS) -o $@ $^

$(TARGET_API): $(SRCS_API)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	del /Q $(TARGET_TERMINAL).exe $(TARGET_API).exe 2>nul || true

.PHONY: all clean
