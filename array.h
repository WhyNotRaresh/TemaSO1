#pragma once
#include <string.h>

#define ARRAY_SIZE 100

typedef struct {
	int size;
	int last;
	char** data;
} Array;

Array allocArray(int);		// Alloc Array
void deallocArray(Array);	// Dealloc Array

void add(char*, Array*);		// Insert into array
int inArray(char*, Array*);		// Searches for data in array;