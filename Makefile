CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Isrc

SRCS = src/main.c \
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

OBJS = $(SRCS:.c=.o)
TARGET = cozinha

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /Q src\*.o src\core\*.o src\ui\*.o $(TARGET).exe 2>nul || true

.PHONY: all clean
