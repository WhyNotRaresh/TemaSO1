#pragma once
#include <string.h>

#define HASHMAP_SIZE 10

/* HashMap Item */
typedef struct {
	char* key;
	char* data;
} Item, *A_Item, **HashMap;

A_Item allocItem(char*, char*);
void deallocItem(A_Item);

HashMap allocHM(int);			// Allocate HashMap
void deallocHM(HashMap);		// Deallocate HashMap

int hash(const char*);						// Hash function for strings
A_Item search(const char*, HashMap);		// Search in HashMap
void insert(char*, char*, HashMap);			// Insert new item
void erase(char*, HashMap);					// Delete item
char** getKeyList(HashMap);					// Returns a list of all keys