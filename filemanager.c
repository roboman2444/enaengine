#include <string.h>

#include "globaldefs.h"
#include "filemanager.h"

int loadFileString(const char * filename, char ** output, int * length, int debugmode){
	FILE *file = fopen(filename, "r");
	if(!file){
		if(debugmode)fprintf(stderr, "ERROR -- file does not exist\n");
		*length = 0;
		return FALSE;
	}
	fseek(file, 0, SEEK_END);
	*length = ftell(file);
	if(debugmode && *length == 0) fprintf(stderr, "ERROR -- File has no contents\n");
	if(debugmode > 1) printf("DEBUG -- Opened file %s with length %i bytes\n", filename, *length);
	rewind(file);
	*output = malloc(*length+1);
	fread(*output, 1, *length, file);
	if(debugmode > 2) printf("DEBUG -- Opened file contents: \n %s \n", *output);
	fclose(file);
	if(debugmode > 1) printf("DEBUG -- Closed file %s \n", filename);
	return *length; //will return false if length is 0
}
//UNTESTED
int loadFileStringNoLength(const char * filename, char ** output, int debugmode){
	FILE *file = fopen(filename, "r");
	if(!file){
		if(debugmode)fprintf(stderr, "ERROR -- file does not exist\n");
		return FALSE;
	}
	fseek(file, 0, SEEK_END);
	int length = ftell(file);
	if(debugmode && length == 0) fprintf(stderr, "ERROR -- File has no contents\n");
	if(debugmode > 1) printf("DEBUG -- Opened file %s with length %i bytes\n", filename, length);
	rewind(file);
	*output = malloc(length+1);
	fread(*output, 1, length, file);
	if(debugmode > 2) printf("DEBUG -- Opened file contents: \n %s \n", *output);
	fclose(file);
	if(debugmode > 1) printf("DEBUG -- Closed file %s \n", filename);
	return length; //will return false if length is 0
}


