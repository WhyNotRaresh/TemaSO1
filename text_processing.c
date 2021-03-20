#include "text_processing.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

char* formatDirName(const char* input_arg) {
	char* path = NULL;
	int input_len = strlen(input_arg);

	if (input_arg[input_len - 1] == '/') {
		path = (char*) calloc (input_len + 1, 1);
		strncpy(path, input_arg, input_len);
	} else {
		path = (char*) calloc (input_len + 2, 1);
		strncpy(path, input_arg, input_len);
		path[input_len] = '/';
	}

	return path;
}

char* formatDefine(const char* input_arg) {
	char* definition = NULL;
	int input_len = strlen(input_arg);

	if (isNumber(input_arg) == 1) {
		definition = calloc(input_len + 1, 1);
		strncpy(definition, input_arg, input_len);
	} else {
		definition = calloc(input_len + 3, 1);
		definition[0] = '\"'; definition[input_len + 1] = '\"';
		strncpy(definition + 1, input_arg, input_len);
	}

	return definition;
}

int canBeName(const char c) {
	int ret = -1;
	if (isalpha(c)) ret = 1;
	if (isdigit(c)) ret = 1;
	if (strchr("_-", c)) ret = 1;

	return ret;
}

int isNumber(const char* str) {
	int i = 0;
	for (; i < strlen(str); i++) {
		if (str[i] < '0' || str[i] > '9')
			return -1;
	}
	return 1;
}

char* computeString(char* line) {
	int inApostrophe = 0;
	char apostrophe = '\'';
	int word_start = -1;
	int line_len = strlen(line);

	int new_line_len = strlen(line);							// alloc'd size of new line
	char* new_line = (char*) calloc (new_line_len + 1, 1);		// computet string of input
	char* new_line_ptr = new_line;								// pointer to last unwritten chr in new line
	A_Item it;
	char* result;

	int i = 0;
	for (; i <= line_len; i++) {

		// checking if we would write outside of alloc'd memory
		int new_line_index = new_line_ptr - new_line;
		if (new_line_index >= new_line_len - 1) {

			/* Realloc'ing new line */

			new_line_len *= 2;
			new_line = realloc(new_line, new_line_len);
			new_line_ptr = new_line + new_line_index;
		}

		/* Checking apostrophes */

		if (line[i] == apostrophe) {
			*(new_line_ptr++) = line[i];
			inApostrophe++;
			continue;
		}

		if (inApostrophe % 2 == 1){

			/* if in quotes/apostrophes just add chr to new line character */

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

			if (i == line_len || (word_start != -1 && canBeName(line[i]) == -1)) {

				/* End of word */

				int word_len = i - word_start;
				char* word = (char*) calloc (word_len + 1, 1);
				strncpy(word, line + word_start, word_len);

				/* Searching Define map */

				it = search(word, def_mappings);
				if (it != NULL) {
					int data_len = strlen(it->data);

					int new_line_index = new_line_ptr - new_line;
					// checking if we would write outside of alloc'd memory
					if (new_line_index + data_len >= new_line_len - 1) {

						/* Realloc'ing new line */

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

	/* Truncating result to exact size */

	result = (char*) calloc (new_line_ptr - new_line + 1, 1);
	strncpy(result, new_line, new_line_ptr - new_line);
	free(new_line);

	return result;
}

char* multiLineDefine(FILE* in, char* data, int def_len) {
	char* backslash, *definition;
	int i;
	definition = (char*) calloc (def_len + 1, 1);
	strncpy(definition, data, def_len);


	if (data[strlen(data) - 1] == '\\') {

		/* Multi Line Define */

		char* new_line = NULL;
		size_t line_len = 0;
		size_t read = 0;

		while ((read = getline(&new_line, &line_len, in)) != -1) {
			char* token = strtok(new_line, "\n");	// newt line of the definition

			/* Alloc'ing new space for new definiton */

			char* new_def = (char*) calloc(def_len + strlen(token) + 1, 1);

			strncpy(new_def, definition, def_len);
			strncpy(new_def + def_len, token, strlen(token));
			def_len = strlen(new_def);

			free(definition);
			definition = new_def;

			// Exit condition
			backslash = NULL;
			if ((backslash = strstr(new_line, "\\")) == 0) {
				break;
			}
		}

		free(new_line);

		/* Replacing specific chrs from definition */
		
		for (i = 0; i < def_len; i++) {
			if (definition[i] == '\t' ||
				definition[i] == '\\')
				definition[i] = ' ';
		}
	}

	definition = realloc(definition, def_len + 1);
	definition[def_len] = '\0';

	return definition;
}

FILE* searchDirArray(char* file, Array dir_array) {
	int i = 0;
	FILE* out = fopen(file, "r");
	if (out != NULL) return out;

	for (; i < dir_array.last; i++) {
		const char* dir = get(i, dir_array);
		char* path = (char*) calloc (strlen(dir) + strlen(file) + 2, 1);

		strncpy(path, dir, strlen(dir));
		strcat(path, file);

		out = fopen(path, "r");
		free(path);
		if (out != NULL) return out;
	}

	return NULL;
}