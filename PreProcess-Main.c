#include "hashmap.h"
#include "array.h"
#include "str_aux.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define DEF 	"#define"
#define UNDEF 	"#undef"

HashMap def_mappings;

int mapAllArgs(FILE**, FILE**, int, char**);		// map all arguments
void process(FILE*, FILE*);							// process input file
char* computeLine(char*);							// computes new line for output
char* multiLineDefine(FILE*, char*);

int main(int argc, char* argv[]) {
	def_mappings = allocHM();
	FILE *in = NULL, *out = NULL;

	if (mapAllArgs(&in, &out, argc, argv) != -1) {
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
		char* statement;

		if ((statement = strstr(line, DEF)) != 0) {

			/* #define statement found */

			statement += 8;
			char* key = strtok(statement, " ");
			char* data = computeLine(strtok(NULL, "\n"));

			insert(key, data, def_mappings);
			free(data);
		} else if((statement = strstr(line, UNDEF)) != 0) {

			/* #undef statement found */

			statement += 7;
			char* key = strtok(statement, "\n");
			erase(key, def_mappings);
		} else {

			/* No macro */

			char* out_line = computeLine(line);
			fprintf(out, "%s", out_line);
			free(out_line);
		}
	}

	free(line);
}

char* computeLine(char* line) {
	int inQuotes = 0, inApostrophe = 0;
	char quotes = '\"', apostrophe = '\'';
	int word_start = -1;

	char* new_line = (char*) calloc (strlen(line) * 2, 1);
	char* new_line_ptr = new_line;

	int i = 0;
	for (; i < strlen(line); i++) {
		if (line[i] == quotes) {
			*(new_line_ptr++) = line[i];
			inQuotes++;
			continue;
		}
		if (line[i] == apostrophe) {
			*(new_line_ptr++) = line[i];
			inApostrophe++;
			continue;
		}

		if (inQuotes % 2 == 1 || inApostrophe % 2 == 1){

			/* if in quotes/apostrope just print character */

			*(new_line_ptr++) = line[i];
		} else {

			/* if outside of quotes, work on text */

			if (word_start == -1 && isalpha(line[i])) {
				/* New word */
				word_start = i;
			}

			if (word_start == -1){
				/* Outside of any word */
				*(new_line_ptr++) = line[i];
				continue;
			}

			if (word_start != -1 && canBeName(line[i]) == -1) {

				/* End of word */

				int word_len = i - word_start;
				char* word = (char*) calloc (word_len + 1, sizeof(char));
				strncpy(word, line + word_start, word_len);

				/* Searching Define map */

				A_Item it = search(word, def_mappings);
				if (it != NULL) {
					strncpy(new_line_ptr, it->data, strlen(it->data));
					new_line_ptr += strlen(it->data);
				} else {

					/* Just printing normal word */

					strncpy(new_line_ptr, word, strlen(word));
					new_line_ptr += strlen(word);
				}
				*(new_line_ptr++) = line[i];
				
				free(word);
				word_start = -1;
			}
		}
	}

	if (word_start != -1) {
		strncpy(new_line_ptr, line + word_start, 1);
	}

	return new_line;
}
