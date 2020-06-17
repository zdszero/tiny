#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <libgen.h>
#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"
#include "analyze.h"
#include "cgen.h"

/*********************************************
 *  SOME FLAGS USED FOR SELECTIVE COMPILING  *
 *********************************************/

int no_parse   = TRUE;;
int no_analyze = TRUE;
int no_code    = TRUE;

FILE *listing;
FILE *source;
FILE *code;

int echoSource   = FALSE;
int traceScan    = FALSE;
int traceParse   = FALSE;
int traceAnalyze = FALSE;
int traceCode    = FALSE;

int main(int argc, char *argv[])
{
    char src[20];
    static struct option opts[] = {
        {"help", no_argument, 0, 'h'},
        {"scan", no_argument, 0, 's'},
        {"parse", no_argument, 0, 'p'},
        {"analyze", no_argument, 0, 'a'},
        {"code", no_argument, 0, 'c'},
        {"file", required_argument, 0, 'f'}
    };
    static int filename_set = FALSE;
    static int optcount = 0;
    while (TRUE) {
        int option_index;
        int opt;
        opt = getopt_long(argc, argv, "hspacf:",
                opts, &option_index);
        /* no further options */
        if (opt == -1)
            break;
        switch (opt) {
            case 'h':
                printf("compiling options:\n\
-s --scan             scan only\n\
-p --parse            scan and parse\n\
-a --analyze          scan, parse and analyze\n\
-c --code             scan, parse, analyze and generate intermediate code\n\
-f --file <filename>  input tiny file\n\
usage:\n\
./tiny source_code -[option]\n");
                exit(0);
            case 's':
                optcount++;
                echoSource    = TRUE;
                traceScan     = TRUE;
                break;
            case 'p':
                optcount++;
                no_parse      = FALSE;
                traceParse    = TRUE;
                break;
            case 'a':
                optcount++;
                no_parse      = FALSE;
                no_analyze    = FALSE;
                no_code       = TRUE;
                traceAnalyze  = TRUE;
                break;
            case 'c':
                optcount++;
                no_parse      = FALSE;
                no_analyze    = FALSE;
                no_code       = FALSE;
                traceCode     = TRUE;
                break;
            case 'f':
                strcpy(src, optarg);
                filename_set = TRUE;
                break;
            case '?':
                printf("missing argument\n");
                break;
            default:
                fprintf(stderr, "should never come here\n");
                exit(1);
                break;
        }
    }
    if (!filename_set) {
        fprintf(stderr, "%s\n", "filename not set");
        exit(2);
    }
    if (optcount != 1) {
        fprintf(stderr, "%s\n", "more than one compiling options");
        exit(3);
    }

    if (strchr(src, '.') == NULL)
        strcat(src, ".tny");
    source = fopen(src, "r");
    if (source == NULL) {
        fprintf(stderr, "file %s not found\n", src);
        return 1;
    }
    listing = stdout;
    if (no_parse) {
        while (getToken() != ENDFILE);
    } else {
        TreeNode *syntaxTree = parse();
        if (!Error && traceParse == TRUE) {
            fprintf(listing, "\nSyntex Tree:\n");
            printTree(syntaxTree);
        }
        if (!no_analyze) {
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
        }
        if (!no_code) {
            if (!Error) {
                char *codefile;
                char *base = basename(src);
                /* the length of tiny file without .tny */
                int prelen = strcspn(base, ".");
                codefile = (char *) malloc(prelen + 4);
                strncpy(codefile, base, prelen);
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
        }
    }
    fclose(source);
    return 0;
}
