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
char* computeString(char*);							// computes new line for output
char* multiLineDefine(FILE*, char*, int);

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

char* computeString(char* line) {
	int inQuotes = 0, inApostrophe = 0;
	char quotes = '\"', apostrophe = '\'';
	int word_start = -1;

	int new_line_len = strlen(line);							// alloc'd size of new line
	char* new_line = (char*) calloc (new_line_len + 1, 1);		// computet string of input
	char* new_line_ptr = new_line;								// pointer to last unwritten chr in new line

	int i = 0;
	for (; i < strlen(line); i++) {

		// checking if we would write outside of alloc'd memory
		int new_line_index = new_line_ptr - new_line;
		if (new_line_index >= new_line_len - 1) {

			/* Reallocing new line */

			new_line_len *= 2;
			new_line = realloc(new_line, new_line_len);
			new_line_ptr = new_line + new_line_index;
		}

		/* Checking of quotes and apostrophes */

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

			/* if in quotes/apostrophes just print character */

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
				char* word = (char*) calloc (word_len + 1, 1);
				strncpy(word, line + word_start, word_len);

				/* Searching Define map */

				A_Item it = search(word, def_mappings);
				if (it != NULL) {
					int data_len = strlen(it->data);

					int new_line_index = new_line_ptr - new_line;
					// checking if we would write outside of alloc'd memory
					if (new_line_index + data_len >= new_line_len - 1) {

						/* Reallocing new line */

						new_line_len += data_len + 2;
						new_line = realloc(new_line, new_line_len);
						new_line_ptr = new_line + new_line_index;
					}


					strncpy(new_line_ptr, it->data, data_len);
					new_line_ptr += data_len;
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
		*(new_line_ptr++) = *(line + word_start);
	}

	/* Truncating result to exact size */

	char* result = (char*) calloc (new_line_ptr - new_line + 1, 1);
	strncpy(result, new_line, new_line_ptr - new_line);
	free(new_line);

	return result;
}

char* multiLineDefine(FILE* in, char* data, int def_len) {
	char* definition = (char*) calloc (def_len + 1, 1);
	strncpy(definition, data, def_len);

	if (data[strlen(data) - 1] == '\\') {

		/* Multi Line Define */

		char* new_line = NULL;
		size_t line_len = 0;
		size_t read = 0;

		while ((read = getline(&new_line, &line_len, in)) != -1) {
			char* token = strtok(new_line, "\n");
			char* new_def = (char*) calloc(def_len + strlen(token) + 1, 1);

			strncpy(new_def, definition, def_len);
			strncpy(new_def + def_len, token, strlen(token));
			def_len = strlen(new_def);

			free(definition);
			definition = new_def;

			char* backslash = NULL;
			if ((backslash = strstr(new_line, "\\")) == 0) {
				break;
			}
		}

		free(new_line);
		
		int i = 0;
		for (; i < def_len; i++) {
			if (definition[i] == '\t' ||
				definition[i] == '\\')
				definition[i] = ' ';
		}
	}

	definition = realloc(definition, def_len + 1);
	definition[def_len] = '\0';

	return definition;
}