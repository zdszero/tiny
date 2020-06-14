#ifndef _CODE_H_
#define _CODE_H_

/* reg 2, 3, 4 not used */

/*
 *  mp -> t1  temp value
 *        t2
 *        ...
 *        y
 *  gp -> x   local variable
 *
 */

/* program counter */
#define pc 7

/* memory pointer */
#define mp 6

/* global pointer */
#define gp 5

/* accumulator */
#define ac 0

/* 2nd accumulator */
#define ac1 1

/* code emitting functions */

void emitComment(char *c);

/* opcode r, s, t
 * r: target register
 * s: 1st source register
 * t: 2nd source register
 * c: comment to be echoed if traceCode == TRUE
 */
void emitRO(char *op, int r, int s, int t, char *c);

/* opcode r, d(s)
 * r: target register
 * d: offset
 * s: base register
 * c: comment
 */
void emitRM(char *op, int r, int d, int s, char *c);

/* skips 'howMany' code locations for later bacpatch
 * return the current code location
 */
int emitSkip(int howMany);

/* back up to a previous skipped location */
void emitBackUp(int loc);

/* restore the current code location to the
 * highest previously unemitted postion
 */
void emitRestore(void);

/* connverts an absolute reference to
 * a pc-relative reference when emitting
 * a register-to-memory TM instruction
 * r: target register
 * a: absolute location in memory
 */
void emitRM_Abs(char *op, int r, int a, char *c);

#endif
