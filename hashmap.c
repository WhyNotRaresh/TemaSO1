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

HashMap allocHM(int size) {
	HashMap map = (HashMap) calloc (size, sizeof(A_Item));
	return map;
}

void deallocHM(HashMap map) {
	int i = 0;
	for ( ; i < HASHMAP_SIZE; i++) {
		if (map[i] != NULL)
			deallocItem(map[i]);
	}
	free (map);
}

////////////// HASHMAP FUNCTIONS //////////////

A_Item search(const char* key, HashMap map) {
	int index = hash(key);

	while (map[index] != NULL) {
		if (strncmp(map[index]->key, key, strlen(key)) == 0 &&
			strlen(map[index]->key) == strlen(key))
			return map[index];
		index = ++index % HASHMAP_SIZE;
	}

	return NULL;
}

void insert(char* key, char* data, HashMap map) {
	A_Item item = allocItem(key, data);
	if (item != NULL) {

		int index = hash(key);

		while (map[index] != NULL) {
			index = ++index % HASHMAP_SIZE;
		}

		map[index] = item;
	}
}

void erase(char* key, HashMap map) {
	int index = hash(key);

	while (map[index] != NULL) {
		if (strncmp(map[index]->key, key, strlen(key)) == 0 &&
				strlen(map[index]->key) == strlen(key)){
			deallocItem(map[index]);
			map[index] = NULL;
			break;
		}
			
		index = ++index % HASHMAP_SIZE;
	}
}

char** getKeyList(HashMap map) {
	int i = 0, j = 0;
	char** keys = (char**) calloc (HASHMAP_SIZE + 1, sizeof(char*));

	for (; i < HASHMAP_SIZE; i++) {
		if (map[i] != NULL) {
			keys[j] = malloc(strlen(map[i]->key));
			strcpy(keys[j], map[i]->key);
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

	return hash % HASHMAP_SIZE;
}