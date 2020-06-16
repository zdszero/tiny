#include "analyze.h"
#include "globals.h"
#include "symtab.h"

/* counter for variable memory location */
static int location = 0;

/* used for both preOrder and postOrder traverse
 *  use preOrder to build the symbol table
 *  use postOrder to to do the type checking
 * since node.type is a synthesized attribute,
 * which is the only attribute we should consider in tiny
 */
static void traverse(TreeNode *t,
                void (* preProc) (TreeNode *),
                void (* postProc) (TreeNode *)) {
    if (t == NULL)
        return;
    preProc(t);
    for (int i = 0; i < MAXCHILDREN; i++) {
        traverse(t->child[i], preProc, postProc);
    }
    postProc(t);
    traverse(t->sibling, preProc, postProc);
}

static void nullProc(TreeNode *t) {
    return;
}

static void insertNode(TreeNode *t) {
    switch (t->nodeKind) {
        case StmtK:
            switch (t->kind.stmt) {
                case AssignK:
                case ReadK:
                    printf("insert %s at line %d\n", t->attr.name, t->lineno);
                    if (st_lookup(t->attr.name) == -1) {
                        st_insert(t->attr.name, t->lineno, ++location);
                    } else {
                        st_insert(t->attr.name, t->lineno, location);
                    }
                    break;
                default:
                    break;
            }
        case ExpK:
            switch (t->kind.exp) {
                case IdK:
                    printf("insert %s at line %d\n", t->attr.name, t->lineno);
                    if (st_lookup(t->attr.name) == -1) {
                        st_insert(t->attr.name, t->lineno, ++location);
                    } else {
                        st_insert(t->attr.name, t->lineno, location);
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

static void typeError(TreeNode *t, char *msg) {
    fprintf(listing, "Type Error at line %d: %s\n", t->lineno, msg);
    Error = TRUE;
}

static void checkNode(TreeNode *t) {
    switch (t->nodeKind) {
        case StmtK:
            switch (t->kind.stmt) {
                case IfK:
                    ASSERT(t->child[0] != NULL, "if child should not be null")
                    if (t->child[0]->type != Boolean)
                        typeError(t, "if test is not boolean");
                    break;
                case RepeatK:
                    ASSERT(t->child[1] != NULL, "repeat child should not be null")
                    if (t->child[1]->type != Boolean)
                        typeError(t, "repeat test is not boolean");
                    break;
                case AssignK:
                    ASSERT(t->child[0] != NULL, "assign child should not be null")
                    if (t->child[0]->type != Integer)
                        typeError(t, "assign test is not integer");
                    break;
                case WriteK:
                    ASSERT(t->child[0] != NULL, "write child should not be null")
                    if (t->child[0]->type != Integer)
                        typeError(t, "write test is not integer");
                    break;
                /* ReadK node has name attribute */
                default:
                    break;
            }
            break;
        case ExpK:
            switch (t->kind.exp) {
                case OpK:
                    ASSERT(t->child[0] != NULL && t->child[1] != NULL, "op child should not be null")
                    if (t->child[0]->type != Integer ||
                        t->child[1]->type != Integer)
                        typeError(t, "op test not both integer");
                    if (t->attr.op == EQ || t->attr.op == LT)
                        t->type = Boolean;
                    else
                        t->type = Integer;
                    break;
                case ConstK:
                case IdK:
                    t->type = Integer;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}


void buildSymTab(TreeNode *syntaxTree) {
    traverse(syntaxTree, insertNode, nullProc);
    if (traceAnalyze) {
        fprintf(listing, "\nSymbol Table:\n\n");
        printSymTal(listing);
    }
}

void typeCheck(TreeNode *syntaxTree) {
    traverse(syntaxTree, nullProc, checkNode);
}
