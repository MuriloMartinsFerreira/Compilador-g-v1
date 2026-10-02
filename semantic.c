#include <stdio.h>
#include "semantic.h"
#include "symbol_table.h"

static SymbolTableStack symbol_stack;

static int semantic_error(int line, const char *message)
{
    fprintf(stderr, "ERRO: %s linha %d\n", message, line);
    return 0;
}

static int get_symbol_type(SymbolType type)
{
    if (type == SYMBOL_INT)
        return SYMBOL_INT;

    return SYMBOL_CAR;
}

static int analyze_declarations(AST *node)
{
    SymbolType type;

    if (node == NULL)
        return 1;

    if (node->type == AST_LISTA_DECL) {
        if (!analyze_declarations(node->child1))
            return 0;

        if (!analyze_declarations(node->child2))
            return 0;

        return 1;
    }

    if (node->type == AST_DECL) {
        if (node->child1 == NULL || node->lexeme == NULL)
            return 1;

        if (node->child1->type == AST_TYPE_INT)
            type = SYMBOL_INT;
        else if (node->child1->type == AST_TYPE_CAR)
            type = SYMBOL_CAR;
        else
            return semantic_error(node->line, "TIPO DE DECLARACAO INVALIDO");

        if (!symbol_table_insert(&symbol_stack, node->lexeme, type))
            return semantic_error(
                node->line,
                "IDENTIFICADOR JA DECLARADO NO ESCOPO"
            );

        return 1;
    }

    if (!analyze_declarations(node->child1))
        return 0;

    if (!analyze_declarations(node->child2))
        return 0;

    return 1;
}

static int analyze_expression(AST *node, SymbolType *type);

static int analyze_identifier(AST *node, SymbolType *type)
{
    Symbol *symbol;

    symbol = symbol_table_lookup(&symbol_stack, node->lexeme);

    if (symbol == NULL)
        return semantic_error(
            node->line,
            "IDENTIFICADOR NAO DECLARADO"
        );

    *type = get_symbol_type(symbol->type);

    return 1;
}

static int analyze_expression(AST *node, SymbolType *type)
{
    SymbolType left_type;
    SymbolType right_type;
    Symbol *symbol;

    if (node == NULL)
        return 0;

    switch (node->type) {
        case AST_IDENTIFIER:
            return analyze_identifier(node, type);

        case AST_INTCONST:
            *type = SYMBOL_INT;
            return 1;

        case AST_CARCONST:
            *type = SYMBOL_CAR;
            return 1;

        case AST_ASSIGN:
            if (node->child1 == NULL || node->child2 == NULL)
                return semantic_error(
                    node->line,
                    "ATRIBUICAO INVALIDA"
                );

            if (node->child1->type != AST_IDENTIFIER)
                return semantic_error(
                    node->line,
                    "LADO ESQUERDO DA ATRIBUICAO INVALIDO"
                );

            symbol = symbol_table_lookup(
                &symbol_stack,
                node->child1->lexeme
            );

            if (symbol == NULL)
                return semantic_error(
                    node->child1->line,
                    "IDENTIFICADOR NAO DECLARADO"
                );

            if (!analyze_expression(node->child2, &right_type))
                return 0;

            left_type = get_symbol_type(symbol->type);

            if (left_type != right_type)
                return semantic_error(
                    node->line,
                    "TIPOS INCOMPATIVEIS NA ATRIBUICAO"
                );

            *type = left_type;
            return 1;

        case AST_OR:
        case AST_AND:
            if (!analyze_expression(node->child1, &left_type))
                return 0;

            if (!analyze_expression(node->child2, &right_type))
                return 0;

            if (left_type != SYMBOL_INT || right_type != SYMBOL_INT)
                return semantic_error(
                    node->line,
                    "OPERADOR LOGICO REQUER EXPRESSOES DO TIPO INT"
                );

            *type = SYMBOL_INT;
            return 1;

        case AST_EQ:
        case AST_NEQ:
            if (!analyze_expression(node->child1, &left_type))
                return 0;

            if (!analyze_expression(node->child2, &right_type))
                return 0;

            if (left_type != right_type)
                return semantic_error(
                    node->line,
                    "OPERANDOS DE TIPOS DIFERENTES"
                );

            *type = SYMBOL_INT;
            return 1;

        case AST_LT:
        case AST_GT:
        case AST_GE:
        case AST_LE:
            if (!analyze_expression(node->child1, &left_type))
                return 0;

            if (!analyze_expression(node->child2, &right_type))
                return 0;

            if (left_type != right_type)
                return semantic_error(
                    node->line,
                    "OPERANDOS DE TIPOS DIFERENTES"
                );

            *type = SYMBOL_INT;
            return 1;

        case AST_ADD:
        case AST_SUB:
        case AST_MUL:
        case AST_DIV:
            if (!analyze_expression(node->child1, &left_type))
                return 0;

            if (!analyze_expression(node->child2, &right_type))
                return 0;

            if (left_type != SYMBOL_INT || right_type != SYMBOL_INT)
                return semantic_error(
                    node->line,
                    "OPERADOR ARITMETICO REQUER EXPRESSOES DO TIPO INT"
                );

            *type = SYMBOL_INT;
            return 1;

        case AST_NEG:
            if (!analyze_expression(node->child1, &left_type))
                return 0;

            if (left_type != SYMBOL_INT)
                return semantic_error(
                    node->line,
                    "NEGACAO ARITMETICA REQUER EXPRESSAO DO TIPO INT"
                );

            *type = SYMBOL_INT;
            return 1;

        case AST_NOT:
            if (!analyze_expression(node->child1, &left_type))
                return 0;

            if (left_type != SYMBOL_INT)
                return semantic_error(
                    node->line,
                    "NEGACAO LOGICA REQUER EXPRESSAO DO TIPO INT"
                );

            *type = SYMBOL_INT;
            return 1;

        case AST_LISTA_DECL:
        case AST_DECL:
        case AST_TYPE_INT:
        case AST_TYPE_CAR:
        case AST_PROGRAM:
        case AST_BLOCK:
        case AST_LISTA_COMANDOS:
        case AST_EMPTY:
        case AST_READ:
        case AST_WRITE_EXPR:
        case AST_WRITE_STRING:
        case AST_NEWLINE:
        case AST_IF:
        case AST_IF_ELSE:
        case AST_WHILE:
        case AST_STRING:
            return semantic_error(
                node->line,
                "EXPRESSAO INVALIDA"
            );

        default:
            return semantic_error(
                node->line,
                "EXPRESSAO DESCONHECIDA"
            );
    }
}

static int analyze_command(AST *node);

static int analyze_command_list(AST *node)
{
    if (node == NULL)
        return 1;

    if (node->type == AST_LISTA_COMANDOS) {
        if (!analyze_command(node->child1))
            return 0;

        if (!analyze_command_list(node->child2))
            return 0;

        return 1;
    }

    return analyze_command(node);
}

static int analyze_block(AST *node)
{
    int has_scope;

    if (node == NULL || node->type != AST_BLOCK)
        return 1;

    has_scope = node->child1 != NULL;

    if (has_scope)
        symbol_table_push(&symbol_stack);

    if (!analyze_declarations(node->child1)) {
        if (has_scope)
            symbol_table_pop(&symbol_stack);
        return 0;
    }

    if (!analyze_command_list(node->child2)) {
        if (has_scope)
            symbol_table_pop(&symbol_stack);
        return 0;
    }

    if (has_scope)
        symbol_table_pop(&symbol_stack);

    return 1;
}

static int analyze_command(AST *node)
{
    SymbolType type;
    Symbol *symbol;

    if (node == NULL)
        return 1;

    switch (node->type) {
        case AST_EMPTY:
            return 1;

        case AST_ASSIGN:
            return analyze_expression(node, &type);

        case AST_READ:
            if (node->child1 == NULL ||
                node->child1->type != AST_IDENTIFIER)
                return semantic_error(
                    node->line,
                    "IDENTIFICADOR INVALIDO EM LEIA"
                );

            symbol = symbol_table_lookup(
                &symbol_stack,
                node->child1->lexeme
            );

            if (symbol == NULL)
                return semantic_error(
                    node->child1->line,
                    "IDENTIFICADOR NAO DECLARADO"
                );

            return 1;

        case AST_WRITE_EXPR:
            return analyze_expression(node->child1, &type);

        case AST_WRITE_STRING:
        case AST_NEWLINE:
            return 1;

        case AST_IF:
            if (!analyze_expression(node->child1, &type))
                return 0;

            if (type != SYMBOL_INT)
                return semantic_error(
                    node->child1->line,
                    "CONDICAO DEVE SER DO TIPO INT"
                );

            return analyze_command(node->child2);

        case AST_IF_ELSE:
            if (!analyze_expression(node->child1, &type))
                return 0;

            if (type != SYMBOL_INT)
                return semantic_error(
                    node->child1->line,
                    "CONDICAO DEVE SER DO TIPO INT"
                );

            if (node->child2 == NULL)
                return 1;

            if (!analyze_command(node->child2->child1))
                return 0;

            return analyze_command(node->child2->child2);

        case AST_WHILE:
            if (!analyze_expression(node->child1, &type))
                return 0;

            if (type != SYMBOL_INT)
                return semantic_error(
                    node->child1->line,
                    "CONDICAO DEVE SER DO TIPO INT"
                );

            return analyze_command(node->child2);

        case AST_BLOCK:
            return analyze_block(node);

        default:
            return semantic_error(
                node->line,
                "COMANDO DESCONHECIDO"
            );
    }
}

int semantic_analyze(AST *root)
{
    int result;

    if (root == NULL)
        return 0;

    symbol_table_init(&symbol_stack);

    if (root->type != AST_PROGRAM) {
        symbol_table_free(&symbol_stack);
        return semantic_error(
            root->line,
            "RAIZ DA AST INVALIDA"
        );
    }

    result = analyze_block(root->child1);

    symbol_table_free(&symbol_stack);

    return result;
}