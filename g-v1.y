%{
#include <stdio.h>
#include <stdlib.h>

extern int yylex(void);
extern int yylineno;
extern char *yytext;
extern FILE *yyin;

void yyerror(const char *s);
%}

%token PRINCIPAL

%token IDENTIFICADOR
%token INT
%token CAR

%token LEIA
%token ESCREVA
%token NOVALINHA

%token SE
%token ENTAO
%token SENAO
%token FIMSE
%token ENQUANTO

%token CADEIACARACTERES
%token CARCONST
%token INTCONST

%token OU
%token E

%token IGUAL
%token DIFERENTE

%token MAIORIGUAL
%token MENORIGUAL

%token MAIS
%token MENOS
%token MULT
%token DIV

%token ATRIB

%token MENOR
%token MAIOR

%token NAO

%token ABRECHAVE
%token FECHACHAVE

%token ABREPARENTESE
%token FECHAPARENTESE

%token DOISPONTOS
%token PONTOEVIRGULA
%token VIRGULA

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
    ;

DeclPrograma:
    PRINCIPAL Bloco
    ;

Bloco:
    ABRECHAVE ListaComando FECHACHAVE
    | VarSection ABRECHAVE ListaComando FECHACHAVE
    ;

VarSection:
    ABRECHAVE ListaDeclVar FECHACHAVE
    ;

ListaDeclVar:
    IDENTIFICADOR DeclVar DOISPONTOS Tipo PONTOEVIRGULA ListaDeclVar
    | IDENTIFICADOR DeclVar DOISPONTOS Tipo PONTOEVIRGULA
    ;

DeclVar:
    /* vazio */
    | VIRGULA IDENTIFICADOR DeclVar
    ;

Tipo:
    INT
    | CAR
    ;

ListaComando:
    Comando
    | Comando ListaComando
    ;

Comando:
    PONTOEVIRGULA
    | Expr PONTOEVIRGULA
    | LEIA IDENTIFICADOR PONTOEVIRGULA
    | ESCREVA Expr PONTOEVIRGULA
    | ESCREVA CADEIACARACTERES PONTOEVIRGULA
    | NOVALINHA PONTOEVIRGULA
    | SE ABREPARENTESE Expr FECHAPARENTESE
      ENTAO Comando
      FIMSE
    | SE ABREPARENTESE Expr FECHAPARENTESE
      ENTAO Comando
      SENAO Comando
      FIMSE
    | ENQUANTO ABREPARENTESE Expr FECHAPARENTESE
      Comando
    | Bloco
    ;

Expr:
    OrExpr
    | IDENTIFICADOR ATRIB Expr
    ;

OrExpr:
    OrExpr OU AndExpr
    | AndExpr
    ;

AndExpr:
    AndExpr E EqExpr
    | EqExpr
    ;

EqExpr:
    EqExpr IGUAL DesigExpr
    | EqExpr DIFERENTE DesigExpr
    | DesigExpr
    ;

DesigExpr:
    DesigExpr MENOR AddExpr
    | DesigExpr MAIOR AddExpr
    | DesigExpr MAIORIGUAL AddExpr
    | DesigExpr MENORIGUAL AddExpr
    | AddExpr
    ;

AddExpr:
    AddExpr MAIS MulExpr
    | AddExpr MENOS MulExpr
    | MulExpr
    ;

MulExpr:
    MulExpr MULT UnExpr
    | MulExpr DIV UnExpr
    | UnExpr
    ;

UnExpr:
    MENOS PrimExpr
    | NAO PrimExpr
    | PrimExpr
    ;

PrimExpr:
    IDENTIFICADOR
    | CARCONST
    | INTCONST
    | ABREPARENTESE Expr FECHAPARENTESE
    ;

%%

void yyerror(const char *s)
{
    fprintf(stderr,
            "ERRO: %s na linha %d\n",
            s,
            yylineno);
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

    fclose(yyin);

    return resultado;
}