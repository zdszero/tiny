#ifndef _UTIL_H_
#define _UTIL_H_

#include "globals.h"

extern void printToken(TokenType, char *);

extern TreeNode *newStmtNode(StmtKind);

extern TreeNode *newExpNode(ExpKind);

extern char *copyString(char *);

extern void printTree(TreeNode *);

#endif
