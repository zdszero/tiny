#include <stdlib.h>
#include "globals.h"
#include "parse.h"
#include "scan.h"
#include "util.h"

int Error;
static TokenType token;

/* function prototypes for recursive calls */
static TreeNode *stmt_sequence(void);
static TreeNode *statement(void);
static TreeNode *if_stmt(void);
static TreeNode *repeat_stmt(void);
static TreeNode *assign_stmt(void);
static TreeNode *read_stmt(void);
static TreeNode *write_stmt(void);
static TreeNode *expr(void);
static TreeNode *simple_exp(void);
static TreeNode *term(void);
static TreeNode *factor(void);

static void syntaxError(char *msg) {
    fprintf(listing, "\n>>> ");
    fprintf(listing, "Syntax error in line %d: %s", lineno, msg);
    Error = TRUE;
}

static void match(TokenType expected) {
    if (token == expected) {
        token = getToken();
    } else {
        syntaxError("unexpected token → ");
        /* error */
        printToken(token, tokenString);
        fprintf(listing, "Expected Token: ");
        printToken(expected, "");
    }
}

TreeNode *parse(void) {
    TreeNode *t;
    token = getToken();
    t = stmt_sequence();
    if (token != ENDFILE) {
        syntaxError("Code end before eof\n");
    }
    return t;
}

static TreeNode *stmt_sequence(void) {
    TreeNode *t, *p, *q;
    t = statement();
    p = t;
    while (token != ENDFILE && token != ELSE && token != END && token != UNTIL) {
        match(SEMI);
        q = statement();
        if (q != NULL) {
            if (t == NULL) {
                t = q;
                p = q;
            } else {
                p->sibling = q;
                p = q;
            }
        }
    }
    return t;
}

static TreeNode *statement(void) {
    TreeNode *t = NULL;
    switch (token) {
        /* abnormal condition */
        case PLUS:
        case MINUS:
        case TIMES:
            syntaxError("operator without a previous operand\n");
            match(token);
            match(ID);
            statement();
            break;
        case EQ:
        case LT:
            syntaxError("comparison without a previous operand\n");
            match(token);
            match(ID);
            statement();
            break;
        case THEN:
            syntaxError("then without a previous if\n");
            match(THEN);
            if (t != NULL)
                t->child[1] = stmt_sequence();
            if (token == ELSE) {
                match(ELSE);
                if (t != NULL)
                    t->child[2] = stmt_sequence();
            }
            match(END);
            break;
        /* normal condition */
        case IF:
            t = if_stmt();
            break;
        case REPEAT:
            t = repeat_stmt(); 
            break;
        case ID:
            t = assign_stmt();
            break;
        case READ:
            t = read_stmt();
            break;
        case WRITE:
            t = write_stmt();
            break;
        default:
            syntaxError("unexpected token → ");
            printToken(token, tokenString);
            token = getToken();
            break;
    }
    return t;
}

static TreeNode *if_stmt(void) {
    TreeNode *t = newStmtNode(IfK);
    match(IF);
    if (t != NULL)
        t->child[0] = expr();
    match(THEN);
    if (t != NULL)
        t->child[1] = stmt_sequence();
    if (token == ELSE) {
        match(ELSE);
        if (t != NULL)
            t->child[2] = stmt_sequence();
    }
    match(END);
    return t;
}

static TreeNode *repeat_stmt(void) {
    TreeNode *t = newStmtNode(RepeatK);
    match(REPEAT);
    if (t != NULL)
        t->child[0] = stmt_sequence();
    match(UNTIL);
    if (t != NULL)
        t->child[1] = expr();
    return t;
}

static TreeNode *assign_stmt(void) {
    TreeNode *t = newStmtNode(AssignK);
    if (token == ID)
        t->attr.name = copyString(tokenString);
    match(ID);
    match(ASSIGN);
    if (t != NULL)
        t->child[0] = expr();
    return t;
}

static TreeNode *read_stmt(void) {
    TreeNode *t = newStmtNode(ReadK);
    match(READ);
    if (t != NULL && token == ID)
        t->attr.name = copyString(tokenString);
    match(ID);
    return t;
}

static TreeNode *write_stmt(void) {
    TreeNode *t = newStmtNode(WriteK);
    match(WRITE);
    if (t != NULL)
        t->child[0] = expr();
    return t;
}
static TreeNode *expr(void) {
    TreeNode *t = simple_exp();
    if (token == LT || token == EQ) {
        TreeNode *p = newExpNode(OpK);
        if (p != NULL) {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
        }
        match(token);
        if (t != NULL)
            t->child[1] = simple_exp();
    }
    return t;
}

static TreeNode *simple_exp(void) {
    TreeNode *t = term();
    while (token == PLUS || token == MINUS) {
        TreeNode *p = newExpNode(OpK);
        if (p != NULL) {
            p->child[0] = t;
            p->attr.op = token;
            match(token);
            p->child[1] = term();
            t = p;
        }
    }
    return t;
}

static TreeNode *term(void) {
    TreeNode *t = factor();
    while (token == TIMES || token == OVER) {
        TreeNode *p = newExpNode(OpK);
        if (p != NULL) {
            p->child[0] = t;
            p->attr.op = token;
            match(token);
            p->child[1] = factor();
            t = p;
        }
    }
    return t;
}

static TreeNode *factor(void) {
    TreeNode *t = NULL;
    switch (token) {
        case LPREN:
            match(LPREN);
            t = expr();
            match(RPREN);
            break; 
        case NUM:
            t = newExpNode(ConstK);
            if (t != NULL)
                t->attr.val = atoi(tokenString);
            match(NUM);
            break;
        case ID:
            t = newExpNode(IdK);
            if (t != NULL)
                t->attr.name = copyString(tokenString);
            match(ID);
            break;
        default:
            syntaxError("unexpected token → ");
            printToken(token, tokenString);
            token = getToken();
            break;
    }
    return t;
}
