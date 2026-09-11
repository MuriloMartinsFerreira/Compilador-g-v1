%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "g-v1.tab.h"
%}

%option yylineno
%option noyywrap

%x COMENTARIO

%%

"principal"     { return PRINCIPAL; }
"int"           { return INT; }
"car"           { return CAR; }
"leia"          { return LEIA; }
"escreva"       { return ESCREVA; }
"novalinha"     { return NOVALINHA; }

"se"            { return SE; }
"entao"         { return ENTAO; }
"senao"         { return SENAO; }
"fimse"         { return FIMSE; }
"enquanto"      { return ENQUANTO; }

"/*"            { BEGIN(COMENTARIO); }

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
    exit(1);
}

\"[^\n\"]*\" {
    yylval.lexeme = copiar_lexema();
    return CADEIACARACTERES;
}

\"[^\n\"]* {
    fprintf(stderr,
            "ERRO: CADEIA DE CARACTERES OCUPA MAIS DE UMA LINHA na linha %d\n",
            yylineno);
}

\'([^\\'\n]|\\[nt])\' {
    yylval.lexeme = copiar_lexema();
    return CARCONST;
}

[0-9]+ {
    yylval.lexeme = copiar_lexema();
    return INTCONST;
}

[A-Za-z_][A-Za-z0-9_]* {
    yylval.lexeme = copiar_lexema();
    return IDENTIFICADOR;
}

"||"            { return OU; }
"&"             { return E; }

"=="            { return IGUAL; }
"!="            { return DIFERENTE; }

">="            { return MAIORIGUAL; }
"<="            { return MENORIGUAL; }

"+"             { return MAIS; }
"-"             { return MENOS; }
"*"             { return MULT; }
"/"             { return DIV; }

"="             { return ATRIB; }

"<"             { return MENOR; }
">"             { return MAIOR; }

"!"             { return NAO; }

"{"             { return ABRECHAVE; }
"}"             { return FECHACHAVE; }

"("             { return ABREPARENTESE; }
")"             { return FECHAPARENTESE; }

":"             { return DOISPONTOS; }
";"             { return PONTOEVIRGULA; }
","             { return VIRGULA; }

[ \t\r\n]+ {
}

. {
    fprintf(stderr,
            "ERRO: CARACTERE INVALIDO '%s' na linha %d\n",
            yytext,
            yylineno);
}

<<EOF>> { return -1; }

%%

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