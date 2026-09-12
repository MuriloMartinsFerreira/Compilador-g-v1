#include <stdio.h>
#include "symbol_table.h"

int main(void)
{
    SymbolTableStack stack;
    Symbol *symbol;

    symbol_table_init(&stack);

    symbol_table_push(&stack);

    symbol_table_insert(&stack, "x", SYMBOL_INT);
    symbol_table_insert(&stack, "y", SYMBOL_CAR);

    symbol = symbol_table_lookup(&stack, "x");

    if (symbol != NULL)
        printf("x encontrado: int\n");

    symbol_table_push(&stack);

    symbol_table_insert(&stack, "x", SYMBOL_CAR);

    symbol = symbol_table_lookup(&stack, "x");

    if (symbol != NULL)
        printf("x encontrado no escopo interno: car\n");

    symbol_table_pop(&stack);

    symbol = symbol_table_lookup(&stack, "x");

    if (symbol != NULL)
        printf("x encontrado novamente no escopo externo: int\n");

    symbol_table_pop(&stack);

    symbol_table_free(&stack);

    return 0;
}