#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <stdio.h>

/*******************************************
 *  SOME VARIABLES USED THROUGH ALL FILES  *
 *******************************************/

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

extern FILE *source;
extern FILE *listing;

extern int Error;

/* line number when listing */
extern int lineno;

/*********************************************
 *  SOME FLAGS USED FOR SELECTIVE COMPILING  *
 *********************************************/

#define NO_PARSE FALSE
#define NO_ANALYZE TRUE
#define NO_CODE TRUE

/* source code will be printed during scanning if echoSource = True */
extern int echoSource;

extern int traceScan;
extern int traceParse;
extern int traceAnalyze;
extern int traceCode;


/**********
 *  SCAN  *
 **********/

typedef enum {
    /* special tokens */
    ENDFILE, ERROR,
    /* reserved words */
    IF, THEN, ELSE, END, REPEAT, UNTIL, READ, WRITE,
    /* multicharacter tokens */
    ID, NUM,
    /* arithmetic and bool expression */
    PLUS, MINUS, TIMES, OVER, EQ, LT, LPREN, RPREN, SEMI, ASSIGN
} TokenType;


/***********
 *  PARSE  *
 ***********/

typedef enum {StmtK, ExpK} NodeKind;
typedef enum {IfK, RepeatK, AssignK, ReadK, WriteK} StmtKind;
typedef enum {OpK, ConstK, IdK} ExpKind;

/* ExpType is used for type checking */
typedef enum {Void, Integer, Boolean} ExpType;

#define MAXCHILDREN 3

typedef struct treeNode {
    struct treeNode *sibling;
    struct treeNode *child[MAXCHILDREN];
    int lineno;
    NodeKind nodeKind;
    /* node kind */
    union {
        StmtKind stmt;
        ExpKind exp;
    } kind;
    /* attributes for nonterminals */
    union {
        TokenType op;
        int val;
        char *name;
    } attr;
    ExpType type;
} TreeNode;

#endif
