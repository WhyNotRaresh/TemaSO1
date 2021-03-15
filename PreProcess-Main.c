#include "hashmap.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
	allocHM(20);

	insert("ana", "mere");
	A_Item it = search("ana");
	if (it != NULL)
		printf("%s\n", it->data);

	deallocHM();
	return 0;
}