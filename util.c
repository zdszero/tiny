#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "util.h"

    /* ENDFILE, ERROR, */
    /* IF, THEN, END, REPEAT, UNTIL, READ, WRITE, */
    /* ID, NUM, */
    /* PLUS, MINUS, TIMES, OVER, EQ, LT, LPREN, RPREN, SEMI, ASSIGN */


void printToken(TokenType token, char *tokenString) {
    switch (token) {
        case IF:
        case THEN:
        case END:
        case REPEAT:
        case UNTIL:
        case READ:
        case WRITE:
            fprintf(listing, "RESERVED WORD: %s\n", tokenString);
            break;
        case ID:
            fprintf(listing, "IDENTIFIER: %s\n", tokenString);
            break;
        case NUM:
            fprintf(listing, "NUMBER: %s\n", tokenString);
            break;
        case PLUS:
            fprintf(listing, "+\n");
            break;
        case MINUS:
            fprintf(listing, "-\n");
            break;
        case TIMES:
            fprintf(listing, "*\n"); 
            break;
        case OVER:
            fprintf(listing, "/\n");
            break;
        case EQ:
            fprintf(listing, "=\n");
            break;
        case LT:
            fprintf(listing, "<\n");
            break;
        case LPREN:
            fprintf(listing, "(\n");
            break;
        case RPREN:
            fprintf(listing, ")\n");
            break;
        case SEMI:
            fprintf(listing, ";\n");
            break;
        case ASSIGN:
            fprintf(listing, ":=\n");
            break;
        case ENDFILE:
            fprintf(listing, "EOF\n");
            break;
        case ERROR:
            fprintf(listing, "ERROR: %s\n", tokenString);
            break;
        default:
            fprintf(listing, "UNKNOWN TOKEN: %s\n", tokenString);
            break; 
    }
}

TreeNode *newStmtNode(StmtKind kind) {
    TreeNode *t = (TreeNode *) malloc(sizeof(TreeNode));
    if (t == NULL) {
        fprintf(stderr, "Out of memory error at line: %d\n", lineno);
    } else {
        for (unsigned i = 0; i < MAXCHILDREN; ++i) {
            t->child[i] = NULL;
        }
        t->sibling = NULL;
        t->nodeKind = StmtK;
        t->kind.stmt = kind;
        t->lineno = lineno;
    }
    return t;
}

TreeNode *newExpNode(ExpKind kind) {
    TreeNode *t = (TreeNode *) malloc(sizeof(TreeNode));
    if (t == NULL) {
        fprintf(stderr, "Out of memory error at line: %d\n", lineno);
    } else {
        for (unsigned i = 0; i < MAXCHILDREN; ++i) {
            t->child[i] = NULL;
        }
        t->sibling = NULL;
        t->nodeKind = ExpK;
        t->kind.exp = kind;
        t->lineno = lineno;
    }
    return t;
}

char *copyString(char *src) {
    if (src == NULL) {
        return NULL;
    }
    unsigned len = strlen(src) + 1;
    char *dst = (char *) malloc(len);
    strncpy(dst, src, len);
    if (dst == NULL) {
        fprintf(stderr, "Out of memory error at line: %d\n", lineno);
    }
    return dst;
}

static void printIndent(unsigned level) {
    for (unsigned i = 0; i < level; ++i) {
        printf("  ");
    }
}

static int level = -1;

void printTree(TreeNode *root) {
    level++;
    while (root != NULL) {
        printIndent(level);
        if (root->nodeKind == StmtK) {
            switch (root->kind.stmt) {
                case IfK:
                    fprintf(listing, "If\n");
                    break; 
                case RepeatK:
                    fprintf(listing, "Repeat\n");
                    break;
                case AssignK:
                    fprintf(listing, "Assign to: %s\n", root->attr.name);
                    break;
                case ReadK:
                    fprintf(listing, "Read: %s\n", root->attr.name);
                    break;
                case WriteK:
                    fprintf(listing, "Write: %s\n", root->attr.name);
                    break;
                default:
                    fprintf(listing, "Unknown kind\n");
                    break;
            }
        } else if (root->nodeKind == ExpK) {
            switch (root->kind.exp) {
                case ConstK:
                    fprintf(listing, "Const: %d\n", root->attr.val);
                    break;
                case IdK:
                    fprintf(listing, "Id: %s\n", root->attr.name);
                    break;
                case OpK:
                    fprintf(listing, "Op: ");
                    printToken(root->attr.op, "\0");
                    break;
                default:
                    fprintf(listing, "Unknown kind\n");
                    break;
            } 
        } else {
            fprintf(listing, "Unknown node kind");
        }
        for (unsigned i = 0; i < MAXCHILDREN; ++i) {
            printTree(root->child[i]);
        }
        root = root->sibling;
    }
    level--;
}
