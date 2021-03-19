#pragma once
#include <string.h>

typedef struct {
	int size;
	int last;
	char** data;
} Array;

Array allocArray(int);		// Alloc Array
void deallocArray(Array);	// Dealloc Array

void add(char*, Array*);		// Insert into array
const char* get(int, Array);	// Get element in index i;
int inArray(char*, Array);		// Searches for data in array;