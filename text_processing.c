#include "text_processing.h"
#include <string.h>
#include <ctype.h>

int canBeName(const char c) {
	int ret = -1;
	if (isalpha(c)) ret = 1;
	if (isdigit(c)) ret = 1;
	if (strchr("_-", c)) ret = 1;

	return ret;
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