#include "hashmap.h"
#include "immutable_array.h"
#include "text_processing.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int print;

int mapAllArgs(FILE**, FILE**, int, char**);		// map all arguments
void process(FILE*, FILE*);							// process input file

int main(int argc, char* argv[]) {
	FILE *in = NULL, *out = NULL;
	def_mappings = allocHM(HASHMAP_SIZE);
	dir_array	 = allocArray(10);
	add("./_test/inputs/", &dir_array);

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
	deallocArray(dir_array);
	return 0;
}

int mapAllArgs(FILE** in, FILE** out, int argc, char* argv[]) {
	int i = 1;
	for (; i < argc; i++) {
		char beginning[2];
		char* new_arg;

		/* Searching for spaced arguments */

		if (strcmp(argv[i], "-D") == 0) {
			if (++i < argc) {
				char *data, *key, *definition;
				data = strstr(argv[i], "=");
				if (data == 0) data = "";
				else 		   data += 1;
				key = strtok(argv[i], "=");
				definition = formatDefine(data);
				insert(key, definition, def_mappings);
				free(definition);
				continue;
			} else {
				return -1;
			}
		}

		if (strcmp(argv[i], "-I") == 0) {
			if (++i < argc) {
				char* path = formatDirName(argv[i]);
				add(path, &dir_array);
				free(path);
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

		strncpy(beginning, argv[i], 2);
		new_arg = argv[i] + 2;

		if (strcmp(beginning, "-D") == 0) {
			char *data, *key, *definition;
			data = strstr(argv[i], "=");
			if (data == 0) data = "";
			else 		   data += 1;
			key = strtok(new_arg, "=");
			definition = formatDefine(data);
			insert(key, definition, def_mappings);
			free(definition);
			continue;
		}

		if (strcmp(beginning, "-I") == 0) {
			char* path = formatDirName(new_arg);
			add(path, &dir_array);
			free(path);
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
	char* line;
	int read = 0;

	while ((read = readLine(in, &line)) != -1) {
		int quotes = -1;
		char* quotes_ptr = line;

		do {
			char* work_string = NULL;						// string to pe processed
			char* next_quotes = NULL;						// pointer to next set of quotes
			char* statement = NULL;
			quotes++;										// number of quotes found in line increased
			next_quotes = strstr(quotes_ptr, "\"");			// getting next quote in line

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

			statement = NULL;
			if ((statement = strstr(work_string, DEF)) != 0) {

				/* #define statement found */

				char *key, *data, *definition;
				statement = strstr(line, DEF) + 8;
				key = strtok(statement, " ");
				data = strtok(NULL, "\n");							// remaining text in line
				
				if (data != NULL) {
					data = multiLineDefine(in, data, strlen(data));	// checks for defines on multiple lines
					definition = computeString(data);				// replaces strings already defined in hashmap
					free(data);
				} else {
					definition = calloc(2, 1);
					definition[0] = ' ';							// dummy definition for key
				}

				insert(key, definition, def_mappings);
				free(definition);

				quotes_ptr = NULL;
			} else if((statement = strstr(work_string, UNDEF)) != 0) {

				/* #undef statement found */

				char* key;
				statement = strstr(line, UNDEF) + 7;
				key = strtok(statement, "\n");						// extracts key
				erase(key, def_mappings);							// removes item with maching from hashmap
			} else if((statement = strstr(work_string, IFDEF)) != 0){

				/* #ifdef statement found */

				char* key;
				A_Item it;
				statement = strstr(line, IFDEF) + 7;
				key = strtok(statement, "\n");						// extracts key
				it = search(key, def_mappings);						// searches for item in hashmap

				if (it == NULL) print = 0;							// sets program to not print anymore
			} else if((statement = strstr(work_string, IFNDEF)) != 0){

				/* #ifndef statement found */

				char* key;
				A_Item it;
				statement = strstr(line, IFNDEF) + 8;
				key = strtok(statement, "\n");						// extracts key
				it = search(key, def_mappings);						// searches for item in hashmap

				if (it == NULL) print = 1;							// if key is present in hashmap, allows program to print to out file
				else  			print = 0;							// if not, blocks program from writing to file
			} else if ((statement = strstr(work_string, IF)) != 0) {

				/* #if statement found */

				char *condition;
				int eval_cond;
				statement = strstr(line, IF) + 4;
				condition = computeString(strtok(statement, "\n"));			// string value of condition									
				if (isNumber(condition) != -1) eval_cond = atoi(condition);	// convet to int if the string is a number, otherwise:
				else 						   print = -1;					// blocks printing AND blocks evaluating other statements

				free(condition);

				if (eval_cond != 0 && print == 0) print = 1;		// enables printing to out file
				if (eval_cond == 0 && print == 1) print = 0;		// desables printing to out file
			} else if((statement = strstr(work_string, ELSE)) != 0) {

				/* #else statement found */

				if (print == 0) print = 1;
				else if (print == 1) print = 0;
			} else if((statement = strstr(work_string, ELIF)) != 0) {

				/* #elif statement found */

				if (print == 0) {
					char *condition;
					int eval_cond;
					statement = strstr(line, ELIF) + 6;
					condition = computeString(strtok(statement, "\n"));			// string value of condition
					eval_cond;

					if (isNumber(condition) != -1) eval_cond = atoi(condition);	// convet to int if the string is a number, otherwise:
					else 						   print = -1;					// blocks printing AND blocks evaluating other statements

					free(condition);

					if (eval_cond != 0 && print == 0) print = 1; 	// enables printing to out file
					if (eval_cond == 0 && print == 1) print = 0;	// desables printing to out file
				}
			}else if((statement = strstr(work_string, ENDIF)) != 0) {

				/* #endif statement found */

				print = 1;											// enables printing to out file
			} else if((statement = strstr(work_string, INC)) != 0) {

				/* #include statement found */

				if (print == 1) {
					char* file = strtok(strstr(line, "\""), "\"");				// getting file name

					FILE* include_file = searchDirArray(file, dir_array);		// openning file

					if (include_file != NULL) {
						process(include_file, out);
						fclose(include_file);
					} else {
						fprintf(stderr, "12");
						return;
					}
					quotes_ptr = NULL;
				}
			}else {

				/* No macro */

				if (print == 1) {
					char* out_line = computeString(work_string);
					fprintf(out, "%s", out_line);
					free(out_line);
				}
			}

			free(work_string);
		} while(quotes_ptr != NULL);

		free(line);
	}
	free(line);
}