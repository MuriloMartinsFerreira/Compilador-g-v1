%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "g-v1.tab.h"

static char *copiar_lexema(void)
{
    char *copia;

    copia = malloc(strlen(yytext) + 1);

    if (copia == NULL) {
        fprintf(stderr, "ERRO: memoria insuficiente\n");
        exit(1);
    }

    strcpy(copia, yytext);

    return copia;
}
%}

%option yylineno
%option noyywrap

%x COMENTARIO

%%

"principal" {
    yylval.line = yylineno;
    return PRINCIPAL;
}

"int" {
    yylval.line = yylineno;
    return INT;
}

"car" {
    yylval.line = yylineno;
    return CAR;
}

"leia" {
    yylval.line = yylineno;
    return LEIA;
}

"escreva" {
    yylval.line = yylineno;
    return ESCREVA;
}

"novalinha" {
    yylval.line = yylineno;
    return NOVALINHA;
}

"se" {
    yylval.line = yylineno;
    return SE;
}

"entao" {
    yylval.line = yylineno;
    return ENTAO;
}

"senao" {
    yylval.line = yylineno;
    return SENAO;
}

"fimse" {
    yylval.line = yylineno;
    return FIMSE;
}

"enquanto" {
    yylval.line = yylineno;
    return ENQUANTO;
}

"/*" {
    BEGIN(COMENTARIO);
}

<COMENTARIO>"*/" {
    BEGIN(INITIAL);
}

<COMENTARIO>\n {
}

<COMENTARIO>. {
}

<COMENTARIO><<EOF>> {
    fprintf(stderr,
            "ERRO: COMENTARIO NAO TERMINA na linha %d\n",
            yylineno);
    return 1;
}

\"[^\n\"]*\" {
    yylval.token.lexeme = copiar_lexema();
    yylval.token.line = yylineno;
    return CADEIACARACTERES;
}

\"[^\n\"]* {
    fprintf(stderr,
            "ERRO: CADEIA DE CARACTERES OCUPA MAIS DE UMA LINHA na linha %d\n",
            yylineno);
}

\'([^\\'\n]|\\[nt])\' {
    yylval.token.lexeme = copiar_lexema();
    yylval.token.line = yylineno;
    return CARCONST;
}

[0-9]+ {
    yylval.token.lexeme = copiar_lexema();
    yylval.token.line = yylineno;
    return INTCONST;
}

[A-Za-z_][A-Za-z0-9_]* {
    yylval.token.lexeme = copiar_lexema();
    yylval.token.line = yylineno;
    return IDENTIFICADOR;
}

"||" {
    yylval.line = yylineno;
    return OU;
}

"&" {
    yylval.line = yylineno;
    return E;
}

"==" {
    yylval.line = yylineno;
    return IGUAL;
}

"!=" {
    yylval.line = yylineno;
    return DIFERENTE;
}

">=" {
    yylval.line = yylineno;
    return MAIORIGUAL;
}

"<=" {
    yylval.line = yylineno;
    return MENORIGUAL;
}

"+" {
    yylval.line = yylineno;
    return MAIS;
}

"-" {
    yylval.line = yylineno;
    return MENOS;
}

"*" {
    yylval.line = yylineno;
    return MULT;
}

"/" {
    yylval.line = yylineno;
    return DIV;
}

"=" {
    yylval.line = yylineno;
    return ATRIB;
}

"<" {
    yylval.line = yylineno;
    return MENOR;
}

">" {
    yylval.line = yylineno;
    return MAIOR;
}

"!" {
    yylval.line = yylineno;
    return NAO;
}

"{" {
    yylval.line = yylineno;
    return ABRECHAVE;
}

"}" {
    yylval.line = yylineno;
    return FECHACHAVE;
}

"(" {
    yylval.line = yylineno;
    return ABREPARENTESE;
}

")" {
    yylval.line = yylineno;
    return FECHAPARENTESE;
}

":" {
    yylval.line = yylineno;
    return DOISPONTOS;
}

";" {
    yylval.line = yylineno;
    return PONTOEVIRGULA;
}

"," {
    yylval.line = yylineno;
    return VIRGULA;
}

[ \t\r\n]+ {
}

. {
    fprintf(stderr,
            "ERRO: CARACTERE INVALIDO '%s' na linha %d\n",
            yytext,
            yylineno);
}

%%