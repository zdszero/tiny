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

/* transform char * into loction which is int in hash table */
static int hash(char *key) {
    int temp = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        temp = ((temp << SHIFT) + key[i]) % SIZE;
    }
    return temp;
}


void st_insert(char *name, int lineno, int memloc) {
    int loc = hash(name);
    /* try to find the available location */
    while (hashTable[loc] != NULL && strcmp(hashTable[loc]->name, name) != 0) {
        loc = (loc + 1) % SIZE;
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

// return -1 if symbol cannot be found
int st_lookup(char *name) {
    int loc = hash(name);
    while (hashTable[loc] != NULL && strcmp(hashTable[loc]->name, name) != 0) {
        loc++;
    }
    if (hashTable[loc] == NULL)
        return -1;
    else
        return hashTable[loc]->memloc;
}

void printSymTal(FILE *listing) {
    fprintf(listing, "%s\n", "Variable name  Location  Line Numbers");
    fprintf(listing, "%s\n", "-------------  --------  ------------");
    int i;
    for (i = 0; i < SIZE; i++) {
        if (hashTable[i] != NULL) {
            BucketList *b = hashTable[i];
            LineList *l = b->line;
            fprintf(listing, "%-13s  ", b->name);
            fprintf(listing, "%-8d  ", b->memloc);
            while (l != NULL) {
                fprintf(listing, "%d ", l->lineno);
                l = l->next;
            }
            fprintf(listing, "\n");
        }
    }
}
