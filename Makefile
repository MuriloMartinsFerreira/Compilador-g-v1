CC      = gcc
CFLAGS  = -Wall -Wno-unused-function -g
TARGET  = g-v1
OBJS    = g-v1.tab.o lex.yy.o ast.o symbol_table.o semantic.o codegen.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Analisador sintatico (Bison): gera g-v1.tab.c e g-v1.tab.h
g-v1.tab.c g-v1.tab.h: g-v1.y ast.h
	bison -d g-v1.y

# Analisador lexico (Flex): gera lex.yy.c
lex.yy.c: g-v1.l g-v1.tab.h
	flex -o lex.yy.c g-v1.l

g-v1.tab.o:   g-v1.tab.c ast.h semantic.h codegen.h
lex.yy.o:     lex.yy.c g-v1.tab.h ast.h
ast.o:        ast.c ast.h
symbol_table.o: symbol_table.c symbol_table.h
semantic.o:   semantic.c semantic.h ast.h symbol_table.h
codegen.o:    codegen.c codegen.h ast.h symbol_table.h

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS) g-v1.tab.c g-v1.tab.h lex.yy.c

.PHONY: all clean
