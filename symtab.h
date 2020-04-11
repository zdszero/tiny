#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include <stdio.h>

void st_insert(char *name, int lineno, int loc);

/* return memory location if found, -1 if not found */
int st_lookup(char *name);

/* print symbol table */
void printSymTal(FILE *listing);

#endif
