#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* size of hash table */
#define SIZE 128

/* used for construct hash function */
#define SHIFT 4

typedef struct lineList {
    int lineno;
    struct lineList *next;
} LineList;

typedef struct bucketList {
    char *name;
    int memloc;
    LineList *line;
} BucketList;

static BucketList* hashTable[SIZE] = {NULL};

/* transform char * into loction in hash table */
static int hash(char *key) {
    int temp = 0;
    int i = 0;
    while (key[i] != '\0') {
        temp = ((temp << SHIFT) + key[i]) % SIZE;
        i++;
    }
    return temp;
}


void st_insert(char *name, int lineno, int memloc) {
    int loc = hash(name);
    while (hashTable[loc] != NULL && strcmp(hashTable[loc]->name, name) != NULL) {
        loc++;
    }
    /* variable not found */
    if (hashTable[loc] == NULL) {
        BucketList *bucket = (BucketList *) malloc(sizeof(BucketList));
        hashTable[loc] = bucket;
        bucket->name = name;
        bucket->memloc = memloc;
        bucket->line = (LineList *) malloc(sizeof(LineList));
        bucket->line->lineno = lineno;
        bucket->line->next = NULL;
    /* variable found */
    } else {
        LineList *l = hashTable[loc]->line;
        while (l->next != NULL) {
            l = l->next;
        }
        l->next = (LineList *) malloc(sizeof(LineList));
        l->next->lineno = lineno;
        l->next->next = NULL;
    }
}

int st_lookup(char *name) {
    int loc = hash(name);
    while (hashTable[loc] != NULL && strcmp(hashTable[loc]->name, name) != NULL) {
        loc++;
    }
    if (hashTable[loc] == NULL)
        return 0;
    else
        return loc;
}

void printSymTal(FILE *listing) {
    fprintf(listing, "%s\n", "Variable name  Location  Line Numbers");
    fprintf(listing, "%s\n", "-------------  --------  ------------");
    int i;
    for (i=0; i<SIZE; i++) {
        if (hashTable[i] != NULL) {
            BucketList *b = hashTable[i];
            LineList *l = b->line;
            fprintf(listing, "%-13s  ", b->name);
            fprintf(listing, "%-8d  ", b->memloc);
            while (l->next != NULL) {
                fprintf(listing, "%d ", l->lineno);
                l = l->next;
            }
            fprintf(listing, "\n");
        }
    }
}
