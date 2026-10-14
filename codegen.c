#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "symbol_table.h"

static SymbolTableStack symbol_stack;
static int escopo;
static int proxima_posicao;
static int proximo_label;
static int proxima_string;

typedef struct StringLabel {
    AST *node;
    char *label;
    struct StringLabel *next;
} StringLabel;

static StringLabel *strings = NULL;

static FILE *saida;

static void erro_memoria(void)
{
    fprintf(stderr, "ERRO: memoria insuficiente\n");
    exit(1);
}

static char *copiar_string(const char *texto)
{
    char *copia;

    if (texto == NULL)
        return NULL;

    copia = malloc(strlen(texto) + 1);

    if (copia == NULL)
        erro_memoria();

    strcpy(copia, texto);

    return copia;
}

static int novo_label(void)
{
    return proximo_label++;
}

static void push_temp(void)
{
    fprintf(saida, "sw $s0, 0($sp)\n");
    fprintf(saida, "addiu $sp, $sp, -4\n");
}

static void pop_temp(void)
{
    fprintf(saida, "lw $t1, 4($sp)\n");
    fprintf(saida, "addiu $sp, $sp, 4\n");
}

static int char_value(const char *lexeme)
{
    if (lexeme == NULL)
        return 0;

    if (lexeme[0] != '\'')
        return 0;

    if (lexeme[1] != '\\')
        return (unsigned char)lexeme[1];

    switch (lexeme[2]) {
        case 'n':
            return 10;
        case 't':
            return 9;
        case 'r':
            return 13;
        case '\\':
            return 92;
        case '\'':
            return 39;
        case '"':
            return 34;
        case '0':
            return 0;
        default:
            return (unsigned char)lexeme[2];
    }
}

static void collect_strings(AST *node)
{
    StringLabel *item;

    if (node == NULL)
        return;

    if (node->type == AST_WRITE_STRING) {
        item = malloc(sizeof(StringLabel));

        if (item == NULL)
            erro_memoria();

        item->node = node;

        item->label = malloc(32);

        if (item->label == NULL)
            erro_memoria();

        sprintf(item->label, "str%d", proxima_string++);

        item->next = strings;
        strings = item;
    }

    collect_strings(node->child1);
    collect_strings(node->child2);
}

static const char *get_string_label(AST *node)
{
    StringLabel *item;

    item = strings;

    while (item != NULL) {
        if (item->node == node)
            return item->label;

        item = item->next;
    }

    return NULL;
}

static void print_strings(void)
{
    StringLabel *item;

    item = strings;

    while (item != NULL) {
        fprintf(saida, "%s:\n", item->label);
        fprintf(saida, "    .asciiz %s\n", item->node->lexeme);

        item = item->next;
    }
}

static void free_strings(void)
{
    StringLabel *item;
    StringLabel *next;

    item = strings;

    while (item != NULL) {
        next = item->next;

        free(item->label);
        free(item);

        item = next;
    }

    strings = NULL;
}

static int count_declarations(AST *node)
{
    if (node == NULL)
        return 0;

    if (node->type == AST_DECL)
        return 1;

    return count_declarations(node->child1) +
           count_declarations(node->child2);
}

static void register_declarations(AST *node)
{
    SymbolType type;
    Symbol *symbol;
    int inserted;

    if (node == NULL)
        return;

    if (node->type == AST_LISTA_DECL) {
        register_declarations(node->child1);
        register_declarations(node->child2);
        return;
    }

    if (node->type != AST_DECL)
        return;

    if (node->lexeme == NULL || node->child1 == NULL)
        return;

    if (node->child1->type == AST_TYPE_INT)
        type = SYMBOL_INT;
    else
        type = SYMBOL_CAR;

    inserted = symbol_table_insert(
        &symbol_stack,
        node->lexeme,
        type
    );

    if (!inserted) {
        fprintf(
            stderr,
            "ERRO: identificador %s ja inserido na geracao de codigo\n",
            node->lexeme
        );
        exit(1);
    }

    symbol = symbol_table_lookup(
        &symbol_stack,
        node->lexeme
    );

    if (symbol == NULL) {
        fprintf(
            stderr,
            "ERRO: identificador %s nao encontrado na geracao de codigo\n",
            node->lexeme
        );
        exit(1);
    }

    symbol->position = ++proxima_posicao;
    symbol->scope = escopo;
}

static Symbol *find_symbol(const char *name)
{
    Symbol *symbol;

    symbol = symbol_table_lookup(
        &symbol_stack,
        name
    );

    if (symbol == NULL) {
        fprintf(
            stderr,
            "ERRO: identificador %s nao encontrado na geracao de codigo\n",
            name
        );
        exit(1);
    }

    return symbol;
}

static void cgen_ex(AST *node);
static void cgen_command(AST *node);
static void cgen_command_list(AST *node);
static void cgen_block(AST *node);

static void cgen_equal(int diferente)
{
    int label_true;
    int label_end;

    label_true = novo_label();
    label_end = novo_label();

    if (diferente)
        fprintf(saida, "bne $t1, $s0, L%d\n", label_true);
    else
        fprintf(saida, "beq $t1, $s0, L%d\n", label_true);

    fprintf(saida, "li $s0, 0\n");
    fprintf(saida, "b L%d\n", label_end);

    fprintf(saida, "L%d:\n", label_true);
    fprintf(saida, "li $s0, 1\n");

    fprintf(saida, "L%d:\n", label_end);
}

static void cgen_less_equal(int maior_igual)
{
    int label_true;
    int label_end;

    label_true = novo_label();
    label_end = novo_label();

    if (maior_igual)
        fprintf(saida, "slt $s0, $s0, $t1\n");
    else
        fprintf(saida, "slt $s0, $t1, $s0\n");

    fprintf(saida, "bne $s0, $zero, L%d\n", label_true);

    fprintf(saida, "li $s0, 1\n");
    fprintf(saida, "b L%d\n", label_end);

    fprintf(saida, "L%d:\n", label_true);
    fprintf(saida, "li $s0, 0\n");

    fprintf(saida, "L%d:\n", label_end);
}

static void cgen_ex(AST *node)
{
    Symbol *symbol;
    int label_true;
    int label_end;

    if (node == NULL)
        return;

    switch (node->type) {

        case AST_IDENTIFIER:
            symbol = find_symbol(node->lexeme);

            fprintf(saida, 
                "lw $s0, %d($fp)\n",
                symbol->position * 4
            );
            break;

        case AST_INTCONST:
            fprintf(saida, 
                "li $s0, %s\n",
                node->lexeme
            );
            break;

        case AST_CARCONST:
            fprintf(saida, 
                "li $s0, %d\n",
                char_value(node->lexeme)
            );
            break;

        case AST_ASSIGN:
            cgen_ex(node->child2);

            symbol = find_symbol(
                node->child1->lexeme
            );

            fprintf(saida, 
                "sw $s0, %d($fp)\n",
                symbol->position * 4
            );
            break;

        case AST_ADD:
            cgen_ex(node->child1);
            push_temp();
            cgen_ex(node->child2);
            pop_temp();

            fprintf(saida, "add $s0, $t1, $s0\n");
            break;

        case AST_SUB:
            cgen_ex(node->child1);
            push_temp();
            cgen_ex(node->child2);
            pop_temp();

            fprintf(saida, "sub $s0, $t1, $s0\n");
            break;

        case AST_MUL:
            cgen_ex(node->child1);
            push_temp();
            cgen_ex(node->child2);
            pop_temp();

            fprintf(saida, "mult $t1, $s0\n");
            fprintf(saida, "mflo $s0\n");
            break;

        case AST_DIV:
            cgen_ex(node->child1);
            push_temp();
            cgen_ex(node->child2);
            pop_temp();

            fprintf(saida, "div $t1, $s0\n");
            fprintf(saida, "mflo $s0\n");
            break;

        case AST_LT:
            cgen_ex(node->child1);
            push_temp();
            cgen_ex(node->child2);
            pop_temp();

            fprintf(saida, "slt $s0, $t1, $s0\n");
            break;

        case AST_GT:
            cgen_ex(node->child1);
            push_temp();
            cgen_ex(node->child2);
            pop_temp();

            fprintf(saida, "slt $s0, $s0, $t1\n");
            break;

        case AST_LE:
            cgen_ex(node->child1);
            push_temp();
            cgen_ex(node->child2);
            pop_temp();

            cgen_less_equal(0);
            break;

        case AST_GE:
            cgen_ex(node->child1);
            push_temp();
            cgen_ex(node->child2);
            pop_temp();

            cgen_less_equal(1);
            break;

        case AST_EQ:
            cgen_ex(node->child1);
            push_temp();
            cgen_ex(node->child2);
            pop_temp();

            cgen_equal(0);
            break;

        case AST_NEQ:
            cgen_ex(node->child1);
            push_temp();
            cgen_ex(node->child2);
            pop_temp();

            cgen_equal(1);
            break;

        case AST_AND:
            cgen_ex(node->child1);
            push_temp();
            cgen_ex(node->child2);
            pop_temp();

            fprintf(saida, "and $s0, $t1, $s0\n");
            break;

        case AST_OR:
            cgen_ex(node->child1);
            push_temp();
            cgen_ex(node->child2);
            pop_temp();

            fprintf(saida, "or $s0, $t1, $s0\n");
            break;

        case AST_NEG:
            cgen_ex(node->child1);

            fprintf(saida, 
                "sub $s0, $zero, $s0\n"
            );
            break;

        case AST_NOT:
            cgen_ex(node->child1);

            label_true = novo_label();
            label_end = novo_label();

            fprintf(saida, 
                "beq $s0, $zero, L%d\n",
                label_true
            );

            fprintf(saida, "li $s0, 0\n");

            fprintf(saida, 
                "b L%d\n",
                label_end
            );

            fprintf(saida, 
                "L%d:\n",
                label_true
            );

            fprintf(saida, "li $s0, 1\n");

            fprintf(saida, 
                "L%d:\n",
                label_end
            );
            break;

        default:
            break;
    }
}

static void cgen_command(AST *node)
{
    int label_else;
    int label_end;
    int label_while;
    int label_end_while;
    Symbol *symbol;

    if (node == NULL)
        return;

    switch (node->type) {

        case AST_EMPTY:
            break;

        case AST_ASSIGN:
            cgen_ex(node);
            break;

        case AST_READ:
            fprintf(saida, "li $v0, 5\n");
            fprintf(saida, "syscall\n");

            symbol = find_symbol(
                node->child1->lexeme
            );

            fprintf(saida, 
                "sw $v0, %d($fp)\n",
                symbol->position * 4
            );
            break;

        case AST_WRITE_EXPR:
            cgen_ex(node->child1);

            fprintf(saida, "li $v0, 1\n");
            fprintf(saida, "move $a0, $s0\n");
            fprintf(saida, "syscall\n");
            break;

        case AST_WRITE_STRING:
            fprintf(saida, "li $v0, 4\n");
            fprintf(saida, 
                "la $a0, %s\n",
                get_string_label(node)
            );
            fprintf(saida, "syscall\n");
            break;

        case AST_NEWLINE:
            fprintf(saida, "li $v0, 11\n");
            fprintf(saida, "li $a0, 10\n");
            fprintf(saida, "syscall\n");
            break;

        case AST_IF:
            cgen_ex(node->child1);

            label_end = novo_label();

            fprintf(saida, 
                "beq $s0, $zero, L%d\n",
                label_end
            );

            cgen_command(node->child2);

            fprintf(saida, 
                "L%d:\n",
                label_end
            );
            break;

        case AST_IF_ELSE:
            cgen_ex(node->child1);

            label_else = novo_label();
            label_end = novo_label();

            fprintf(saida, 
                "beq $s0, $zero, L%d\n",
                label_else
            );

            cgen_command(node->child2->child1);

            fprintf(saida, 
                "b L%d\n",
                label_end
            );

            fprintf(saida, 
                "L%d:\n",
                label_else
            );

            cgen_command(node->child2->child2);

            fprintf(saida, 
                "L%d:\n",
                label_end
            );
            break;

        case AST_WHILE:
            label_while = novo_label();
            label_end_while = novo_label();

            fprintf(saida, 
                "L%d:\n",
                label_while
            );

            cgen_ex(node->child1);

            fprintf(saida, 
                "beq $s0, $zero, L%d\n",
                label_end_while
            );

            cgen_command(node->child2);

            fprintf(saida, 
                "b L%d\n",
                label_while
            );

            fprintf(saida, 
                "L%d:\n",
                label_end_while
            );
            break;

        case AST_BLOCK:
            cgen_block(node);
            break;

        default:
            break;
    }
}

static void cgen_command_list(AST *node)
{
    if (node == NULL)
        return;

    if (node->type == AST_LISTA_COMANDOS) {
        cgen_command(node->child1);
        cgen_command_list(node->child2);
        return;
    }

    cgen_command(node);
}

static void cgen_block(AST *node)
{
    int locals;
    int old_position;

    if (node == NULL || node->type != AST_BLOCK)
        return;

    locals = count_declarations(node->child1);
    old_position = proxima_posicao;

    escopo++;

    if (locals > 0) {
        symbol_table_push(&symbol_stack);

        register_declarations(node->child1);

        fprintf(saida, 
            "addiu $sp, $sp, -%d\n",
            locals * 4
        );
    }

    cgen_command_list(node->child2);

    if (locals > 0) {
        fprintf(saida, 
            "addiu $sp, $sp, %d\n",
            locals * 4
        );

        symbol_table_pop(&symbol_stack);
    }

    proxima_posicao = old_position;

    escopo--;
}

void codegen_generate(AST *root, const char *arquivo_entrada)
{
    char *arquivo_saida;
    size_t tamanho;
    size_t comprimento;

    if (root == NULL || arquivo_entrada == NULL)
        return;

    comprimento = strlen(arquivo_entrada);
    tamanho = comprimento + 5;

    arquivo_saida = malloc(tamanho);

    if (arquivo_saida == NULL)
        erro_memoria();

    strcpy(arquivo_saida, arquivo_entrada);

    if (comprimento >= 2 &&
        arquivo_saida[comprimento - 2] == '.' &&
        arquivo_saida[comprimento - 1] == 'g') {
        arquivo_saida[comprimento - 2] = '.';
        arquivo_saida[comprimento - 1] = 'a';
        arquivo_saida[comprimento] = 's';
        arquivo_saida[comprimento + 1] = 'm';
        arquivo_saida[comprimento + 2] = '\0';
    } else {
        strcat(arquivo_saida, ".asm");
    }

    saida = fopen(arquivo_saida, "w");

    if (saida == NULL) {
        fprintf(stderr, "ERRO: nao foi possivel criar o arquivo %s\n", arquivo_saida);
        free(arquivo_saida);
        exit(1);
    }

    symbol_table_init(&symbol_stack);

    escopo = 0;
    proxima_posicao = 0;
    proximo_label = 0;
    proxima_string = 0;
    strings = NULL;

    collect_strings(root);

    fprintf(saida, ".data\n");

    print_strings();

    fprintf(saida, ".text\n");
    fprintf(saida, ".globl main\n");
    fprintf(saida, "main:\n");
    fprintf(saida, "move $fp, $sp\n");

    cgen_block(root->child1);

    fprintf(saida, "li $v0, 10\n");
    fprintf(saida, "syscall\n");

    free_strings();

    symbol_table_free(&symbol_stack);

    fclose(saida);
    saida = NULL;

    free(arquivo_saida);
}