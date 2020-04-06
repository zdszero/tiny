#include <string.h>
#include <ctype.h>
#include "globals.h"
#include "scan.h"
#include "util.h"

typedef enum {
    START, INCOMMENT, INNUM, INID, INASSIGN, DONE
} StateType;

/* maintain current reading string */

char tokenString[MAXTOKENLEN];

/* reading source code */
char linebuf[LINELEN];
int linepos = 0;
int bufsize = 0;
int lineno = 0;
int EOF_flag = FALSE;

int getNextChar(void) {
    if (linepos < bufsize) {
        return linebuf[linepos++]; 
    } else {
        if (fgets(linebuf, LINELEN-1, source) != NULL) {
            lineno++;
            if (echoSource == TRUE) {
                fprintf(listing, "%4d: %s", lineno, linebuf);
            }
            bufsize = strlen(linebuf);
            linepos = 0;
            return linebuf[linepos++]; 
        } else {
            EOF_flag = TRUE;
            /* EOF = -1 */
            return EOF;
        }
    }
}

void ungetNextChar(void) {
    if (EOF_flag == FALSE)
        linepos--;
}

/* check reserved word */
struct {
    char *str;
    TokenType tok;
} reservedWords[RESERVEDNUM] = {
    {"if", IF}, {"then", THEN}, {"else", ELSE}, {"end", END},
    {"repeat", REPEAT}, {"until", UNTIL},
    {"read", READ}, {"write", WRITE}
};

TokenType reservedLookup(char *s) {
    for (int i=0; i<RESERVEDNUM; i++) {
        if (strcmp(s, reservedWords[i].str) == 0)
            return reservedWords[i].tok;
    }
    return ID;
}

TokenType getToken(void) {
    StateType state = START;
    TokenType curToken = ERROR;
    int tokenStringIdx = 0;
    int curChar;
    int save;
    while (state != DONE) {
        curChar = getNextChar();
        save = TRUE;
        switch (state) {
            case START:
                if (curChar == ' ' || curChar == '\t' || curChar == '\n') {
                    save = FALSE;
                } else if (curChar == '{') {
                    state = INCOMMENT;
                    save = FALSE;
                } else if (isdigit(curChar)) {
                    state = INNUM;
                } else if (isalpha(curChar)) {
                    state = INID;
                } else if (curChar == ':') {
                    state = INASSIGN;
                } else {
                    state = DONE;
                    switch (curChar) {
                        case EOF:
                            save = FALSE;
                            curToken = ENDFILE;
                            break;
                        case '+':
                            curToken = PLUS;
                            break;
                        case '-':
                            curToken = MINUS;
                            break;
                        case '*':
                            curToken = TIMES;
                            break;
                        case '/':
                            curToken = OVER;
                            break;
                        case '(':
                            curToken = LPREN;
                            break;
                        case ')':
                            curToken = RPREN;
                            break;
                        case '=':
                            curToken = EQ;
                            break;
                        case '<':
                            curToken = LT;
                            break;
                        case ';':
                            curToken = SEMI;
                            break;
                        default:
                            curToken = ERROR;
                            break;
                    }
                }
                break;
            case INCOMMENT:
                save = FALSE;
                if (curChar == EOF) {
                    state = DONE;
                    curToken = ENDFILE;
                } else if (curChar == '}') {
                    state = START;
                }
                break;
            case INNUM:
                /* if digit save == true */
                if (!isdigit(curChar)) {
                    ungetNextChar();
                    save = FALSE;
                    state = DONE;
                    curToken = NUM;
                }
                break;
            case INID:
                if (!isalpha(curChar)) {
                    ungetNextChar();
                    save = FALSE;
                    state = DONE;
                    curToken = ID;
                }
                break;
            case INASSIGN:
                state = DONE;
                if (curChar == '=') {
                    curToken = ASSIGN;
                } else {
                    ungetNextChar();
                    save = FALSE;
                    curToken = ERROR;
                }
                break;
            case DONE:
            default:
                fprintf(stderr, "%s\n", "should never get here");
                state = DONE;
                curToken = ERROR;
                break;
        }
        if (save == TRUE && tokenStringIdx < MAXTOKENLEN) {
            tokenString[tokenStringIdx++] = (char) curChar;
        }
        if (state == DONE) {
            tokenString[tokenStringIdx] = '\0';
            if (curToken == ID) {
                curToken = reservedLookup(tokenString);
            }
        }
    }
    if (traceScan == TRUE) {
        fprintf(listing, "\t%d: ", lineno);
        printToken(curToken, tokenString);
    }
    return curToken;
}
