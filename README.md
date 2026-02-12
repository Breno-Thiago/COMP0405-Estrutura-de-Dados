# Sistema de Gerenciamento de Cozinha Comunitária

Projeto da disciplina **Estrutura de Dados I**

## Sobre

Sistema de gerenciamento via terminal que controla:
- **Catálogo global de ingredientes** (array dinâmico) com ID único
- **Receitas** (array dinâmico) com lista encadeada de ingredientes por ID
- **Estoque** (array dinâmico) controlado por ID de ingrediente
- **Fila de pedidos** (FIFO) para simular produção
- **Processamento transacional** com rollback via pilha (LIFO)
- **Persistência em TXT**

## Estrutura do Repositório

```
/projeto_cozinha
├── README.md
├── Makefile
├── src/
│   ├── main.c
│   ├── app_context.h / app_context.c
│   ├── core/
│   │   ├── utils.h / utils.c
│   │   ├── catalogo.h / catalogo.c
│   │   ├── ingredientes.h / ingredientes.c
│   │   ├── receitas.h / receitas.c
│   │   ├── estoque.h / estoque.c
│   │   ├── pedidos.h / pedidos.c
│   │   ├── rollback.h / rollback.c
│   │   └── persistencia.h / persistencia.c
│   └── ui/
│       └── ui_terminal.h / ui_terminal.c
└── data/
    ├── ingredientes.txt
    ├── receitas.txt
    ├── estoque.txt
    └── pedidos.txt
```

## Como compilar

```bash
make
```

## Como executar

```bash
./cozinha
```

## Documentação

Consulte o arquivo `main.tex` para o manual técnico completo com todas as interfaces, estruturas e regras do projeto.
