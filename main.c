#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

FILE *listing;
FILE *source;

int echoSource = FALSE;
int traceScan = FALSE;
int traceParse = TRUE;
int traceAnalyze = FALSE;
int traceCode = FALSE;

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
    fclose(source);
    return 0;
}
