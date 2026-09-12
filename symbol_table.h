#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

typedef enum {
    SYMBOL_INT,
    SYMBOL_CAR
} SymbolType;

typedef struct Symbol {
    char *name;
    SymbolType type;
    struct Symbol *next;
} Symbol;

typedef struct SymbolTable {
    Symbol **buckets;
    int size;
    struct SymbolTable *next;
} SymbolTable;

typedef struct {
    SymbolTable *top;
} SymbolTableStack;

void symbol_table_init(SymbolTableStack *stack);

void symbol_table_push(SymbolTableStack *stack);

void symbol_table_pop(SymbolTableStack *stack);

Symbol *symbol_table_lookup(SymbolTableStack *stack, const char *name);

int symbol_table_insert(SymbolTableStack *stack,
                        const char *name,
                        SymbolType type);

void symbol_table_free(SymbolTableStack *stack);

#endif