CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Isrc

# Identifica o sistema operacional
ifeq ($(OS),Windows_NT)
    EXE = .exe
    RM = del /Q
    NULL_DEV = 2>nul || true
else
    EXE =
    RM = rm -f
    NULL_DEV = 2>/dev/null || true
endif

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

TARGET_TERMINAL = cozinha$(EXE)
TARGET_API = cozinha_api$(EXE)

all: $(TARGET_TERMINAL) $(TARGET_API)

$(TARGET_TERMINAL): $(SRCS_TERMINAL)
	$(CC) $(CFLAGS) -o $@ $^

$(TARGET_API): $(SRCS_API)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) $(TARGET_TERMINAL) $(TARGET_API) $(NULL_DEV)

.PHONY: all clean
