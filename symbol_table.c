#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"

#define TABLE_SIZE 101

static unsigned int hash(const char *name)
{
    unsigned int value = 0;

    while (*name) {
        value = value * 31 + (unsigned char)*name;
        name++;
    }

    return value % TABLE_SIZE;
}

static SymbolTable *create_table(void)
{
    SymbolTable *table;

    table = malloc(sizeof(SymbolTable));

    if (table == NULL) {
        fprintf(stderr, "ERRO: memoria insuficiente\n");
        exit(1);
    }

    table->size = TABLE_SIZE;
    table->buckets = calloc(TABLE_SIZE, sizeof(Symbol *));

    if (table->buckets == NULL) {
        fprintf(stderr, "ERRO: memoria insuficiente\n");
        free(table);
        exit(1);
    }

    table->next = NULL;

    return table;
}

static void free_symbol(Symbol *symbol)
{
    Symbol *next;

    while (symbol != NULL) {
        next = symbol->next;
        free(symbol->name);
        free(symbol);
        symbol = next;
    }
}

static void free_table(SymbolTable *table)
{
    int i;

    if (table == NULL)
        return;

    for (i = 0; i < table->size; i++)
        free_symbol(table->buckets[i]);

    free(table->buckets);
    free(table);
}

void symbol_table_init(SymbolTableStack *stack)
{
    if (stack == NULL)
        return;

    stack->top = NULL;
}

void symbol_table_push(SymbolTableStack *stack)
{
    SymbolTable *table;

    if (stack == NULL)
        return;

    table = create_table();
    table->next = stack->top;
    stack->top = table;
}

void symbol_table_pop(SymbolTableStack *stack)
{
    SymbolTable *table;

    if (stack == NULL || stack->top == NULL)
        return;

    table = stack->top;
    stack->top = table->next;

    free_table(table);
}

Symbol *symbol_table_lookup(SymbolTableStack *stack, const char *name)
{
    SymbolTable *table;
    Symbol *symbol;
    unsigned int index;

    if (stack == NULL || name == NULL)
        return NULL;

    table = stack->top;

    while (table != NULL) {
        index = hash(name);
        symbol = table->buckets[index];

        while (symbol != NULL) {
            if (strcmp(symbol->name, name) == 0)
                return symbol;

            symbol = symbol->next;
        }

        table = table->next;
    }

    return NULL;
}

int symbol_table_insert(SymbolTableStack *stack,
                        const char *name,
                        SymbolType type)
{
    SymbolTable *table;
    Symbol *symbol;
    unsigned int index;
    char *name_copy;

    if (stack == NULL || stack->top == NULL || name == NULL)
        return 0;

    table = stack->top;
    index = hash(name);
    symbol = table->buckets[index];

    while (symbol != NULL) {
        if (strcmp(symbol->name, name) == 0)
            return 0;

        symbol = symbol->next;
    }

    symbol = malloc(sizeof(Symbol));

    if (symbol == NULL) {
        fprintf(stderr, "ERRO: memoria insuficiente\n");
        exit(1);
    }

    name_copy = malloc(strlen(name) + 1);

    if (name_copy == NULL) {
        fprintf(stderr, "ERRO: memoria insuficiente\n");
        free(symbol);
        exit(1);
    }

    strcpy(name_copy, name);

    symbol->name = name_copy;
    symbol->type = type;
    symbol->next = table->buckets[index];
    table->buckets[index] = symbol;

    return 1;
}

void symbol_table_free(SymbolTableStack *stack)
{
    if (stack == NULL)
        return;

    while (stack->top != NULL)
        symbol_table_pop(stack);
}