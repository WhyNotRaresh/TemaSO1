#include "str_aux.h"
#include <string.h>
#include <ctype.h>

int canBeName(const char c) {
	int ret = -1;
	if (isalpha(c)) ret = 1;
	if (isdigit(c)) ret = 1;
	if (strchr("_-", c)) ret = 1;

	return ret;
}