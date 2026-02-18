#ifndef CATALOGO_H
#define CATALOGO_H

#include <stddef.h>

/* 
 * Tipo que representa um item do catalogo global.
 * IDs sao unicos e resolvidos pelo sistema para evitar divergencia de nomes.
 */
typedef struct {
    int id;         // id unico gerado por prox_id (>=1)
    char *nome;     // string alocada dinamicamente
    char *unidade;  // string alocada dinamicamente (ex: "g", "un", "ml")
} IngredienteBase;

/* 
 * Estrutura que guarda todo o catalogo (array dinamico).
 */
typedef struct {
    IngredienteBase *itens;
    size_t qtd_atual;
    size_t capacidade;
    int prox_id;
} CatalogoIngredientes;

// Inicializa catalogo.
int cat_inicializar(CatalogoIngredientes **cat);

// Libera toda memoria associada ao catalogo
void cat_liberar(CatalogoIngredientes *cat);

// Adiciona item (nome, unidade). Retorna id (>0) ou 0 se falha
int cat_cadastrar(CatalogoIngredientes *cat, const char *nome, const char *unidade);

// Edita item: retorna 1 sucesso, 0 falha (id nao existe)
int cat_editar(CatalogoIngredientes *cat, int id, const char *novo_nome, const char *nova_unidade);

// Remove item por id. Retorna 1 sucesso, 0 falha
int cat_remover(CatalogoIngredientes *cat, int id);

// Retorna ponteiro para IngredienteBase pelo id, ou NULL se nao encontrar.
IngredienteBase *cat_buscar_id(const CatalogoIngredientes *cat, int id);

// Busca id por nome (comparacao exata). Retorna id ou -1 se nao encontrado
int cat_buscar_nome(const CatalogoIngredientes *cat, const char *nome);

// Acessores convenientes: retornam const char* (ou NULL se nao existir)
const char *cat_get_nome(const CatalogoIngredientes *cat, int id);
const char *cat_get_unidade(const CatalogoIngredientes *cat, int id);

// Lista o catalogo no stdout
void cat_listar(const CatalogoIngredientes *cat);

#endif // CATALOGO_H
