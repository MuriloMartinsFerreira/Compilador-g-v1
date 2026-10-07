#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "symbol_table.h"
#include "codegen.h"

typedef struct StringLabel {
    AST *node;
    int label;
    struct StringLabel *next;
} StringLabel;

static SymbolTableStack symbol_stack;
static int escopo;
static int proxima_posicao;
static int proximo_label;
static int proximo_string;
static StringLabel *strings;

static void erro_codegen(const char *mensagem)
{
    fprintf(stderr, "ERRO: %s\n", mensagem);
    exit(1);
}

static void push_temp(void)
{
    printf("sw $s0, 0($sp)\n");
    printf("addiu $sp, $sp, -4\n");
}

static void pop_temp(void)
{
    printf("lw $t1, 4($sp)\n");
    printf("addiu $sp, $sp, 4\n");
}

static int char_value(const char *lexeme)
{
    if (lexeme == NULL || lexeme[0] != '\'')
        return 0;

    if (lexeme[1] != '\\')
        return (unsigned char)lexeme[1];

    switch (lexeme[2]) {
        case 'n':
            return '\n';
        case 't':
            return '\t';
        case 'r':
            return '\r';
        case '\\':
            return '\\';
        case '\'':
            return '\'';
        case '"':
            return '"';
        case '0':
            return '\0';
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
            erro_codegen("memoria insuficiente");

        item->node = node;
        item->label = proximo_string++;
        item->next = strings;
        strings = item;
    }

    collect_strings(node->child1);
    collect_strings(node->child2);
}

static int string_label(AST *node)
{
    StringLabel *item = strings;

    while (item != NULL) {
        if (item->node == node)
            return item->label;

        item = item->next;
    }

    erro_codegen("string nao encontrada");
    return 0;
}

static void print_string_data(void)
{
    StringLabel *item;
    const char *texto;
    int i;

    for (item = strings; item != NULL; item = item->next) {
        printf("str%d:\n", item->label);
        printf("    .asciiz \"");

        texto = item->node->lexeme;

        if (texto != NULL && texto[0] == '"') {
            texto++;

            for (i = 0; texto[i] != '\0' && texto[i] != '"'; i++) {
                if (texto[i] == '\\' && texto[i + 1] != '\0') {
                    putchar('\\');
                    putchar(texto[i + 1]);
                    i++;
                } else {
                    putchar(texto[i]);
                }
            }
        }

        printf("\"\n");
    }
}

static int count_declarations(AST *decls)
{
    int count = 0;
    AST *atual = decls;

    while (atual != NULL) {
        if (atual->child1 != NULL &&
            atual->child1->type == AST_DECL)
            count++;

        atual = atual->child2;
    }

    return count;
}

static void enter_scope(AST *decls)
{
    AST *atual = decls;
    AST *decl;
    Symbol *symbol;

    symbol_table_push(&symbol_stack);

    while (atual != NULL) {
        decl = atual->child1;

        if (decl != NULL && decl->type == AST_DECL) {
            symbol_table_insert(
                &symbol_stack,
                decl->lexeme,
                decl->child1->type == AST_TYPE_INT
                    ? SYMBOL_INT
                    : SYMBOL_CAR
            );

            symbol = symbol_table_lookup(
                &symbol_stack,
                decl->lexeme
            );

            if (symbol == NULL)
                erro_codegen("falha ao inserir simbolo");

            symbol->position = proxima_posicao;
            symbol->scope = escopo;

            proxima_posicao++;
        }

        atual = atual->child2;
    }
}

static Symbol *find_symbol(AST *node)
{
    Symbol *symbol;

    if (node == NULL || node->lexeme == NULL)
        erro_codegen("identificador invalido");

    symbol = symbol_table_lookup(
        &symbol_stack,
        node->lexeme
    );

    if (symbol == NULL)
        erro_codegen(
            "identificador nao encontrado na geracao de codigo"
        );

    return symbol;
}

static void cgen_ex(AST *node);
static void cgen_command(AST *node);
static void cgen_command_list(AST *node);
static void cgen_block(AST *node);

static void cgen_binary(AST *node, const char *instruction)
{
    cgen_ex(node->child1);
    push_temp();

    cgen_ex(node->child2);
    pop_temp();

    printf(
        "%s $s0, $t1, $s0\n",
        instruction
    );
}

static void cgen_comparison(AST *node, const char *branch)
{
    int label_true = proximo_label++;
    int label_end = proximo_label++;

    cgen_ex(node->child1);
    push_temp();

    cgen_ex(node->child2);
    pop_temp();

    printf(
        "%s $t1, $s0, L%d\n",
        branch,
        label_true
    );

    printf("li $s0, 0\n");
    printf("b L%d\n", label_end);

    printf("L%d:\n", label_true);
    printf("li $s0, 1\n");

    printf("L%d:\n", label_end);
}

static void cgen_ex(AST *node)
{
    Symbol *symbol;
    int label_true;
    int label_end;

    if (node == NULL)
        return;

    switch (node->type) {
        case AST_INTCONST:
            printf(
                "li $s0, %s\n",
                node->lexeme
            );
            return;

        case AST_CARCONST:
            printf(
                "li $s0, %d\n",
                char_value(node->lexeme)
            );
            return;

        case AST_IDENTIFIER:
            symbol = find_symbol(node);

            printf(
                "lw $s0, %d($fp)\n",
                symbol->position * 4
            );
            return;

        case AST_ASSIGN:
            cgen_ex(node->child2);

            symbol = find_symbol(node->child1);

            printf(
                "sw $s0, %d($fp)\n",
                symbol->position * 4
            );
            return;

        case AST_ADD:
            cgen_binary(node, "add");
            return;

        case AST_SUB:
            cgen_ex(node->child1);
            push_temp();

            cgen_ex(node->child2);
            pop_temp();

            printf("sub $s0, $t1, $s0\n");
            return;

        case AST_MUL:
            cgen_ex(node->child1);
            push_temp();

            cgen_ex(node->child2);
            pop_temp();

            printf("mult $t1, $s0\n");
            printf("mflo $s0\n");
            return;

        case AST_DIV:
            cgen_ex(node->child1);
            push_temp();

            cgen_ex(node->child2);
            pop_temp();

            printf("div $t1, $s0\n");
            printf("mflo $s0\n");
            return;

        case AST_LT:
            cgen_ex(node->child1);
            push_temp();

            cgen_ex(node->child2);
            pop_temp();

            printf("slt $s0, $t1, $s0\n");
            return;

        case AST_GT:
            cgen_ex(node->child1);
            push_temp();

            cgen_ex(node->child2);
            pop_temp();

            printf("slt $s0, $s0, $t1\n");
            return;

        case AST_LE:
            cgen_ex(node->child1);
            push_temp();

            cgen_ex(node->child2);
            pop_temp();

            label_true = proximo_label++;
            label_end = proximo_label++;

            printf("slt $s0, $s0, $t1\n");
            printf(
                "beq $s0, $zero, L%d\n",
                label_true
            );

            printf("li $s0, 0\n");
            printf("b L%d\n", label_end);

            printf("L%d:\n", label_true);
            printf("li $s0, 1\n");

            printf("L%d:\n", label_end);
            return;

        case AST_GE:
            cgen_ex(node->child1);
            push_temp();

            cgen_ex(node->child2);
            pop_temp();

            label_true = proximo_label++;
            label_end = proximo_label++;

            printf("slt $s0, $t1, $s0\n");
            printf(
                "beq $s0, $zero, L%d\n",
                label_true
            );

            printf("li $s0, 0\n");
            printf("b L%d\n", label_end);

            printf("L%d:\n", label_true);
            printf("li $s0, 1\n");

            printf("L%d:\n", label_end);
            return;

        case AST_EQ:
            cgen_comparison(node, "beq");
            return;

        case AST_NEQ:
            cgen_comparison(node, "bne");
            return;

        case AST_AND:
            cgen_binary(node, "and");
            return;

        case AST_OR:
            cgen_binary(node, "or");
            return;

        case AST_NEG:
            cgen_ex(node->child1);
            printf("sub $s0, $zero, $s0\n");
            return;

        case AST_NOT:
            cgen_ex(node->child1);

            label_true = proximo_label++;
            label_end = proximo_label++;

            printf(
                "beq $s0, $zero, L%d\n",
                label_true
            );

            printf("li $s0, 0\n");
            printf("b L%d\n", label_end);

            printf("L%d:\n", label_true);
            printf("li $s0, 1\n");

            printf("L%d:\n", label_end);
            return;

        default:
            erro_codegen("expressao nao suportada");
    }
}

static void cgen_command(AST *node)
{
    Symbol *symbol;
    int label_else;
    int label_end;
    int label_while;
    int label_while_end;

    if (node == NULL)
        return;

    switch (node->type) {
        case AST_EMPTY:
            return;

        case AST_ASSIGN:
            cgen_ex(node);
            return;

        case AST_READ:
            symbol = find_symbol(node->child1);

            printf("li $v0, 5\n");
            printf("syscall\n");

            printf(
                "sw $v0, %d($fp)\n",
                symbol->position * 4
            );
            return;

        case AST_WRITE_EXPR:
            cgen_ex(node->child1);

            printf("move $a0, $s0\n");
            printf("li $v0, 1\n");
            printf("syscall\n");
            return;

        case AST_WRITE_STRING:
            printf(
                "la $a0, str%d\n",
                string_label(node)
            );

            printf("li $v0, 4\n");
            printf("syscall\n");
            return;

        case AST_NEWLINE:
            printf("li $a0, 10\n");
            printf("li $v0, 11\n");
            printf("syscall\n");
            return;

        case AST_IF:
            label_end = proximo_label++;

            cgen_ex(node->child1);

            printf(
                "beq $s0, $zero, L%d\n",
                label_end
            );

            cgen_command(node->child2);

            printf("L%d:\n", label_end);
            return;

        case AST_IF_ELSE:
            label_else = proximo_label++;
            label_end = proximo_label++;

            cgen_ex(node->child1);

            printf(
                "beq $s0, $zero, L%d\n",
                label_else
            );

            cgen_command(node->child2->child1);

            printf(
                "b L%d\n",
                label_end
            );

            printf("L%d:\n", label_else);

            cgen_command(node->child2->child2);

            printf("L%d:\n", label_end);
            return;

        case AST_WHILE:
            label_while = proximo_label++;
            label_while_end = proximo_label++;

            printf("L%d:\n", label_while);

            cgen_ex(node->child1);

            printf(
                "beq $s0, $zero, L%d\n",
                label_while_end
            );

            cgen_command(node->child2);

            printf(
                "b L%d\n",
                label_while
            );

            printf("L%d:\n", label_while_end);
            return;

        case AST_BLOCK:
            cgen_block(node);
            return;

        default:
            erro_codegen("comando nao suportado");
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
    int declarations;
    int locals;

    if (node == NULL || node->type != AST_BLOCK)
        return;

    escopo++;

    declarations = node->child1 != NULL;

    if (declarations) {
        locals = count_declarations(node->child1);

        enter_scope(node->child1);

        printf(
            "addiu $sp, $sp, -%d\n",
            locals * 4
        );
    }

    cgen_command_list(node->child2);

    if (declarations) {
        locals = count_declarations(node->child1);

        printf(
            "addiu $sp, $sp, %d\n",
            locals * 4
        );

        symbol_table_pop(&symbol_stack);

        proxima_posicao -= locals;
    }

    escopo--;
}

void codegen_generate(AST *root)
{
    StringLabel *item;
    StringLabel *next;

    if (root == NULL || root->type != AST_PROGRAM)
        erro_codegen("AST invalida");

    symbol_table_init(&symbol_stack);

    escopo = 0;
    proxima_posicao = 1;
    proximo_label = 0;
    proximo_string = 0;
    strings = NULL;

    collect_strings(root);

    if (strings != NULL) {
        printf(".data\n");
        print_string_data();
    }

    printf(".text\n");
    printf(".globl main\n");
    printf("main:\n");
    printf("move $fp, $sp\n");

    cgen_block(root->child1);

    printf("li $v0, 10\n");
    printf("syscall\n");

    item = strings;

    while (item != NULL) {
        next = item->next;
        free(item);
        item = next;
    }

    strings = NULL;

    symbol_table_free(&symbol_stack);
}