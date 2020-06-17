#include "globals.h"
#include "code.h"

/* the current emit loction */
static int emitLoc = 0;

/* highest emit loction so far,
 * used with emitSkip, emitBackup, emitStore
 */
static int highEmitLoc = 0;

void emitComment(char *c) {
    if (traceCode)
        fprintf(code, "* %s\n", c);
}

/* opcode r, s, t */
void emitRO(char *op, int r, int s, int t, char *c) {
    fprintf(code, "%3d:  %5s  %d,%d,%d", emitLoc++, op, r, s, t);
    if (traceCode)
        fprintf(code, "\t%s\n", c);
    if (highEmitLoc < emitLoc)
        highEmitLoc = emitLoc;
}

/* opcode r, d(s) */
void emitRM(char *op, int r, int d, int s, char *c) {
    fprintf(code, "%3d:  %5s  %d,%d(%d)", emitLoc++, op, r, d, s);
    if (traceCode)
        fprintf(code, "\t%s\n", c);
    if (highEmitLoc < emitLoc)
        highEmitLoc = emitLoc;
}

int emitSkip(int howMany) {
    int i = emitLoc;
    emitLoc += howMany;
    if (highEmitLoc < emitLoc)
        highEmitLoc = emitLoc;
    return i;
}

void emitBackUp(int loc) {
    if (loc > highEmitLoc)
        emitComment("Bug in emitBackUp");
    emitLoc = loc;
}

void emitRestore(void) {
    emitLoc = highEmitLoc;
}


void emitRM_Abs(char *op, int r, int a, char *c) {
    fprintf(code, "%3d:  %5s  %d,%d,%d",
            emitLoc, op, r, a-(emitLoc+1), pc);
    emitLoc++;
    if (traceCode)
        fprintf(code, "\t%s\n", c);
    if (highEmitLoc < emitLoc)
        highEmitLoc = emitLoc;
}
