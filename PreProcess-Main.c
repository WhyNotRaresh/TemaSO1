#include "hashmap.h"
#include "array.h"
#include "text_processing.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int mapAllArgs(FILE**, FILE**, int, char**);		// map all arguments
void process(FILE*, FILE*);							// process input file

int main(int argc, char* argv[]) {
	def_mappings = allocHM();
	FILE *in = NULL, *out = NULL;

	if (mapAllArgs(&in, &out, argc, argv) != -1) {
		if (in == NULL) {
			deallocHM(def_mappings);
			return -1;
		}
		if (out == NULL) out = stdout;
		process(in, out);
	}
	

	if (in != NULL)	fclose(in);
	if (out != NULL) fclose(out);
	deallocHM(def_mappings);
	return 0;
}

int mapAllArgs(FILE** in, FILE** out, int argc, char* argv[]) {
	int i = 1;
	for (; i < argc; i++) {

		/* Searching for spaced arguments */

		if (strcmp(argv[i], "-D") == 0) {
			if (++i < argc) {
				char* key = strtok(argv[i], "=");
				char* data = strtok(NULL, "");
				insert(key, data, def_mappings);
				continue;
			} else {
				return -1;
			}
		}

		if (strcmp(argv[i], "-I") == 0) {
			if (++i < argc) {
				continue;
			} else {
				return -1;
			}
		}

		if (strcmp(argv[i], "-o") == 0) {
			if (++i < argc) {
				*out = fopen(argv[i], "w");
				continue;
			} else {
				return -1;
			}
		}

		/* Searching for glued arguments */

		char beginning[2];
		strncpy(beginning, argv[i], 2);
		char* new_arg = argv[i] + 2;

		if (strcmp(beginning, "-D") == 0) {
			char* key = strtok(new_arg, "=");
			char* data = strtok(NULL, "");
			insert(key, data, def_mappings);
			continue;
		}

		if (strcmp(beginning, "-I") == 0) {
			continue;
		}

		if (strcmp(beginning, "-o") == 0) {
			*out = fopen(new_arg, "w");
			continue;
		}

		/* No matching args => in file */

		*in = fopen(argv[i], "r");
	}

	return 0;
}

void process(FILE* in, FILE* out) {
	char* line = NULL;
	int read = 0;
	size_t len = 0;

	while ((read = getline(&line, &len, in)) != -1) {
		char* statement = NULL;

		if ((statement = strstr(line, DEF)) != 0) {

			/* #define statement found */

			statement += 8;
			char* key = strtok(statement, " ");
			char* data = strtok(NULL, "\n");					// remaining text in line
			data = multiLineDefine(in, data, strlen(data));		// checks for defines on multiple lines
			char* definition = computeString(data);				// replaces strings already defined in hashmap

			insert(key, definition, def_mappings);
			free(data);
			free(definition);
		} else if((statement = strstr(line, UNDEF)) != 0) {

			/* #undef statement found */

			statement += 7;
			char* key = strtok(statement, "\n");
			erase(key, def_mappings);
		} else {

			/* No macro */

			char* out_line = computeString(line);
			fprintf(out, "%s", out_line);
			free(out_line);
		}
	}

	free(line);
}