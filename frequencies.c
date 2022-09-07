#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "frequencies.h"
#include "miniunit.h"
#include "clog.h"
//..
bool calc_frequencies(Frequencies freqs, const char* path, const char** a_error) {
	const char* mode = "r";
	FILE* file = fopen(path, mode);
	if(file == NULL) {
		*a_error = strerror(errno);
		return false;
	}
	for(uchar ch = fgetc(file); !feof(file); ch = fgetc(file)) {
		freqs[ch]++;
	}
	fclose(file);
	return true;
}
