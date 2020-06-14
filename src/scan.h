#ifndef _SCAN_H_
#define _SCAN_H_

#include "globals.h"

#define MAXTOKENLEN 40
#define LINELEN 256
#define RESERVEDNUM 8

extern char tokenString[MAXTOKENLEN];

TokenType getToken(void);

#endif
