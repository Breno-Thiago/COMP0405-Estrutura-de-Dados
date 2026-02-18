# 🍳 Sistema de Gerenciamento de Cozinha

**Projeto Acadêmico — Estrutura de Dados (UFS)**

O **Sistema de Gerenciamento de Cozinha** é uma aplicação completa desenvolvida para simular a operação de uma cozinha industrial ou restaurante. O sistema integra o gerenciamento de um catálogo de insumos, controle de estoque em tempo real, cadastro de receitas complexas e uma fila de processamento de pedidos com suporte a rollback transacional.

O projeto foi construído utilizando a linguagem **C** para o núcleo de processamento (Core) e uma interface moderna baseada em tecnologias web (**HTML, CSS, JavaScript e Node.js**), permitindo uma visualização clara das operações de baixo nível.

---

## 🚀 Como Executar

### Pré-requisitos
- **GCC** (Compilador C)
- **Node.js** (v18 ou superior)

### 1. Compilação
O projeto utiliza um `Makefile` para automatizar o processo de build. No terminal, execute:
```bash
make
```
Isso gerará dois executáveis: `cozinha` (modo terminal) e `cozinha_api` (motor para a web).

### 2. Execução (Interface Web)
Para rodar a interface gráfica (Dashboard):
```bash
node server.js
```
Acesse no navegador: `http://localhost:3000`

### 3. Modo Terminal (Opcional)
Para interagir diretamente com o sistema via linha de comando:
```bash
./cozinha
```

---

## 📚 Estruturas de Dados (Requisitos da Matéria)

O projeto implementa rigorosamente todas as estruturas de dados obrigatórias da disciplina, aplicando cada uma no cenário onde sua eficiência é máxima:

1.  **Structs (Estruturas de Dados)**:
    Utilizadas para a modelagem de todas as entidades do sistema (`Ingrediente`, `Receita`, `Pedido`, `NoIngrediente`). As structs permitem agrupar diferentes tipos de dados sob uma única entidade lógica.
    *Exemplo:* O `NoIngrediente` agrupa o ID, a quantidade e o ponteiro para o próximo nó.

2.  **Array / Vetor Dinâmico**:
    Utilizado no **Catálogo de Ingredientes** e no **Estoque**. Permite o acesso por índice e o redimensionamento dinâmico da memória conforme novos insumos são cadastrados.

3.  **Ponteiros com Alocação Dinâmica (`malloc`/`free`)**:
    Essencial para a gestão eficiente de memória. Todos os elementos do sistema (nós de listas, itens da fila, elementos da pilha) são alocados dinamicamente, garantindo que o programa utilize apenas a memória necessária e a libere corretamente após o uso.

4.  **Lista Encadeada**:
    Implementada para gerenciar os **Ingredientes de uma Receita**. Como uma receita pode ter um número variável de ingredientes, a lista encadeada permite inserções e remoções dinâmicas sem a necessidade de realocação de grandes blocos de memória.

5.  **Pilha (Stack — LIFO)**:
    Utilizada para o **Mecanismo de Rollback Transacional**. Ao processar um pedido, cada ingrediente retirado do estoque é empilhado (`PUSH`). Se o processamento falhar por falta de algum insumo posterior, o sistema desempilha (`POP`) os itens e os devolve ao estoque, garantindo a integridade dos dados.

6.  **Fila (Queue — FIFO)**:
    Gerencia a **Fila de Pedidos**. Garante que as solicitações de pratos sejam processadas rigorosamente na ordem em que foram recebidas (o primeiro pedido a entrar é o primeiro a ser processado).

---

## 🛠️ Especificações Técnicas

### Arquitetura do Sistema
O sistema opera em uma arquitetura de camadas, separando a lógica de estruturas de dados da interface de usuário:

```
┌──────────┐     HTTP      ┌──────────┐    stdin/stdout    ┌─────────────┐
│  Browser │ ←──────────→  │ Node.js  │ ←────────────────→ │ cozinha_api │
│ (HTML/JS)│               │(server.js)│      JSON          │    (C)      │
└──────────┘               └──────────┘                    └─────────────┘
```

### Divisão de Módulos
-   `src/core/`: Implementação robusta das estruturas de dados (Lista, Fila, Pilha, Vetor).
-   `src/api.c`: Camada de tradução que comunica o motor em C com o mundo exterior via JSON.
-   `server.js`: Servidor de ponte que gerencia os processos do sistema operacional e a comunicação via WebSockets/HTTP.

---

## 👥 Integrantes

Grupo de Estrutura de Dados — Universidade Federal de Sergipe (UFS)

-   **HELEN DA SILVA BISPO**
-   **JOÃO VICTOR CARVALHO SIMÕES**
-   **BRENO THIAGO ARGEMIRO SANTOS**
-   **GABRIEL FERREIRA BERNARDO**
-   **CAIO MAGNO BRASIL SANTOS DE CARVALHO LEITE**
-   **LUCAS OLIVEIRA TELES CAVALCANTE**
