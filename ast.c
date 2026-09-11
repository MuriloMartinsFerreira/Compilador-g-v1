#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"

static char *copiar_string(const char *texto)
{
    char *copia;

    if (texto == NULL)
        return NULL;

    copia = malloc(strlen(texto) + 1);

    if (copia == NULL) {
        fprintf(stderr, "ERRO: memoria insuficiente\n");
        exit(1);
    }

    strcpy(copia, texto);

    return copia;
}

AST *ast_create(ASTType type, int line, AST *child1, AST *child2,
                const char *lexeme)
{
    AST *node;

    node = malloc(sizeof(AST));

    if (node == NULL) {
        fprintf(stderr, "ERRO: memoria insuficiente\n");
        exit(1);
    }

    node->type = type;
    node->line = line;
    node->child1 = child1;
    node->child2 = child2;
    node->lexeme = copiar_string(lexeme);

    return node;
}

static const char *ast_type_name(ASTType type)
{
    switch (type) {
        case AST_PROGRAM:        return "PROGRAM";
        case AST_BLOCK:          return "BLOCK";
        case AST_DECL:           return "DECL";
        case AST_LISTA_COMANDOS: return "LISTA_COMANDOS";

        case AST_EMPTY:          return "EMPTY";
        case AST_ASSIGN:         return "ASSIGN";
        case AST_READ:           return "READ";
        case AST_WRITE_EXPR:     return "WRITE_EXPR";
        case AST_WRITE_STRING:   return "WRITE_STRING";
        case AST_NEWLINE:        return "NEWLINE";
        case AST_IF:             return "IF";
        case AST_IF_ELSE:        return "IF_ELSE";
        case AST_WHILE:          return "WHILE";

        case AST_OR:             return "OR";
        case AST_AND:            return "AND";
        case AST_EQ:             return "EQ";
        case AST_NEQ:            return "NEQ";
        case AST_LT:             return "LT";
        case AST_GT:             return "GT";
        case AST_GE:             return "GE";
        case AST_LE:             return "LE";
        case AST_ADD:            return "ADD";
        case AST_SUB:            return "SUB";
        case AST_MUL:            return "MUL";
        case AST_DIV:            return "DIV";
        case AST_NEG:            return "NEG";
        case AST_NOT:            return "NOT";

        case AST_IDENTIFIER:     return "IDENTIFIER";
        case AST_INTCONST:       return "INTCONST";
        case AST_CARCONST:       return "CARCONST";
        case AST_STRING:         return "STRING";

        case AST_TYPE_INT:       return "TYPE_INT";
        case AST_TYPE_CAR:       return "TYPE_CAR";

        default:                 return "UNKNOWN";
    }
}

void ast_print(AST *node, int level)
{
    int i;

    if (node == NULL)
        return;

    for (i = 0; i < level; i++)
        printf("  ");

    printf("%s", ast_type_name(node->type));

    if (node->lexeme != NULL)
        printf(" [%s]", node->lexeme);

    printf(" (linha %d)\n", node->line);

    if (node->child1 != NULL)
        ast_print(node->child1, level + 1);

    if (node->child2 != NULL)
        ast_print(node->child2, level + 1);
}

void ast_free(AST *node)
{
    if (node == NULL)
        return;

    ast_free(node->child1);
    ast_free(node->child2);

    free(node->lexeme);
    free(node);
}