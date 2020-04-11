CC = gcc
CFLAGS = -g -Wall
objects = main.o util.o scan.o parse.o symtab.o analyze.o

tiny: $(objects)
	gcc -o tiny $(objects) -g
main.o: main.c globals.h util.h scan.h parse.h
scan.o: scan.c globals.h util.h scan.h
parse.o: parse.c globals.h util.h scan.h parse.h
analyze.o: analyze.c globals.h analyze.h symtab.h
symtab.o: symtab.c globals.h
util.o: util.c globals.h util.h

.PHONY: clean
clean:
	@echo "start cleaning"
	rm *.o
