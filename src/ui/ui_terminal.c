#include "ui_terminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../core/utils.h"

static void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void ui_loop(AppContext* app) {
    int opcao = -1;
    while (opcao != 0) {
        printf("\n========================================\n");
        printf("    GERENCIAMENTO DE COZINHA COMUNITARIA\n");
        printf("========================================\n");
        printf("1. Catalogo de Ingredientes (Base)\n");
        printf("2. Estoque (Armazem)\n");
        printf("3. Banco de Receitas\n");
        printf("4. Fila de Pedidos\n");
        printf("0. Sair e Salvar\n");
        printf("========================================\n");
        printf("Escolha uma opcao: ");
        if (scanf("%d", &opcao) != 1) {
            limpar_buffer();
            continue;
        }

        switch (opcao) {
            case 1: ui_tela_catalogo(app); break;
            case 2: ui_tela_estoque(app); break;
            case 3: ui_tela_receitas(app); break;
            case 4: ui_tela_pedidos(app); break;
            case 0: printf("Finalizando sistema...\n"); break;
            default: printf("Opcao invalida!\n");
        }
    }
}

/* --- TELA: CATALOGO --- */
void ui_tela_catalogo(AppContext* app) {
    int sub = -1;
    while (sub != 0) {
        printf("\n--- CATALOGO DE INGREDIENTES ---\n");
        printf("1. Listar ingredientes\n");
        printf("2. Cadastrar novo ingrediente\n");
        printf("3. Editar ingrediente\n");
        printf("4. Remover ingrediente\n");
        printf("0. Voltar\n");
        printf("Opcao: ");
        scanf("%d", &sub);
        limpar_buffer();

        char nome[100], unidade[20];
        int id;

        switch (sub) {
            case 1:
                cat_listar(app->cat);
                break;
            case 2:
                printf("Nome do ingrediente: ");
                fgets(nome, sizeof(nome), stdin); utl_chomp(nome);
                printf("Unidade (g, ml, un, etc): ");
                fgets(unidade, sizeof(unidade), stdin); utl_chomp(unidade);
                id = cat_cadastrar(app->cat, nome, unidade);
                if (id) printf("Cadastrado com sucesso! ID: %d\n", id);
                else printf("Erro ao cadastrar.\n");
                break;
            case 3:
                printf("ID do ingrediente para editar: ");
                scanf("%d", &id); limpar_buffer();
                printf("Novo nome (Enter para manter): ");
                fgets(nome, sizeof(nome), stdin); utl_chomp(nome);
                printf("Nova unidade (Enter para manter): ");
                fgets(unidade, sizeof(unidade), stdin); utl_chomp(unidade);
                if (cat_editar(app->cat, id, strlen(nome) > 0 ? nome : NULL, strlen(unidade) > 0 ? unidade : NULL))
                    printf("Editado com sucesso.\n");
                else printf("ID nao encontrado.\n");
                break;
            case 4:
                printf("ID do ingrediente para remover: ");
                scanf("%d", &id); limpar_buffer();
                if (cat_remover(app->cat, id)) printf("Removido com sucesso.\n");
                else printf("ID nao encontrado ou em uso.\n");
                break;
        }
    }
}

/* --- TELA: ESTOQUE --- */
void ui_tela_estoque(AppContext* app) {
    int sub = -1;
    while (sub != 0) {
        printf("\n--- ESTOQUE (ARMAZEM) ---\n");
        printf("1. Ver estoque atual\n");
        printf("2. Entrada de itens (Comprar/Repor)\n");
        printf("3. Saida manual de itens\n");
        printf("0. Voltar\n");
        printf("Opcao: ");
        scanf("%d", &sub);
        limpar_buffer();

        int id;
        float qtd;

        switch (sub) {
            case 1:
                est_listar(app->estoque, app->cat);
                break;
            case 2:
                cat_listar(app->cat);
                printf("Selecione o ID do ingrediente: ");
                scanf("%d", &id);
                printf("Quantidade a adicionar: ");
                scanf("%f", &qtd); limpar_buffer();
                est_adicionar(app->estoque, id, qtd);
                printf("Estoque atualizado.\n");
                break;
            case 3:
                est_listar(app->estoque, app->cat);
                printf("ID do ingrediente: ");
                scanf("%d", &id);
                printf("Quantidade a retirar: ");
                scanf("%f", &qtd); limpar_buffer();
                if (est_remover(app->estoque, id, qtd)) printf("Retirada realizada.\n");
                else printf("Falha: Quantidade insuficiente ou ID inexistente.\n");
                break;
        }
    }
}

/* --- TELA: RECEITAS --- */
void ui_tela_receitas(AppContext* app) {
    int sub = -1;
    while (sub != 0) {
        printf("\n--- BANCO DE RECEITAS ---\n");
        printf("1. Listar receitas\n");
        printf("2. Criar nova receita\n");
        printf("3. Adicionar ingrediente a uma receita\n");
        printf("4. Remover receita\n");
        printf("0. Voltar\n");
        printf("Opcao: ");
        scanf("%d", &sub);
        limpar_buffer();

        char nome[100], preparo[500];
        int id_rec, id_ing;
        float qtd;

        switch (sub) {
            case 1:
                rec_listar(app->banco, app->cat);
                break;
            case 2:
                printf("Nome da receita: ");
                fgets(nome, sizeof(nome), stdin); utl_chomp(nome);
                printf("Modo de preparo: ");
                fgets(preparo, sizeof(preparo), stdin); utl_chomp(preparo);
                int id = rec_cadastrar(app->banco, nome, preparo);
                printf("Receita criada com ID: %d (Agora adicione ingredientes nela)\n", id);
                break;
            case 3:
                rec_listar(app->banco, app->cat);
                printf("ID da Receita: ");
                scanf("%d", &id_rec);
                cat_listar(app->cat);
                printf("ID do Ingrediente: ");
                scanf("%d", &id_ing);
                printf("Quantidade necessaria: ");
                scanf("%f", &qtd); limpar_buffer();
                if (rec_add_ingrediente(app->banco, id_rec, id_ing, qtd))
                    printf("Ingrediente adicionado a receita.\n");
                else printf("Erro: Receita nao encontrada.\n");
                break;
            case 4:
                printf("ID da receita para remover: ");
                scanf("%d", &id_rec); limpar_buffer();
                if (rec_remover(app->banco, id_rec)) printf("Receita excluida.\n");
                else printf("Receita nao encontrada.\n");
                break;
        }
    }
}

/* --- TELA: PEDIDOS --- */
void ui_tela_pedidos(AppContext* app) {
    int sub = -1;
    while (sub != 0) {
        printf("\n--- FILA DE PEDIDOS / PRODUCAO ---\n");
        printf("1. Listar pedidos pendentes\n");
        printf("2. Novo pedido (Enfileirar)\n");
        printf("3. Processar proximo pedido (Cozinhar)\n");
        printf("0. Voltar\n");
        printf("Opcao: ");
        scanf("%d", &sub);
        limpar_buffer();

        int id_rec;

        switch (sub) {
            case 1:
                ped_listar(app->fila);
                break;
            case 2:
                rec_listar(app->banco, app->cat);
                printf("ID da Receita para o pedido: ");
                scanf("%d", &id_rec); limpar_buffer();
                Receita* r = rec_buscar_id(app->banco, id_rec);
                if (r) {
                    ped_adicionar(app->fila, r);
                    printf("Pedido adicionado a fila.\n");
                } else printf("Receita nao encontrada.\n");
                break;
            case 3:
                if (ped_processar_proximo(app->fila, app->estoque)) {
                    // Mensagem de sucesso ja impressa pela funcao core
                }
                break;
        }
    }
}
