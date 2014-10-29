#include <string.h>

#include "globaldefs.h"
#include "filemanager.h"
#include "console.h"

int file_loadString(const char * filename, char ** output, int * length, const int debugmode){
	FILE *file = fopen(filename, "r");
	if(!file){
//		if(debugmode)fprintf(stderr, "ERROR -- file does not exist\n");
		if(debugmode)console_printf("ERROR -- file %s does not exist\n", filename);
		*length = 0;
		return FALSE;
	}
	fseek(file, 0, SEEK_END);
	*length = ftell(file);
//	if(debugmode && *length == 0) fprintf(stderr, "ERROR -- File has no contents\n");
	if(debugmode && *length == 0) console_printf("ERROR -- File has no contents\n");
	if(debugmode > 1) console_printf("DEBUG -- Opened file %s with length %i bytes\n", filename, *length);
	rewind(file);
	*output = malloc(*length+1);
	memset(*output, 0, *length+1);
	fread(*output, 1, *length, file);
	if(debugmode > 2) console_nprintf(*length + 37, "DEBUG -- Opened file contents: \n %s \n", *output);
	fclose(file);
	if(debugmode > 1) console_printf("DEBUG -- Closed file %s \n", filename);
	return *length; //will return false if length is 0
}
//UNTESTED
int file_loadStringNoLength(const char * filename, char ** output, const int debugmode){
	FILE *file = fopen(filename, "r");
	if(!file){
		if(debugmode)console_printf("ERROR -- file %s does not exist\n", filename);
		return FALSE;
	}
	fseek(file, 0, SEEK_END);
	int length = ftell(file);
	if(debugmode && length == 0) console_printf("ERROR -- File has no contents\n");
	if(debugmode > 1) console_printf("DEBUG -- Opened file %s with length %i bytes\n", filename, length);
	rewind(file);
	*output = malloc(length+1);
	memset(*output, 0, length+1);
	fread(*output, 1, length, file);
	if(debugmode > 2) console_nprintf(length + 37, "DEBUG -- Opened file contents: \n %s \n", *output);
	fclose(file);
	if(debugmode > 1) console_printf("DEBUG -- Closed file %s \n", filename);
	return length; //will return false if length is 0
}
