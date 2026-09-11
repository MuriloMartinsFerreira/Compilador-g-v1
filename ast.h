#ifndef AST_H
#define AST_H

typedef enum {
    AST_PROGRAM,
    AST_BLOCK,
    AST_DECL,
    AST_LISTA_COMANDOS,

    AST_EMPTY,
    AST_ASSIGN,
    AST_READ,
    AST_WRITE_EXPR,
    AST_WRITE_STRING,
    AST_NEWLINE,
    AST_IF,
    AST_IF_ELSE,
    AST_WHILE,

    AST_OR,
    AST_AND,
    AST_EQ,
    AST_NEQ,
    AST_LT,
    AST_GT,
    AST_GE,
    AST_LE,
    AST_ADD,
    AST_SUB,
    AST_MUL,
    AST_DIV,
    AST_NEG,
    AST_NOT,

    AST_IDENTIFIER,
    AST_INTCONST,
    AST_CARCONST,
    AST_STRING,

    AST_TYPE_INT,
    AST_TYPE_CAR
} ASTType;

typedef struct AST {
    ASTType type;
    int line;
    char *lexeme;

    struct AST *child1;
    struct AST *child2;
} AST;

AST *ast_create(ASTType type, int line, AST *child1, AST *child2,
                const char *lexeme);

void ast_print(AST *node, int level);

void ast_free(AST *node);

#endif