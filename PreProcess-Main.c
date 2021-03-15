#include "hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAXDEF 100

#define DEF 	"#define"
#define UNDEF 	"#undef"

void mapAllArgs(FILE**, FILE**, int, char**);		// map all arguments
void process(FILE*, FILE*);							// process input file

void addNewLineToOutput(FILE*, char*);

int main(int argc, char* argv[]) {
	allocHM(MAXDEF);
	FILE *in = NULL, *out = NULL;

	mapAllArgs(&in, &out, argc, argv);
	if (out == NULL) out = stdout;

	process(in, out);

	if (in != NULL)	fclose(in);
	if (out != NULL) fclose(out);
	deallocHM();
	return 0;
}

void mapAllArgs(FILE** in, FILE** out, int argc, char* argv[]) {
	int i = 1;
	for (; i < argc; i++) {

		/* Searching for spaced arguments */

		if (strcmp(argv[i], "-D") == 0) {
			if (++i < argc) {
				char* key = strtok(argv[i], "=");
				char* data = strtok(NULL, "");
				insert(key, data);
				continue;
			} else {
				return;
			}
		}

		if (strcmp(argv[i], "-I") == 0) {
			if (++i < argc) {
				continue;
			} else {
				return;
			}
		}

		if (strcmp(argv[i], "-o") == 0) {
			if (++i < argc) {
				*out = fopen(argv[i], "w");
				continue;
			} else {
				return;
			}
		}

		/* Searching for glued arguments */

		char beginning[2];
		strncpy(beginning, argv[i], 2);
		char* new_arg = argv[i] + 2;

		if (strcmp(beginning, "-D") == 0) {
			char* key = strtok(new_arg, "=");
			char* data = strtok(NULL, "");
			insert(key, data);
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
}

void process(FILE* in, FILE* out) {
	char* line = NULL;
	int read = 0;
	size_t len = 0;

	while ((read = getline(&line, &len, in)) != -1) {
		char* statement;

		if ((statement = strstr(line, DEF)) != 0) {
			statement += 8;
			char* key = strtok(statement, " ");
			char* data = strtok(NULL, "\n");
			insert(key, data);
		} else if((statement = strstr(line, UNDEF)) != 0) {
			
		}
		else {
			addNewLineToOutput(out, line);
		}
	}

	free(line);
}

void addNewLineToOutput(FILE* out, char* line) {
	int inQuotes = 0, inApostrophe = 0;
	char quotes = '\"', apostrophe = '\'';
	int word_start = -1;

	int i = 0;
	for (; i < strlen(line); i++) {
		if (line[i] == quotes) {
			fprintf(out, "%c", line[i]);
			inQuotes++;
			continue;
		}
		if (line[i] == apostrophe) {
			fprintf(out, "%c", line[i]);
			inApostrophe++;
			continue;
		}

		if (inQuotes % 2 == 1 || inApostrophe % 2 == 1){
			/* if in quotes/apostrope just print character */
			fprintf(out, "%c", line[i]);
		} else {
			/* if outside of quotes, work on text */
			if (word_start == -1 && isalpha(line[i])) {
				word_start = i;
			}

			if (word_start == -1){
				fprintf(out, "%c", line[i]);
				continue;
			}

			if (word_start != -1 && !isalpha(line[i])) {
				int word_len = i - word_start + 1;
				char* key_toSearch = (char*) calloc (word_len + 1, sizeof(char));
				strncpy(key_toSearch, line + word_start, word_len);

				A_Item it = search(key_toSearch);
				if (it == NULL) {
					fprintf(out, "%s", key_toSearch);
				} else {
					fprintf(out, "%s", it->data);
				}

				free(key_toSearch);
				word_start = -1;	
			}
		}
	}
}