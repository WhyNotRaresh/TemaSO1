#include "hashmap.h"
#include "array.h"
#include "text_processing.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

int print;

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

		print = 1;
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
		int quotes = -1;
		char* quotes_ptr = line;

		do {
			char* work_string = NULL;						// string to pe processed
			quotes++;										// number of quotes found in line increased
			char* next_quotes = strstr(quotes_ptr, "\"");	// getting next quote in line

			if (next_quotes != 0) {
				work_string = (char*) calloc(next_quotes - quotes_ptr + 2, 1);
				strncpy(work_string, quotes_ptr, next_quotes - quotes_ptr + 1);
				quotes_ptr = next_quotes + 1;
			} else {
				work_string = (char*) calloc(strlen(line) + line - quotes_ptr + 1, 1);
				strncpy(work_string, quotes_ptr, strlen(line) + line - quotes_ptr);
				quotes_ptr = NULL;
			}

			if (quotes % 2 == 1) {

				/* Working inside Quotes => just print line */

				if (print == 1) {
					fprintf(out, "%s", work_string);
				}
				free(work_string);
				continue;
			}

			/* Working outside of quotes => process sting */

			char* statement = NULL;
			if ((statement = strstr(work_string, DEF)) != 0) {

				/* #define statement found */

				statement = line + 8;
				char* key = strtok(statement, " ");
				char* data = strtok(NULL, "\n");					// remaining text in line
				data = multiLineDefine(in, data, strlen(data));		// checks for defines on multiple lines
				char* definition = computeString(data);				// replaces strings already defined in hashmap

				insert(key, definition, def_mappings);
				free(data);
				free(definition);

				quotes_ptr = NULL;
			} else if((statement = strstr(work_string, UNDEF)) != 0) {

				/* #undef statement found */

				statement += 7;
				char* key = strtok(statement, "\n");
				erase(key, def_mappings);
			} else if ((statement = strstr(work_string, IF)) != 0) {

				/* #if statement found */

				statement += 4;
				char* condition = computeString(strtok(statement, "\n"));
				int eval_cond;
				if (isNumber(condition) != -1) eval_cond = atoi(condition);
				else 						   print = -1;

				free(condition);

				if (eval_cond != 0 && print == 0) print = 1; 
				if (eval_cond == 0 && print == 1) print = 0;
			} else if((statement = strstr(work_string, ELSE)) != 0) {

				/* #else statement found */

				if (print == 0) print = 1;
				else if (print == 1) print = 0;
			} else if((statement = strstr(work_string, ELIF)) != 0) {

				/* #elif statement found */

				if (print == 0) {
					statement += 6;
					char* condition = computeString(strtok(statement, "\n"));
					int eval_cond;

					if (isNumber(condition) != -1) eval_cond = atoi(condition);
					else 						   print = -1;

					free(condition);

					if (eval_cond != 0 && print == 0) print = 1; 
					if (eval_cond == 0 && print == 1) print = 0;
				}
			}else if((statement = strstr(work_string, ENDIF)) != 0) {

				/* #endif statement found */

				print = 1;
			} else {

				/* No macro */

				if (print == 1) {
					char* out_line = computeString(work_string);
					fprintf(out, "%s", out_line);
					free(out_line);
				}
			}

			free(work_string);
		} while(quotes_ptr != NULL);
	}

	free(line);
}