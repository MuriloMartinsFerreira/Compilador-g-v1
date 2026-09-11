/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_G_V1_TAB_H_INCLUDED
# define YY_YY_G_V1_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    PRINCIPAL = 258,               /* PRINCIPAL  */
    IDENTIFICADOR = 259,           /* IDENTIFICADOR  */
    INT = 260,                     /* INT  */
    CAR = 261,                     /* CAR  */
    LEIA = 262,                    /* LEIA  */
    ESCREVA = 263,                 /* ESCREVA  */
    NOVALINHA = 264,               /* NOVALINHA  */
    SE = 265,                      /* SE  */
    ENTAO = 266,                   /* ENTAO  */
    SENAO = 267,                   /* SENAO  */
    FIMSE = 268,                   /* FIMSE  */
    ENQUANTO = 269,                /* ENQUANTO  */
    CADEIACARACTERES = 270,        /* CADEIACARACTERES  */
    CARCONST = 271,                /* CARCONST  */
    INTCONST = 272,                /* INTCONST  */
    OU = 273,                      /* OU  */
    E = 274,                       /* E  */
    IGUAL = 275,                   /* IGUAL  */
    DIFERENTE = 276,               /* DIFERENTE  */
    MAIORIGUAL = 277,              /* MAIORIGUAL  */
    MENORIGUAL = 278,              /* MENORIGUAL  */
    MAIS = 279,                    /* MAIS  */
    MENOS = 280,                   /* MENOS  */
    MULT = 281,                    /* MULT  */
    DIV = 282,                     /* DIV  */
    ATRIB = 283,                   /* ATRIB  */
    MENOR = 284,                   /* MENOR  */
    MAIOR = 285,                   /* MAIOR  */
    NAO = 286,                     /* NAO  */
    ABRECHAVE = 287,               /* ABRECHAVE  */
    FECHACHAVE = 288,              /* FECHACHAVE  */
    ABREPARENTESE = 289,           /* ABREPARENTESE  */
    FECHAPARENTESE = 290,          /* FECHAPARENTESE  */
    DOISPONTOS = 291,              /* DOISPONTOS  */
    PONTOEVIRGULA = 292,           /* PONTOEVIRGULA  */
    VIRGULA = 293                  /* VIRGULA  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_G_V1_TAB_H_INCLUDED  */
