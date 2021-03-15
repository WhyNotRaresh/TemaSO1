#pragma once
#include <string.h>

/* HashMap Item */
typedef struct {
	char* key;
	char* data;
} Item, *A_Item, **HashMap;

HashMap mapping;
int hashmap_size;

A_Item allocItem(char*, char*);
void deallocItem(A_Item);

HashMap allocHM(int);	// Allocate HashMap
void deallocHM();		// Deallocate HashMap

int hash(const char*);				// Hash function for strings
A_Item search(const char*);			// Search in HashMap
void insert(char*, char*);			// Insert new Item
char** getKeyList();				// Returns a list of all keys