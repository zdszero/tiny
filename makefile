CC = gcc
CFLAGS = -g -Wall
objects = main.o util.o scan.o parse.o symtab.o analyze.o cgen.o code.o

tiny: $(objects)
	gcc -o tiny $(objects)
main.o: main.c globals.h util.h scan.h parse.h cgen.h
scan.o: scan.c globals.h util.h scan.h
parse.o: parse.c globals.h util.h scan.h parse.h
analyze.o: analyze.c globals.h analyze.h symtab.h
symtab.o: symtab.c globals.h
util.o: util.c globals.h util.h
cgen.o: cgen.c globals.h cgen.h code.h symtab.h
code.o: code.c globals.h code.h

tm: tm.c
	gcc -o tm tm.c -g -Wall

.PHONY: clean
clean:
	@echo "start cleaning"
	rm *.o
