%code requires {
#include "ast.h"

typedef struct {
    char *lexeme;
    int line;
} TokenAttr;
}

%{
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"

extern int yylex(void);
extern int yylineno;
extern char *yytext;
extern FILE *yyin;

void yyerror(const char *s);

AST *raiz;

static AST *criar_lista_decl(AST *nomes, AST *tipo)
{
    AST *resultado = NULL;
    AST *atual = nomes;

    while (atual != NULL) {
        AST *nome = atual->child1;
        AST *proximo = atual->child2;

        AST *tipo_novo = ast_create(
            tipo->type,
            tipo->line,
            NULL,
            NULL,
            tipo->lexeme
        );

        AST *decl = ast_create(
            AST_DECL,
            nome->line,
            tipo_novo,
            NULL,
            nome->lexeme
        );

        if (resultado == NULL) {
            resultado = ast_create(
                AST_LISTA_DECL,
                decl->line,
                decl,
                NULL,
                NULL
            );
        } else {
            AST *novo = ast_create(
                AST_LISTA_DECL,
                decl->line,
                decl,
                NULL,
                NULL
            );

            AST *fim = resultado;

            while (fim->child2 != NULL)
                fim = fim->child2;

            fim->child2 = novo;
        }

        free(nome->lexeme);
        free(nome);
        free(atual);
        atual = proximo;
    }

    ast_free(tipo);

    return resultado;
}

static AST *criar_lista_nomes(AST *nome, AST *resto)
{
    return ast_create(AST_LISTA_DECL, nome->line, nome, resto, NULL);
}
%}

%union {
    int line;
    TokenAttr token;
    AST *node;
}

%token <line> PRINCIPAL
%token <token> IDENTIFICADOR
%token <line> INT
%token <line> CAR
%token <line> LEIA
%token <line> ESCREVA
%token <line> NOVALINHA
%token <line> SE
%token <line> ENTAO
%token <line> SENAO
%token <line> FIMSE
%token <line> ENQUANTO
%token <token> CADEIACARACTERES
%token <token> CARCONST
%token <token> INTCONST
%token <line> OU
%token <line> E
%token <line> IGUAL
%token <line> DIFERENTE
%token <line> MAIORIGUAL
%token <line> MENORIGUAL
%token <line> MAIS
%token <line> MENOS
%token <line> MULT
%token <line> DIV
%token <line> ATRIB
%token <line> MENOR
%token <line> MAIOR
%token <line> NAO
%token <line> ABRECHAVE
%token <line> FECHACHAVE
%token <line> ABREPARENTESE
%token <line> FECHAPARENTESE
%token <line> DOISPONTOS
%token <line> PONTOEVIRGULA
%token <line> VIRGULA

%type <node> Programa
%type <node> DeclPrograma
%type <node> Bloco
%type <node> VarSection
%type <node> ListaDeclVar
%type <node> DeclVar
%type <node> Tipo
%type <node> ListaComando
%type <node> Comando
%type <node> Expr
%type <node> OrExpr
%type <node> AndExpr
%type <node> EqExpr
%type <node> DesigExpr
%type <node> AddExpr
%type <node> MulExpr
%type <node> UnExpr
%type <node> PrimExpr

%left OU
%left E
%left IGUAL DIFERENTE
%left MENOR MAIOR MAIORIGUAL MENORIGUAL
%left MAIS MENOS
%left MULT DIV
%right NAO
%right ATRIB

%%

Programa:
    DeclPrograma
    {
        $$ = ast_create(AST_PROGRAM, $1->line, $1, NULL, NULL);
        raiz = $$;
    }
    ;

DeclPrograma: 
    PRINCIPAL Bloco
    {
        $$ = $2;
    }
    ;

Bloco:
    ABRECHAVE ListaComando FECHACHAVE
    {
        $$ = ast_create(AST_BLOCK, $1, NULL, $2, NULL);
    }
    | VarSection ABRECHAVE ListaComando FECHACHAVE
    {
        $$ = ast_create(AST_BLOCK, $2, $1, $3, NULL);
    }
    ;

VarSection:
    ABRECHAVE ListaDeclVar FECHACHAVE
    {
        $$ = $2;
    }
    ;

ListaDeclVar:
    IDENTIFICADOR DeclVar DOISPONTOS Tipo PONTOEVIRGULA ListaDeclVar
    {
        AST *nome = ast_create(
            AST_IDENTIFIER,
            $1.line,
            NULL,
            NULL,
            $1.lexeme
        );

        AST *nomes = criar_lista_nomes(nome, $2);
        AST *decls = criar_lista_decl(nomes, $4);

        if ($6 != NULL)
            $$ = ast_create(AST_LISTA_DECL, decls->line, decls, $6, NULL);
        else
            $$ = decls;

        free($1.lexeme);
    }
    | IDENTIFICADOR DeclVar DOISPONTOS Tipo PONTOEVIRGULA
    {
        AST *nome = ast_create(
            AST_IDENTIFIER,
            $1.line,
            NULL,
            NULL,
            $1.lexeme
        );

        AST *nomes = criar_lista_nomes(nome, $2);
        $$ = criar_lista_decl(nomes, $4);

        free($1.lexeme);
    }
    ;

DeclVar:
    {
        $$ = NULL;
    }
    | VIRGULA IDENTIFICADOR DeclVar
    {
        AST *nome = ast_create(
            AST_IDENTIFIER,
            $2.line,
            NULL,
            NULL,
            $2.lexeme
        );

        $$ = criar_lista_nomes(nome, $3);

        free($2.lexeme);
    }
    ;

Tipo:
    INT
    {
        $$ = ast_create(AST_TYPE_INT, $1, NULL, NULL, "int");
    }
    | CAR
    {
        $$ = ast_create(AST_TYPE_CAR, $1, NULL, NULL, "car");
    }
    ;

ListaComando:
    Comando
    {
        $$ = ast_create(AST_LISTA_COMANDOS, $1->line, $1, NULL, NULL);
    }
    | Comando ListaComando
    {
        $$ = ast_create(AST_LISTA_COMANDOS, $1->line, $1, $2, NULL);
    }
    ;

Comando:
    PONTOEVIRGULA
    {
        $$ = ast_create(AST_EMPTY, $1, NULL, NULL, NULL);
    }
    | Expr PONTOEVIRGULA
    {
        $$ = $1;
    }
    | LEIA IDENTIFICADOR PONTOEVIRGULA
    {
        AST *id = ast_create(
            AST_IDENTIFIER,
            $2.line,
            NULL,
            NULL,
            $2.lexeme
        );

        $$ = ast_create(AST_READ, $1, id, NULL, NULL);

        free($2.lexeme);
    }
    | ESCREVA Expr PONTOEVIRGULA
    {
        $$ = ast_create(AST_WRITE_EXPR, $1, $2, NULL, NULL);
    }
    | ESCREVA CADEIACARACTERES PONTOEVIRGULA
    {
        $$ = ast_create(
            AST_WRITE_STRING,
            $1,
            NULL,
            NULL,
            $2.lexeme
        );

        free($2.lexeme);
    }
    | NOVALINHA PONTOEVIRGULA
    {
        $$ = ast_create(AST_NEWLINE, $1, NULL, NULL, NULL);
    }
    | SE ABREPARENTESE Expr FECHAPARENTESE
      ENTAO Comando
      FIMSE
    {
        $$ = ast_create(AST_IF, $1, $3, $6, NULL);
    }
    | SE ABREPARENTESE Expr FECHAPARENTESE
      ENTAO Comando
      SENAO Comando
      FIMSE
    {
        AST *ramos = ast_create(
            AST_LISTA_COMANDOS,
            $6->line,
            $6,
            $8,
            NULL
        );

        $$ = ast_create(AST_IF_ELSE, $1, $3, ramos, NULL);
    }
    | ENQUANTO ABREPARENTESE Expr FECHAPARENTESE Comando
    {
        $$ = ast_create(AST_WHILE, $1, $3, $5, NULL);
    }
    | Bloco
    {
        $$ = $1;
    }
    ;

Expr:
    OrExpr
    {
        $$ = $1;
    }
    | IDENTIFICADOR ATRIB Expr
    {
        AST *id = ast_create(
            AST_IDENTIFIER,
            $1.line,
            NULL,
            NULL,
            $1.lexeme
        );

        $$ = ast_create(AST_ASSIGN, $2, id, $3, NULL);

        free($1.lexeme);
    }
    ;

OrExpr:
    OrExpr OU AndExpr
    {
        $$ = ast_create(AST_OR, $2, $1, $3, NULL);
    }
    | AndExpr
    {
        $$ = $1;
    }
    ;

AndExpr:
    AndExpr E EqExpr
    {
        $$ = ast_create(AST_AND, $2, $1, $3, NULL);
    }
    | EqExpr
    {
        $$ = $1;
    }
    ;

EqExpr:
    EqExpr IGUAL DesigExpr
    {
        $$ = ast_create(AST_EQ, $2, $1, $3, NULL);
    }
    | EqExpr DIFERENTE DesigExpr
    {
        $$ = ast_create(AST_NEQ, $2, $1, $3, NULL);
    }
    | DesigExpr
    {
        $$ = $1;
    }
    ;

DesigExpr:
    DesigExpr MENOR AddExpr
    {
        $$ = ast_create(AST_LT, $2, $1, $3, NULL);
    }
    | DesigExpr MAIOR AddExpr
    {
        $$ = ast_create(AST_GT, $2, $1, $3, NULL);
    }
    | DesigExpr MAIORIGUAL AddExpr
    {
        $$ = ast_create(AST_GE, $2, $1, $3, NULL);
    }
    | DesigExpr MENORIGUAL AddExpr
    {
        $$ = ast_create(AST_LE, $2, $1, $3, NULL);
    }
    | AddExpr
    {
        $$ = $1;
    }
    ;

AddExpr:
    AddExpr MAIS MulExpr
    {
        $$ = ast_create(AST_ADD, $2, $1, $3, NULL);
    }
    | AddExpr MENOS MulExpr
    {
        $$ = ast_create(AST_SUB, $2, $1, $3, NULL);
    }
    | MulExpr
    {
        $$ = $1;
    }
    ;

MulExpr:
    MulExpr MULT UnExpr
    {
        $$ = ast_create(AST_MUL, $2, $1, $3, NULL);
    }
    | MulExpr DIV UnExpr
    {
        $$ = ast_create(AST_DIV, $2, $1, $3, NULL);
    }
    | UnExpr
    {
        $$ = $1;
    }
    ;

UnExpr:
    MENOS PrimExpr
    {
        $$ = ast_create(AST_NEG, $1, $2, NULL, NULL);
    }
    | NAO PrimExpr
    {
        $$ = ast_create(AST_NOT, $1, $2, NULL, NULL);
    }
    | PrimExpr
    {
        $$ = $1;
    }
    ;

PrimExpr:
    IDENTIFICADOR
    {
        $$ = ast_create(
            AST_IDENTIFIER,
            $1.line,
            NULL,
            NULL,
            $1.lexeme
        );

        free($1.lexeme);
    }
    | CARCONST
    {
        $$ = ast_create(
            AST_CARCONST,
            $1.line,
            NULL,
            NULL,
            $1.lexeme
        );

        free($1.lexeme);
    }
    | INTCONST
    {
        $$ = ast_create(
            AST_INTCONST,
            $1.line,
            NULL,
            NULL,
            $1.lexeme
        );

        free($1.lexeme);
    }
    | ABREPARENTESE Expr FECHAPARENTESE
    {
        $$ = $2;
    }
    ;

%%

void yyerror(const char *s)
{
    fprintf(stderr, "ERRO: %s na linha %d\n", s, yylineno);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr,
                "Uso: %s arquivo.g\n",
                argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");

    if (yyin == NULL) {
        fprintf(stderr,
                "ERRO: nao foi possivel abrir o arquivo %s\n",
                argv[1]);
        return 1;
    }

    int resultado = yyparse();

    ast_free(raiz);
    fclose(yyin);

    return resultado;
}
