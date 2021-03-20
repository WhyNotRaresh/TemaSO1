#pragma once
#include "hashmap.h"
#include "immutable_array.h"
#include <stdlib.h>
#include <stdio.h>

/* macro words */

#define DEF 	"#define"
#define UNDEF 	"#undef"
#define IF 		"#if"
#define IFDEF	"#ifdef"
#define IFNDEF	"#ifndef"
#define ELSE	"#else"
#define ELIF	"#elif"
#define ENDIF	"#endif"
#define INC 	"#include"

HashMap def_mappings;
Array 	dir_array;

int readLine(FILE*, int*, char**);
char* formatDirName(const char*);
char* formatDefine(const char*);
int canBeName(const char);							// checks if c can be part of variable name
int isNumber(const char*);
char* computeString(char*);							// computes new line for output
char* multiLineDefine(FILE*, char*, int);			// checks for multi line #define
FILE* searchDirArray(char*, Array);