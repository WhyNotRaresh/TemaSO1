#include "hashmap.h"
#include <stddef.h>
#include <stdlib.h>

////////////// ITEM ALLOCATION //////////////

A_Item allocItem(char* key, char* data) {
	A_Item item = (A_Item) malloc (sizeof(Item));
	item->key = (char*) calloc(strlen(key) + 1, 1);
	item->data = (char*) calloc(strlen(data) + 1, 1);

	strncpy(item->key, key, strlen(key));
	strncpy(item->data, data, strlen(data));
	return item;
}

void deallocItem(A_Item item) {
	free(item->key);
	free(item->data);
	free(item);
}

////////////// HASHMAP ALLOCATION //////////////

HashMap allocHM(int maxSize) {
	hashmap_size = maxSize;
	mapping = (HashMap) calloc (maxSize, sizeof(A_Item));
	return mapping;
}

void deallocHM() {
	int i = 0;
	for ( ; i < hashmap_size; i++) {
		if (mapping[i] != NULL)
			deallocItem(mapping[i]);
	}
	free (mapping);
}

////////////// HASHMAP FUNCTIONS //////////////

A_Item search(const char* key) {
	int index = hash(key);

	while (mapping[index] != NULL) {
		if (strncmp(mapping[index]->key, key, strlen(key)) == 0 &&
			strlen(mapping[index]->key) == strlen(key))
			return mapping[index];
		index = ++index % hashmap_size;
	}

	return NULL;
}

void insert(char* key, char* data) {
	A_Item item = allocItem(key, data);
	if (item == NULL) return;

	int index = hash(key);

	while (mapping[index] != NULL) {
		index = ++index % hashmap_size;
	}

	mapping[index] = item;
}

char** getKeyList() {
	int i = 0, j = 0;
	char** keys = (char**) calloc (hashmap_size + 1, sizeof(char*));

	for (; i < hashmap_size; i++) {
		if (mapping[i] != NULL) {
			keys[j] = malloc(strlen(mapping[i]->key));
			strcpy(keys[j], mapping[i]->key);
			j++;
		}
	}

	return keys;
}

int hash(const char* str) {
	unsigned long hash = 5381;
	int c;

	while (c = *str++) {
		hash = ((hash << 5) + hash) + c; /* hash * 33  + c */
	}

	return hash % hashmap_size;
}