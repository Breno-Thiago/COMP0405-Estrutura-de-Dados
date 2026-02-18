# 🍳 Sistema de Gerenciamento de Cozinha

**Projeto Acadêmico — Estrutura de Dados (UFS)**

O **Sistema de Gerenciamento de Cozinha** é uma plataforma robusta desenvolvida para simular e gerenciar o fluxo operacional de um restaurante industrial. O projeto destaca-se pela sua arquitetura híbrida:

1.  **Motor de Processamento (Core C)**: Uma API de alto desempenho desenvolvida inteiramente em linguagem C, responsável pelo gerenciamento rigoroso das estruturas de dados e garantia da integridade através de transações com lógica de *commit* e *rollback*.
2.  **Interface Moderna (Frontend Web)**: Um dashboard interativo construído com HTML5, CSS3 e JavaScript, focado na experiência do usuário e na visualização em tempo real das operações de estoque e pedidos.
3.  **Comunicação (Bridge Node.js)**: Um servidor de integração que permite a comunicação fluida entre o ambiente Web e o motor de baixo nível em C, traduzindo comandos em alta velocidade.

---

## 🚀 Como Executar (Guia Rápido)

### 1. Requisitos
- Compilador **GCC** instalado.
- **Node.js** (versão 18+) para a interface web.

### 2. Compilação
Acesse a pasta raiz do projeto no terminal e execute:
```bash
# Se tiver 'make' instalado:
make

# Ou manualmente (Windows/Linux):
gcc -Isrc src/api.c src/app_context.c src/core/*.c -o cozinha_api
gcc -Isrc src/main.c src/app_context.c src/core/*.c src/ui/*.c -o cozinha
```

### 3. Rodar a Interface Web
```bash
node server.js
```
Abra o navegador em: `http://localhost:3000`

---

## 📚 Estruturas de Dados Obrigatórias

O sistema foi projetado para demonstrar o uso prático de todas as estruturas fundamentais da disciplina:

### 🧩 1. Structs (Modelagem e Ponteiros)
Toda a lógica é baseada na manipulação de **Structs** para representar as entidades. Utilizamos **Ponteiros** intensamente para navegar entre os nós e para a **Alocação Dinâmica** (`malloc`/`free`), garantindo que o sistema suporte qualquer volume de dados sem desperdício de memória.
- *Onde:* `NoIngrediente`, `Receita`, `Pedido`, `ItemCatalogo`.

### 📦 2. Vetores Dinâmicos (Array)
O **Estoque** e o **Catálogo** são gerenciados por vetores que crescem sob demanda. Isso permite acesso rápido via índice aos ingredientes básicos.
- *Onde:* Módulos `catalogo.c` e `estoque.c`.

### 🔗 3. Listas Ligadas
Diferente do catálogo fixo, cada **Receita** possui uma **Lista Encadeada** de ingredientes. Isso foi escolhido porque cada prato tem um número imprevisível de itens, e a lista permite gerenciar esse conjunto de forma flexível.
- *Onde:* Módulo `ingredientes.c`.

### ⏳ 4. Fila (Queue — FIFO)
O processamento de pedidos segue a regra "Primeiro a Chegar, Primeiro a ser Atendido". Os pedidos feitos pelo site entram em uma fila persistente.
- *Onde:* Módulo `pedidos.c`.

### 🔄 5. Pilha (Stack — LIFO)
A joia do projeto: o **Rollback Transacional**. Quando um pedido complexo começa a ser processado, cada item retirado do estoque é "empilhado". Se um ingrediente faltar no meio do caminho, o sistema dá um `POP` em tudo e devolve ao estoque, evitando que a cozinha fique com dados inconsistentes.
- *Onde:* Módulo `rollback.c`.

---

## ⚙️ Fluxo de Funcionamento (Integração Total)

Para entender como as estruturas conversam entre si, veja o caminho de um pedido:

1.  **Entrada**: O pedido chega e é inserido na **Fila (FIFO)**.
2.  **Consulta**: O sistema busca a **Struct** da receita no **Vetor/Array** de receitas.
3.  **Verificação**: O motor percorre a **Lista Encadeada** de ingredientes daquela receita.
4.  **Reserva**: Para cada item, usa-se **Ponteiros** para alterar o estoque. Cada sucesso é armazenado na **Pilha (LIFO)**.
5.  **Finalização**: Se tudo der certo, a pilha é limpa. Se algo faltar, a pilha desempilha e restaura o estoque original.

---

## 👥 Integrantes (Grupo UFS)

- **HELEN DA SILVA BISPO**
- **JOÃO VICTOR CARVALHO SIMÕES**
- **BRENO THIAGO ARGEMIRO SANTOS**
- **GABRIEL FERREIRA BERNARDO**
- **CAIO MAGNO BRASIL SANTOS DE CARVALHO LEITE**
- **LUCAS OLIVEIRA TELES CAVALCANTE**
