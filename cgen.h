#ifndef _CGEN_H_
#define _CGEN_H_

#include "globals.h"

/* traverse the syntax tree to generate code,
 * codefile is the file name of the generating code
 */
void codeGen(TreeNode *syntaxTree, char *codefile);

#endif
