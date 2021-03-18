#pragma once
#include "hashmap.h"
#include <stdlib.h>
#include <stdio.h>

/* macro words */

#define DEF 	"#define"
#define UNDEF 	"#undef"
#define IF 		"#if"
#define ELSE	"#else"
#define ELIF	"#elif"
#define ENDIF	"#endif"

HashMap def_mappings;

int canBeName(const char);							// checks if c can be part of variable name
int isNumber(const char*);
char* computeString(char*);							// computes new line for output
char* multiLineDefine(FILE*, char*, int);			// checks for multi line #define