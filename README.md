# 🍳 Sistema de Gerenciamento de Cozinha

**Projeto acadêmico — Estrutura de Dados (UFS)**

Sistema completo de gerenciamento de receitas, estoque e pedidos de cozinha, desenvolvido em **C** com interface web (**HTML/CSS/JS + Node.js**).

---

## 📚 Estruturas de Dados Utilizadas

| Estrutura | Uso no projeto | Módulo |
|---|---|---|
| **Lista Ligada** | Ingredientes de cada receita | `ingredientes.c` |
| **Vetor Dinâmico** | Catálogo de ingredientes e estoque | `catalogo.c`, `estoque.c` |
| **Fila (FIFO)** | Fila de pedidos (primeiro pedido = primeiro processado) | `pedidos.c` |
| **Pilha (LIFO)** | Rollback transacional ao processar pedidos | `rollback.c` |
| **Tabela Hash** *(via busca)* | Busca rápida por ID no catálogo | `catalogo.c` |

### 🔄 Pilha de Rollback — Como funciona

Ao processar um pedido, o sistema **retira ingredientes do estoque** um a um. Cada retirada é empilhada (**PUSH**) na pilha de rollback:

```
Processando "Bolo de Chocolate":
  PUSH → Farinha (2kg)    ← retirou do estoque
  PUSH → Ovos (4un)       ← retirou do estoque
  PUSH → Leite (500ml)    ← retirou do estoque
  ✗ Chocolate em Pó → INSUFICIENTE!
```

Se algum ingrediente faltar, o sistema faz **rollback** — desempilha (**POP**) e devolve tudo ao estoque:

```
  POP → Leite (500ml)     ← devolveu ao estoque
  POP → Ovos (4un)        ← devolveu ao estoque
  POP → Farinha (2kg)     ← devolveu ao estoque
```

A interface web **visualiza cada operação** PUSH/POP em tempo real com um modal detalhado.

---

## 🏗️ Arquitetura

```
┌──────────┐     HTTP      ┌──────────┐    stdin/stdout    ┌─────────────┐
│  Browser │ ←──────────→  │ Node.js  │ ←────────────────→ │ cozinha_api │
│ (HTML/JS)│               │(server.js)│      JSON          │    (C)      │
└──────────┘               └──────────┘                    └─────────────┘
                                                                  │
                                                           ┌──────┴──────┐
                                                           │  data/*.txt │
                                                           └─────────────┘
```

O **core em C** é responsável por toda a lógica de negócio. O Node.js serve como ponte HTTP, encaminhando comandos via `stdin` e recebendo respostas JSON via `stdout`.

---

## 📁 Estrutura do Projeto

```
projeto_cozinha/
├── src/
│   ├── core/              # Módulos centrais em C
│   │   ├── catalogo.c/h   # Vetor dinâmico de ingredientes
│   │   ├── estoque.c/h    # Gerenciamento de estoque
│   │   ├── receitas.c/h   # Banco de receitas + lista ligada
│   │   ├── pedidos.c/h    # Fila FIFO de pedidos
│   │   ├── rollback.c/h   # Pilha de rollback
│   │   ├── persistencia.c/h # Persistência em .txt
│   │   ├── ingredientes.c/h # Lista ligada de ingredientes
│   │   └── utils.c/h      # Utilitários
│   ├── ui/
│   │   └── ui_terminal.c/h # Interface terminal (modo CLI)
│   ├── api.c              # Camada API (protocolo JSON via stdin/stdout)
│   ├── main.c             # Ponto de entrada do modo terminal
│   └── app_context.c/h    # Contexto global da aplicação
├── interface_web/
│   ├── index.html         # Página principal
│   ├── style.css          # Estilos (dark theme)
│   └── app.js             # Lógica do frontend
├── data/                  # Dados persistidos
│   ├── ingredientes.txt   # Catálogo de ingredientes
│   ├── estoque.txt        # Quantidades em estoque
│   ├── receitas.txt       # Receitas e seus ingredientes
│   └── pedidos.txt        # Fila de pedidos
├── server.js              # Servidor Node.js (ponte HTTP ↔ C)
├── Makefile               # Build do projeto
└── README.md
```

---

## 🚀 Como Executar

### Pré-requisitos

- **GCC** (MinGW no Windows)
- **Node.js** (v18+)

### Compilar

```bash
# Compilar ambos (terminal + api)
make

# Ou compilar manualmente:
gcc -Isrc src/api.c src/app_context.c src/core/*.c -o cozinha_api
gcc -Isrc src/main.c src/app_context.c src/core/*.c src/ui/*.c -o cozinha
```

### Modo Web (Dashboard)

```bash
node server.js
# Acesse: http://localhost:3000
```

### Modo Terminal (CLI)

```bash
./cozinha
```

---



## 👥 Integrantes

Grupo de Estrutura de Dados — Universidade Federal de Sergipe (UFS)

- **HELEN DA SILVA BISPO**
- **JOÃO VICTOR CARVALHO SIMÕES**
- **BRENO THIAGO ARGEMIRO SANTOS**
- **GABRIEL FERREIRA BERNARDO**
- **CAIO MAGNO BRASIL SANTOS DE CARVALHO LEITE**
- **LUCAS OLIVEIRA TELES CAVALCANTE**
