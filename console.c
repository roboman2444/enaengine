#include "GL/glew.h" //todo remove
#include "GL/gl.h" //todo remove

// needed for va_ stuff
#include <stdarg.h>
#include <string.h>

#include "globaldefs.h"
#include "console.h"
#include "vbomanager.h"
#include "shadermanager.h"
#include "textmanager.h"

typedef struct consolechar_s { //todo redo
	GLfloat verts[16];
}consolechar_t;

char ** consoleOutputBuffer;


int maxConsoleBufferLines = 2048; //todo make into cvar
int maxConsoleBufferLineLength = 2048; //todo make cvar/define

int consoleCircleBufferPlace = 0; // start!... err end
int consoleStringsPrinted = 0; // useful to not print blank lines + reallocation

char *tempPrint;

int consoleDisplayNeedsUpdate;
char currentConsoleTextTrackerFlag;


char * consoleFont = "FreeMono.ttf";



int consoleWidth = 64;
int consoleHeight = 20;


//a flag system to delete text not drawn in the past x frames
//todo

consoleTextTracker_t * texttracker;
unsigned int consoleDrawLines;

//int consoleVBO;

int updateConsoleText(unsigned int offset){

	if(offset > consoleStringsPrinted - consoleHeight) offset = consoleStringsPrinted - consoleHeight;
	currentConsoleTextTrackerFlag = !currentConsoleTextTrackerFlag;
/*
	if(!consoleVBO){
		consoleVBO= createAndAddVBORPOINT("console", 2);
		glBindVertexArray(consoleVBO->vaoid);
		glEnableVertexAttribArray(POSATTRIBLOC);
		glVertexAttribPointer(POSATTRIBLOC, 2, GL_FLOAT, GL_FALSE, 4* sizeof(GLfloat), 0);
		glEnableVertexAttribArray(TCATTRIBLOC);
		glVertexAttribPointer(TCATTRIBLOC, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));
	}
*/
	if(!texttracker){
		texttracker = malloc(consoleHeight * sizeof(consoleTextTracker_t));
		memset(texttracker, 0, consoleHeight * sizeof(consoleTextTracker_t));
	}
	consoleDrawLines= consoleStringsPrinted;
	if(consoleDrawLines > consoleHeight) consoleDrawLines = consoleHeight;

	int n, p = consoleCircleBufferPlace - consoleDrawLines - offset;
	for(n = 0; n < consoleDrawLines; n++){
//		printf("p = %i\n",p);
		char fg[3] = {255, 255, 255};
		text_t *t = createAndAddTextFindFontRPOINT(consoleOutputBuffer[p], consoleFont, 10, 1, fg);
		texttracker[n].flag = currentConsoleTextTrackerFlag;
		texttracker[n].textid = t->myid;
		texttracker[n].textureid = t->textureid;
		texttracker[n].width = t->width;
		texttracker[n].height = t->height;
//		printf("texttracker: %i, %ix%i\n", t->textureid, t->width, t->height);
		p = (p+1) % maxConsoleBufferLines;
	}


	return TRUE;


/*
	int i;
	for(i = 0; i < consoleHeight; i++;){
		//delete old crap
		if(texttracker[i].flag
	}
*/
}
/*
consolechar_t generateCharacter(float offsetx, float offsety, float scalex, float scaley, char c){
	consolechar_t output;
	output.verts[0] = offsetx;
	output.verts[1] = offsety;
			// 0.0, 0.0,
	output.verts[4] = offsetx+scalex;
	output.verts[5]	= offsety;
			//, 0.0, 0.0,
	output.verts[8] = offsetx+scalex;
	output.verts[9]	= offsety+scaley;

	output.verts[12]= offsetx;
	output.verts[13]= offsety+scaley;
	return output;
}
*/
/*
int updateConsoleVBO(void){
	if(!consoleVBO) return 0; // something bad
	glBindVertexArray(consoleVBO->vaoid);
	glBindBuffer(GL_ARRAY_BUFFER, consoleVBO->vboid);
	int charcount = 0;
	int n, p = consoleCircleBufferPlace;
	for(n = 0; n < consoleStringsPrinted && n < consoleHeight; n++){
		p--;
		if(p < 0) p = maxConsoleBufferLines-1;
		if(consoleOutputBuffer)charcount += strlen(consoleOutputBuffer[p]);
	}

	consolechar_t * consoleVBOTempBuffer = malloc(charcount * sizeof(consolechar_t));

	p = consoleCircleBufferPlace;
	int numchecked = 0;
	for(n = 0; n < consoleStringsPrinted && n < consoleHeight; n++){
		p--;
		if(p < 0) p = maxConsoleBufferLines-1;
		int g;
		for(g = 0; g < strlen(consoleOutputBuffer[p]); g++){
			consoleVBOTempBuffer[numchecked] = generateCharacter((float)g/consoleWidth, 1.0 - ((float)n/consoleHeight), 1.0/consoleWidth, 1.0/consoleHeight, consoleOutputBuffer[p][g]);
		}
	}
	glBufferData(GL_ARRAY_BUFFER, charcount * 16 * sizeof(GLfloat), consoleVBOTempBuffer, GL_STATIC_DRAW);
	free(consoleVBOTempBuffer);
	shaderprogram_t *program = findProgramByName("console"); //todo redo this
	if(!program->id) return FALSE;
	glUseProgram(program->id);
	GLint posattrib = findShaderAttribPos(program, "position");
	glEnableVertexAttribArray(posattrib);
	glVertexAttribPointer(posattrib, 2, GL_FLOAT, GL_FALSE, 4 *sizeof(GLfloat), 0);
	GLint tcattrib = findShaderAttribPos(program, "texCoord");
	glEnableVertexAttribArray(tcattrib);
	glVertexAttribPointer(tcattrib, 2, GL_FLOAT, GL_FALSE, 4 *sizeof(GLfloat), (void*) ( 2 * sizeof(GLfloat)));
	return TRUE;
}
*/
int deleteConsoleBuffer(void){
	int n;
	if(!consoleOutputBuffer) return FALSE; // its already deleted
	for(n = 0; n < maxConsoleBufferLines; n++){
		if(consoleOutputBuffer[n]) free(consoleOutputBuffer[n]);
		consoleOutputBuffer[n] = 0;
	}
	free(consoleOutputBuffer);
	consoleOutputBuffer = 0;
	consoleCircleBufferPlace = 0;
	consoleStringsPrinted = 0;
	consoleDisplayNeedsUpdate = TRUE;
	return TRUE; // should do for now
}

int resizeConsoleBuffer(int size){
	if(size < 1) size = 1; //force a size

	char **newBuffer = malloc(size * sizeof(char *));

//	if(!newBuffer); //todo debug
	memset(newBuffer, 0, size*sizeof(char *));

	if(consoleOutputBuffer){ //if there already is one

		//TODO since newBuffer is already zero'd  i can probably only reset the parts that were set. Some hocus pocus with consoleStringsPrinted

		if(consoleStringsPrinted > size) consoleStringsPrinted = size; // set the number of printed strings to the size of the buffer if they are too much... needed if i eventually resize the buffer back up to that size or such

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
		consoleCircleBufferPlace = n;
	} else { // and if you are doing the init
		memset(newBuffer, 0, size * sizeof(char*));
		consoleCircleBufferPlace = 0;//just in case
		consoleStringsPrinted = 0;
	}

	consoleOutputBuffer = newBuffer;
	maxConsoleBufferLines = size;
	consoleDisplayNeedsUpdate = TRUE;
	return maxConsoleBufferLines;
}
int initConsoleSystem(void){ //should work for now
	tempPrint = malloc(maxConsoleBufferLineLength * sizeof(char));
	resizeConsoleBuffer(maxConsoleBufferLines);
//	consoleVBO = createAndAddVBO("console", 2); //todo set type to something
//	consoleVAOid = consoleVBO->vaoid;
//	resizeConsoleBuffer(5); for testing resizing
	consoleDisplayNeedsUpdate = TRUE;
	currentConsoleTextTrackerFlag = 0;
	return TRUE; // good enough for now
}
int consolePrintf(const char *format, ...){//very similar to printf...
	va_list arg;
	int done;


	if(!consoleOutputBuffer){ //no console, fall back to stdout printing
		va_start(arg, format);
		done = vfprintf (stdout, format, arg); //not likely to be a tempPrint malloced, so not using it
		va_end(arg);
		return done;
	}

	if(!tempPrint) return 0;// somehow we messed up

	//initialize string
	//slap string into buffer
	//move down into empty buffer spot
	int length;
	va_start(arg, format);
	done = vsnprintf(tempPrint, maxConsoleBufferLineLength, format, arg);
	va_end(arg);

	printf(tempPrint); // possibly faster than having it re-interpret the format

	length = strlen(tempPrint) + 2;
	consoleOutputBuffer[consoleCircleBufferPlace] = realloc(consoleOutputBuffer[consoleCircleBufferPlace], length * sizeof(char)); //reallocate that string in the buffer to only the size needed
	strncpy(consoleOutputBuffer[consoleCircleBufferPlace], tempPrint, length * sizeof(char));

	consoleCircleBufferPlace++;
	consoleCircleBufferPlace = (consoleCircleBufferPlace % maxConsoleBufferLines); // add one to the position
	if(consoleStringsPrinted < maxConsoleBufferLines)consoleStringsPrinted++; //add one to how "full" the buffer is

	//maybe call a function to update vbos for the console or something


	consoleDisplayNeedsUpdate = TRUE;
	return done;
}

/*NOTE
	when using this function you have to take into account the length of all of the inputs. That includes format.
	So, if you are doing consoleNPrintf( SOMESIZE, "yar har %s\n", string); with string being 200 characters long,
	you have to take into account of the length of "yar har \n" as well. (so final would be 210 or so characters long.
*/
int consoleNPrintf(size_t size, const char *format, ...){//very similar to printf...
	va_list arg;
	int done;

	if(!consoleOutputBuffer){ //no console, fall back to stdout printing
		va_start(arg, format);
		done = vfprintf (stdout, format, arg); //not likely to be a tempPrint malloced, so not using it
		va_end(arg);
		return done;
	}

	char * bigTempPrint = malloc(size);
	if(!bigTempPrint) return 0;// somehow we messed up

	//initialize string
	//slap string into buffer
	//move down into empty buffer spot
	va_start(arg, format);
	done = vsnprintf(bigTempPrint, size, format, arg);
	va_end(arg);

	printf(bigTempPrint); // possibly faster than having it re-interpret the format

	consoleOutputBuffer[consoleCircleBufferPlace] = realloc(consoleOutputBuffer[consoleCircleBufferPlace], size); //reallocate that string in the buffer to only the size needed
	strncpy(consoleOutputBuffer[consoleCircleBufferPlace], bigTempPrint, size);
	free(bigTempPrint);

	consoleCircleBufferPlace++;
	consoleCircleBufferPlace = (consoleCircleBufferPlace % maxConsoleBufferLines); // add one to the position
	if(consoleStringsPrinted < maxConsoleBufferLines)consoleStringsPrinted++; //add one to how "full" the buffer is

	//maybe call a function to update vbos for the console or something


	consoleDisplayNeedsUpdate = TRUE;
	return done;
}
int printConsoleBackwards(void){
	printf("Console buffer backwards : \n");
	int n, p = consoleCircleBufferPlace;
	for(n = 0; n < consoleStringsPrinted; n++){
		p--;
		if(p < 0) p = maxConsoleBufferLines-1;
		if(consoleOutputBuffer)printf(consoleOutputBuffer[p]);
	}
	return TRUE;
}
