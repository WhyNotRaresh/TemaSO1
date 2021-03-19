#include "immutable_array.h"
#include <stdlib.h>

////////////// ARRAY ALLOCATION //////////////

Array allocArray(int size) {
	Array array;
	array.size = size;
	array.data = (char**) calloc(size, sizeof(char*));
	array.last = 0;

	return array;
}

void deallocArray(Array array) {
	for (; array.last >= 0; array.last--) {
		free(array.data[array.last]);
	}
	free(array.data);
}

////////////// ARRAY FUNCTIONS //////////////

void add(char* str, Array *arr) {
	char* new_str = calloc (strlen(str) + 1, 1);
	strncpy(new_str, str, strlen(str));

	if (arr->last == arr->size) return;

	arr->data[(arr->last)++] = new_str;
}

const char* get(int index, Array arr) {
	if (index > arr.last) return NULL;
	return arr.data[index];
}

int inArray(char* str, Array arr) {
	int i = 0;
	for (; i < arr.last; i++) {
		if (strncmp(arr.data[i], str, strlen(str)) == 0 &&
				strlen(arr.data[i]) == strlen(str))
			return i;
	}

	return -1;
}