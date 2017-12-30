VPATH = JASM/src/common:JASM/src/init:JASM/src/lexer:JASM/src/parser:JASM/src/lib:JASM/src/print:JASM/src/table:JASM/src
DIR = JASM/src
CFLAGS = -c -g
ASM = function.o hostAPI.o instr.o label.o symbol.o str.o\
	  run.o init.o lexer.o parser.o print.o error.o link.o\
	  chars.o file.o string.o
#ASM = init.o link.o
asm: $(ASM)
	gcc -Wall -pedantic -g $(ASM) -o assembler

link.o: link.c link.h JASM.h
	gcc $(CFLAGS) $(DIR)/common/link.c -o link.o

chars.o:chars.c unit.h JASM.h
	gcc $(CFLAGS) $(DIR)/lib/chars.c -o chars.o

error.o: error.c unit.h JASM.h
	gcc $(CFLAGS) $(DIR)/lib/error.c -o error.o

file.o: file.c unit.h JASM.h
	gcc $(CFLAGS) $(DIR)/lib/file.c -o file.o

string.o: string.c unit.h JASM.h
	gcc $(CFLAGS) $(DIR)/lib/string.c -o string.o

str.o: str.c str.h JASM.h
	gcc $(CFLAGS) $(DIR)/table/str.c -o str.o

function.o: function.c function.h JASM.h
	gcc $(CFLAGS) $(DIR)/table/function.c -o function.o

hostAPI.o: hostAPI.c hostAPI.h JASM.h
	gcc $(CFLAGS) $(DIR)/table/hostAPI.c -o hostAPI.o

instr.o: instr.c instr.h JASM.h
	gcc $(CFLAGS) $(DIR)/table/instr.c -o instr.o

label.o: label.c label.h JASM.h
	gcc $(CFLAGS) $(DIR)/table/label.c -o label.o

symbol.o: symbol.c symbol.h JASM.h
	gcc $(CFLAGS) $(DIR)/table/symbol.c -o symbol.o

lexer.o: lexer.c lexer.h JASM.h
	gcc $(CFLAGS) $(DIR)/lexer/lexer.c -o lexer.o

parser.o: parser.c parser.h JASM.h
	gcc $(CFLAGS) $(DIR)/parser/parser.c -o parser.o
print.o: print.c print.h JASM.h
	gcc $(CFLAGS) $(DIR)/print/print.c -o print.o

init.o: init.c init.h JASM.h
	gcc $(CFLAGS) $(DIR)/init/init.c -o init.o

run.o: JASM.c JASM.h
	gcc $(CFLAGS) $(DIR)/JASM.c -o run.o
clean:
	rm -rf *.o