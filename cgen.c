#include <string.h>
#include "globals.h"
#include "cgen.h"
#include "code.h"
#include "symtab.h"

static int tmpOffset = 0;

static void cGen(TreeNode *t);
static void genStmt(TreeNode *t);
static void genExp(TreeNode *t);

void genCode(TreeNode *t, char *codefile);

void cGen(TreeNode *t) {
    if (t == NULL)
        return;
    switch (t->nodeKind) {
        case StmtK:
            genStmt(t);
            break;
        case ExpK:
            genExp(t);
            break;
        default:
            break;
    }
    cGen(t->sibling);
}

void genStmt(TreeNode *t) {
    TreeNode *p1, *p2, *p3;
    int loc;
    int savedLoc1, savedLoc2, currentLoc;
    switch (t->kind.stmt) {
        case IfK:
            if (traceCode)
                emitComment("-> if");
            /* if test */
            p1 = t->child[0];
            /* true case */
            p2 = t->child[1];
            /* false case */
            p3 = t->child[2];
            /* generate code for test expression */
            cGen(p1);
            savedLoc1 = emitSkip(1);
            emitComment("if: jump to else belongs here");
            /* recurse on then part */
            cGen(p2);
            savedLoc2 = emitSkip(1);
            emitComment("if: jump to end belongs here");
            currentLoc = emitSkip(0);
            emitBackUp(savedLoc1);
            emitRM_Abs("JEQ", ac, currentLoc, "if: jmp to else");
            emitRestore();
            cGen(p3);
            currentLoc = emitSkip(0);
            emitBackUp(savedLoc2);
            emitRM_Abs("LDA", pc, currentLoc, "jmp to end");
            emitRestore();
            if (traceCode)
                emitComment("<- if");
            break;
        case RepeatK:
            if (traceCode)
                emitComment("-> repeat");
            /* body */
            p1 = t->child[0];
            /* test */
            p2 = t->child[1];
            savedLoc1 = emitSkip(0);
            emitComment("repeat: jump after body comes back here");
            cGen(p1);
            cGen(p2);
            emitRM_Abs("JEQ", ac, savedLoc1, "repeat: jmp back to body");
            if (traceCode)
                emitComment("<- repeat");
            break;
        case AssignK:
            if (traceCode)
                emitComment("-> assign");
            cGen(t->child[0]);
            loc = st_lookup(t->attr.name);
            emitRM("ST", ac, loc, gp, "assign: store value");
            if (traceCode)
                emitComment("<- assign");
            break;
        case ReadK:
            if (traceCode)
                emitComment("-> read");
            emitRO("IN", ac, 0, 0, "read integer value");
            loc = st_lookup(t->attr.name);
            emitRM("ST", ac, loc, gp, "read: store value");
            if (traceCode)
                emitComment("<- read");
            break;
        case WriteK:
            if (traceCode)
                emitComment("-> write");
            cGen(t->child[0]);
            emitRO("OUT", ac, 0, 0, "write ac");
            if (traceCode)
                emitComment("<- write");
            break;
        default:
            break;
    }
}

void genExp(TreeNode *t) {
    int loc;
    TreeNode *p1, *p2;
    switch (t->kind.exp) {
        case OpK:
            if (traceCode)
                emitComment("-> Op");
            p1 = t->child[0];
            p2 = t->child[1];
            cGen(p1);
            /* push left value to temp area */
            emitRM("ST", ac, tmpOffset--, mp, "op: push left");
            /* store left value in ac1 */
            cGen(p2);
            emitRM("LD", ac1, ++tmpOffset, mp, "op: load left");
            switch (t->attr.op) {
                case PLUS:
                    emitRO("ADD", ac, ac1, ac, "op +");
                    break;
                case MINUS:
                    emitRO("SUB", ac, ac1, ac, "op -");
                    break;
                case TIMES:
                    emitRO("MUL", ac, ac1, ac, "op ×");
                    break;
                case OVER:
                    emitRO("DIV", ac, ac1, ac, "op /");
                    break;
                case LT:
                    emitRO("SUB", ac, ac1, ac, "op -");
                    emitRM("JLT", ac, 2, pc, "br if true");
                    emitRM("LDC", ac, 0, ac, "false case");
                    emitRM("LDA", pc, 1, pc, "unconditional jmp");
                    emitRM("LDC", ac, 1, ac, "true case");
                    break;
                case EQ:
                    emitRO("SUB", ac, ac1, ac, "op ==");
                    emitRO("JEQ", ac, 2, pc, "br if true");
                    emitRM("LDC", ac, 0, ac, "false case");
                    emitRM("LDA", pc, 1, pc, "unconditional jmp");
                    emitRM("LDC", ac, 1, ac, "true case");
                    break;
                default:
                    emitComment("BUG! Unknown operator!");
                    break;
            }
            if (traceCode)
                emitComment("-> Op");
            break;
        case ConstK:
            if (traceCode)
                emitComment("-> Const");
            emitRM("LDC", ac, t->attr.val, 0, "load const");
            if (traceCode)
                emitComment("<- Const");
            break;
        case IdK:
            if (traceCode)
                emitComment("-> Id");
            loc = st_lookup(t->attr.name);
            emitRM("LD", ac, loc, gp, "load id value");
            if (traceCode)
                emitComment("<- Id");
            break;
        default:
            break;
    }
}

void codeGen(TreeNode *t, char *codefile) {
    char *s = (char *) malloc(strlen(codefile) + 7);
    strcpy(s, "File: ");
    strcat(s, codefile);
    emitComment("Tiny Compilation to TM code");
    emitComment(s);
    /* generate standard prelude */
    emitComment("Standard prelude:");
    emitRM("LD", mp, 0, ac, "load maxaddress from location 0");
    emitRM("ST", ac, 0, ac, "clear location 0");
    emitComment("End of standard prelude");
    /* generate code for TINY program */
    cGen(t);
    /* finish */
    emitComment("End of execution.");
    emitRO("HALT", 0, 0, 0, "");
}
