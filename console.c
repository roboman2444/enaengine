// needed for va_ stuff
#include <stdarg.h>
#include <string.h>

#include "globaldefs.h"
#include "console.h"

char ** consoleOutputBuffer;

int maxConsoleBufferLines = 2048; //todo make into cvar
int maxConsoleBufferLineLength = 2048; //todo make cvar/define

int consoleCircleBufferPlace = 0; // start!... err end
int consoleStringsPrinted = 0; // useful to not print blank lines + reallocation

char *tempPrint;

int resizeConsoleBuffer(int size){
	if(size == maxConsoleBufferLines) return size;//uh... why should i resize to the same size?

	char **newBuffer = malloc(size * sizeof(char *));
	if(consoleStringsPrinted < size) consoleStringsPrinted = size; // set the number of printed strings to the size of the buffer if they are too much... needed if i eventually resize the buffer back up to that size or such


	int toClean = maxConsoleBufferLines - consoleStringsPrinted;
	int n;
	for(n = 0; n < toClean; n++){
		int check = (n + consoleCircleBufferPlace) % maxConsoleBufferLines;
		if(consoleOutputBuffer[check]) free(consoleOutputBuffer[check]);
		consoleOutputBuffer[check] = 0;
	}

	consoleCircleBufferPlace -= consoleStringsPrinted;
	if(consoleCircleBufferPlace < 0) consoleCircleBufferPlace += size; //to get to the "start" of the buffer

	for(n = 0; n < consoleStringsPrinted; n++, consoleCircleBufferPlace++){ //fill new buffer with old buffers data, at place 0
		newBuffer[n] = consoleOutputBuffer[consoleCircleBufferPlace]; // moved for readability... used to have this in the for
	}

	free(consoleOutputBuffer);
	consoleOutputBuffer = newBuffer;
	consoleCircleBufferPlace = n;

	maxConsoleBufferLines = size;
	return maxConsoleBufferLines;
}
int initConsoleSystem(void){ //should work for now
	tempPrint = malloc(maxConsoleBufferLineLength * sizeof(char));
	resizeConsoleBuffer(maxConsoleBufferLines);
	return TRUE; // good enough for now
}
int consolePrintf(const char *format, ...){//very similar to printf... oh noes muh gnu source code as a ref!


	//initialize string
	//slap string into buffer
	//move down into empty buffer spot
	va_list arg;
	int done;
	int length;
	va_start(arg, format);
	done = vsnprintf(tempPrint, maxConsoleBufferLineLength, format, arg);
	va_end(arg);
	length = strlen(tempPrint) + 2;
	consoleOutputBuffer[consoleCircleBufferPlace] = realloc(consoleOutputBuffer[consoleCircleBufferPlace], length * sizeof(char)); //reallocate that string in the buffer to only the size needed
	strncpy(consoleOutputBuffer[consoleCircleBufferPlace], tempPrint, length * sizeof(char));

	consoleCircleBufferPlace++;
	consoleCircleBufferPlace = (consoleCircleBufferPlace % maxConsoleBufferLines); // add one to the position
	if(consoleStringsPrinted < maxConsoleBufferLines)consoleStringsPrinted++; //add one to how "full" the buffer is

	//maybe call a function to update vbos for the console or something
	//maybe print to cout? as well?



	return done;
}
