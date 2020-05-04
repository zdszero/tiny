#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"
#include "analyze.h"
#include "cgen.h"

/*********************************************
 *  SOME FLAGS USED FOR SELECTIVE COMPILING  *
 *********************************************/

#define NO_PARSE FALSE
#define NO_ANALYZE FALSE
#define NO_CODE FALSE

FILE *listing;
FILE *source;
FILE *code;

int echoSource = FALSE;
int traceScan = FALSE;
int traceParse = TRUE;
int traceAnalyze = TRUE;
int traceCode = TRUE;

/* get filename in path and return it */
static char * getFileName(char *path);

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "%s\n", "usage: ./program sourceCode");
        return -1;
    }
    char src[20];
    strcpy(src, argv[1]);
    if (strchr(src, '.') == NULL)
        strcat(src, ".tny");
    source = fopen(src, "r");
    if (source == NULL) {
        fprintf(stderr, "file %s not found\n", src);
        return 1;
    }
    listing = stdout;
#if NO_PARSE
    while (getToken() != ENDFILE);
#else
    TreeNode *syntaxTree = parse();
    if (traceParse == TRUE) {
        fprintf(listing, "\nSyntex Tree:\n");
        printTree(syntaxTree);
    }
#endif
#if !NO_ANALYZE
    if (!Error) {
        if (traceAnalyze)
            fprintf(listing, "\nBuilding Symbol Table...\n");
        buildSymTab(syntaxTree);
        if (traceAnalyze)
            fprintf(listing, "\nChecking Types...\n");
        typeCheck(syntaxTree);
        if (traceAnalyze)
            fprintf(listing, "\nChecking Finished\n");
    }
#endif
#if !NO_CODE
    if (!Error) {
        char *codefile;
        char *basename = getFileName(src);
        int prelen = strcspn(basename, ".");
        codefile = (char *) malloc(prelen + 4);
        strncpy(codefile, basename, prelen);
        free(basename);
        strcat(codefile, ".tm");
        printf("codefile: %s\n", codefile);
        code = fopen(codefile, "w");
        if (code == NULL) {
            fprintf(stderr, "unable to open %s\n", codefile);
            exit(1);
        }
        codeGen(syntaxTree, codefile);
        fclose(code);
    }
#endif
    fclose(source);
    return 0;
}

char * getFileName(char *path) {
    if (path == NULL) {
        fprintf(stderr, "path shouldn't be empty\n");
        exit(1);
    }
    int curIdx = 0, nameIdx = 0;
    /* curIdx == strlren(path) when exiting loop */
    while (path[curIdx] != '\0') {
        if (path[curIdx] == '/') {
            nameIdx = curIdx + 1;
        }
        curIdx++;
    }
    char *filename = (char *) malloc(curIdx - nameIdx + 1);
    int i;
    for (i=0; i<curIdx-nameIdx; i++) {
        filename[i] = path[nameIdx + i];
    }
    filename[i] = '\0';
    return filename;
}
