#ifndef RECEITAS_H
#define RECEITAS_H

#include "ingredientes.h"

typedef struct {
    int id;
    char* nome;
    char* modo_preparo;
    NoIngrediente* ingredientes; // Cabeça da lista encadeada
} Receita;

typedef struct {
    Receita** vetor;
    int qtd_atual;
    int capacidade;
    int prox_id;
} BancoReceitas;

// Inicializa o banco (Array Dinamico)
BancoReceitas* rec_inicializar();

// Libera memoria total
void rec_liberar_tudo(BancoReceitas* banco);

// CRUD e operacoes de receitas
int rec_cadastrar(BancoReceitas* banco, const char* nome, const char* preparo);
Receita* rec_buscar_id(const BancoReceitas* banco, int id);
int rec_remover(BancoReceitas* banco, int id);

// Listagem e edicao
void rec_listar(const BancoReceitas* banco, const CatalogoIngredientes* cat);
int rec_editar_nome(BancoReceitas* banco, int id, const char* novo_nome);
int rec_editar_preparo(BancoReceitas* banco, int id, const char* novo_preparo);

// Gerencia ingredientes dentro da receita
int rec_add_ingrediente(BancoReceitas* banco, int id_receita, int id_ingrediente, float qtd);
int rec_rem_ingrediente(BancoReceitas* banco, int id_receita, int id_ingrediente);

#endif
