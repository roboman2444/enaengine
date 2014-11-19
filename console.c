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
#include "cvarmanager.h"
#include "glstates.h" //todo remove

typedef struct consolechar_s { //todo redo
	GLfloat verts[16];
}consolechar_t;

char ** consoleoutputbuffer;


int maxconsolebufferlines = 2048; //todo make into cvar
int maxconsolebufferlinelength = 2048; //todo make cvar/define

int consolecirclebufferplace = 0; // start!... err end
int consolestringsprinted = 0; // useful to not print blank lines + reallocation

char *tempprint;

int console_displayneedsupdate;
char currentconsoletexttrackerflag;


char * consolefont = "FreeMono.ttf";
int consolefonti = 0;



int consolewidth = 64;


//a flag system to delete text not drawn in the past x frames
//todo

consoleTextTracker_t console_texttracker;
unsigned int console_drawlines;

cvar_t cvar_console_Height = {CVAR_SAVEABLE, "console_Height", "sets the consoles display height", "20"};

//int consoleVBO;
typedef struct {
	unsigned short x0, y0, x1, y1;
	float xoff, yoff, xadvance;
} stbtt_bakedchar;
int console_updateText(unsigned int offset, const unsigned int width, const unsigned int height){
	int consoleheight = cvar_console_Height.valueint;

	if(offset > consolestringsprinted - consoleheight) offset = consolestringsprinted - consoleheight;
	currentconsoletexttrackerflag = !currentconsoletexttrackerflag;
//	if(!console_texttracker){
//		console_texttracker = malloc(consoleheight * sizeof(consoleTextTracker_t));
//		memset(console_texttracker, 0, consoleheight * sizeof(consoleTextTracker_t));
//	}
	console_drawlines= consolestringsprinted;
	if(console_drawlines > consoleheight) console_drawlines = consoleheight;

	font_t * consolefontp;

	if(!consolefonti){
		consolefontp = createAndAddFontRPOINT(consolefont);
		consolefonti = consolefontp->myid;
		console_texttracker.textureid = consolefontp->textureid;
	} else {
		consolefontp = returnFontById(consolefonti);
	}
	console_texttracker.count = 0;
	unsigned int curplace = 0;
	int n, p = consolecirclebufferplace - console_drawlines - offset;


	float charwidth = (float) 12 / height;
	float charheight= (float) 24 / width;
	float charsizeh = (float)24 / (height * 32);
	float charsizew = (float)24 / (width * 32);
	float cury = 1.0f - charheight;
	for(n = 0; n < console_drawlines; n++){
		unsigned int mycount = strlen(consoleoutputbuffer[p]);
		console_texttracker.count += mycount;
		if(console_texttracker.count > console_texttracker.maxc){
			//resize it
			console_texttracker.verts = realloc(console_texttracker.verts, 20 * console_texttracker.count * sizeof(float));
			console_texttracker.faces = realloc(console_texttracker.faces, 6 * console_texttracker.count * sizeof(unsigned int));
			for(; console_texttracker.maxc < console_texttracker.count; console_texttracker.maxc++){
				float * verts = &console_texttracker.verts[console_texttracker.maxc * 20];
				verts[2] = 0.0f;
				verts[7] = 0.0f;
				verts[12] = 0.0f;
				verts[17] = 0.0f;//z

				unsigned int * faces = &console_texttracker.faces[console_texttracker.maxc * 6];
				unsigned int mymaxc = console_texttracker.maxc * 4;
				faces[0] = mymaxc;
				faces[1] = mymaxc+1;
				faces[2] = mymaxc+2;
				faces[3] = mymaxc;
				faces[4] = mymaxc+2;
				faces[5] = mymaxc+3;
			}
		}
		GLfloat curx = -1.0f;
		int i;
		//RUN THOUGH EACH CHAR AND PUTTER HERE
		for(i = 0; i < mycount; i++){
			if(consoleoutputbuffer[p][i] == ' '){
				curx += charwidth;
			} else if(consoleoutputbuffer[p][i] == '\t'){
				curx += charwidth * 8.0f;
			} else if(consoleoutputbuffer[p][i] == '\n'){
				curx = -1.0f;
				cury -= charheight;
			} else if(consoleoutputbuffer[p][i] >= 32 && consoleoutputbuffer[p][i] < 128){
				stbtt_bakedchar *b = consoleoutputbuffer[p][i] + ((stbtt_bakedchar *)consolefontp->cdata) - 32;
				if(curx >= 1.0f){
					curx = -1.0f;
					cury -= charheight;
				}
				float ch =  (float)(b->y1 - b->y0) * charsizeh;
				float cw =  (float)(b->x1 - b->x0) * charsizew;
				/*
				float x = curx;
				float y = cury;
				float topx = x + charwidth;
				float topy = y + charheight;
				*/
				float x = curx + b->xoff * charsizew;
				float y = cury + charsizeh;
				float topx = x + cw;
				float topy = y + ch;


				float * verts = &console_texttracker.verts[curplace * 20];


				float bu = (float)b->x0/consolefontp->width;
				float tv = (float)b->y0/consolefontp->height;
				float tu = (float)b->x1/consolefontp->width;
				float bv = (float)b->y1/consolefontp->height;
				verts[0] = x;
				verts[1] = y;
				verts[3] = bu;
				verts[4] = bv;

				verts[5] = topx;
				verts[6] = y;
				verts[8] = tu;
				verts[9] = bv;

				verts[10] = topx;
				verts[11] = topy;
				verts[13] = tu;
				verts[14] = tv;

				verts[15] = x;
				verts[16] = topy;
				verts[18] = bu;
				verts[19] = tv;

//				curx += charwidth;
				curx += cw;
				curplace++;
			}
		}
		p = (p+1) % maxconsolebufferlines;
	}
	console_texttracker.count = curplace;
	console_displayneedsupdate = FALSE;


	return TRUE;


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
	states_bindVertexArray(consoleVBO->vaoid);
	states_bindBuffer(GL_ARRAY_BUFFER, consoleVBO->vboid);
	int charcount = 0;
	int n, p = consolecirclebufferplace;
	for(n = 0; n < consolestringsprinted && n < consoleheight; n++){
		p--;
		if(p < 0) p = maxconsolebufferlines-1;
		if(consoleoutputbuffer)charcount += strlen(consoleoutputbuffer[p]);
	}

	consolechar_t * consoleVBOTempBuffer = malloc(charcount * sizeof(consolechar_t));

	p = consolecirclebufferplace;
	int numchecked = 0;
	for(n = 0; n < consolestringsprinted && n < consoleheight; n++){
		p--;
		if(p < 0) p = maxconsolebufferlines-1;
		int g;
		for(g = 0; g < strlen(consoleoutputbuffer[p]); g++){
			consoleVBOTempBuffer[numchecked] = generateCharacter((float)g/consoleWidth, 1.0 - ((float)n/consoleheight), 1.0/consoleWidth, 1.0/consoleheight, consoleoutputbuffer[p][g]);
		}
	}
	glBufferData(GL_ARRAY_BUFFER, charcount * 16 * sizeof(GLfloat), consoleVBOTempBuffer, GL_STATIC_DRAW);
	free(consoleVBOTempBuffer);
	shaderprogram_t *program = findProgramByName("console"); //todo redo this
	if(!program->id) return FALSE;
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
	if(!consoleoutputbuffer) return FALSE; // its already deleted
	for(n = 0; n < maxconsolebufferlines; n++){
		if(consoleoutputbuffer[n]) free(consoleoutputbuffer[n]);
		consoleoutputbuffer[n] = 0;
	}
	free(consoleoutputbuffer);
	consoleoutputbuffer = 0;
	consolecirclebufferplace = 0;
	consolestringsprinted = 0;
	console_displayneedsupdate = TRUE;
	return TRUE; // should do for now
}

int resizeConsoleBuffer(int size){
	if(size < 1) size = 1; //force a size

	char **newbuffer = malloc(size * sizeof(char *));

//	if(!newbuffer); //todo debug
	memset(newbuffer, 0, size*sizeof(char *));

	if(consoleoutputbuffer){ //if there already is one

		//TODO since newbuffer is already zero'd  i can probably only reset the parts that were set. Some hocus pocus with consolestringsprinted

		if(consolestringsprinted > size) consolestringsprinted = size; // set the number of printed strings to the size of the buffer if they are too much... needed if i eventually resize the buffer back up to that size or such

		int toclean = maxconsolebufferlines - consolestringsprinted;
		int n;
		for(n = 0; n < toclean; n++){
			int check = (n + consolecirclebufferplace) % maxconsolebufferlines;
			if(consoleoutputbuffer[check]) free(consoleoutputbuffer[check]);
			consoleoutputbuffer[check] = 0;
		}

		consolecirclebufferplace -= consolestringsprinted;
		if(consolecirclebufferplace < 0) consolecirclebufferplace += size; //to get to the "start" of the buffer

		for(n = 0; n < consolestringsprinted; n++, consolecirclebufferplace++){ //fill new buffer with old buffers data, at place 0
			newbuffer[n] = consoleoutputbuffer[consolecirclebufferplace]; // moved for readability... used to have this in the for
		}

		free(consoleoutputbuffer);
		consolecirclebufferplace = n;
	} else { // and if you are doing the init
		memset(newbuffer, 0, size * sizeof(char*));
		consolecirclebufferplace = 0;//just in case
		consolestringsprinted = 0;
	}

	consoleoutputbuffer = newbuffer;
	maxconsolebufferlines = size;
	console_displayneedsupdate = TRUE;
	return maxconsolebufferlines;
}
int console_init(void){ //should work for now
	cvar_register(&cvar_console_Height);
	tempprint = malloc(maxconsolebufferlinelength * sizeof(char));
	resizeConsoleBuffer(maxconsolebufferlines);
//	consoleVBO = createAndAddVBO("console", 2); //todo set type to something
//	consoleVAOid = consoleVBO->vaoid;
//	resizeConsoleBuffer(5); for testing resizing
	console_displayneedsupdate = TRUE;
	currentconsoletexttrackerflag = 0;
	return TRUE; // good enough for now
}
int console_printf(const char *format, ...){//very similar to printf...
	va_list arg;
	int done;


	if(!consoleoutputbuffer){ //no console, fall back to stdout printing
		va_start(arg, format);
		done = vfprintf (stdout, format, arg); //not likely to be a tempprint malloced, so not using it
		va_end(arg);
		return done;
	}

	if(!tempprint) return 0;// somehow we messed up

	//initialize string
	//slap string into buffer
	//move down into empty buffer spot
	int length;
	va_start(arg, format);
	done = vsnprintf(tempprint, maxconsolebufferlinelength, format, arg);
	va_end(arg);

	printf(tempprint); // possibly faster than having it re-interpret the format

	length = strlen(tempprint) + 2;
	consoleoutputbuffer[consolecirclebufferplace] = realloc(consoleoutputbuffer[consolecirclebufferplace], length * sizeof(char)); //reallocate that string in the buffer to only the size needed
	strncpy(consoleoutputbuffer[consolecirclebufferplace], tempprint, length * sizeof(char));

	consolecirclebufferplace++;
	consolecirclebufferplace = (consolecirclebufferplace % maxconsolebufferlines); // add one to the position
	if(consolestringsprinted < maxconsolebufferlines)consolestringsprinted++; //add one to how "full" the buffer is

	//maybe call a function to update vbos for the console or something


	console_displayneedsupdate = TRUE;
	return done;
}

/*NOTE
	when using this function you have to take into account the length of all of the inputs. That includes format.
	So, if you are doing console_nprintf( SOMESIZE, "yar har %s\n", string); with string being 200 characters long,
	you have to take into account of the length of "yar har \n" as well. (so final would be 210 or so characters long.
*/
int console_nprintf(const size_t size, const char *format, ...){//very similar to printf...
	va_list arg;
	int done;

	if(!consoleoutputbuffer){ //no console, fall back to stdout printing
		va_start(arg, format);
		done = vfprintf (stdout, format, arg); //not likely to be a tempprint malloced, so not using it
		va_end(arg);
		return done;
	}

	char * bigtempprint = malloc(size);
	if(!bigtempprint) return 0;// somehow we messed up

	//initialize string
	//slap string into buffer
	//move down into empty buffer spot
	va_start(arg, format);
	done = vsnprintf(bigtempprint, size, format, arg);
	va_end(arg);

	printf(bigtempprint); // possibly faster than having it re-interpret the format

	consoleoutputbuffer[consolecirclebufferplace] = realloc(consoleoutputbuffer[consolecirclebufferplace], size); //reallocate that string in the buffer to only the size needed
	strncpy(consoleoutputbuffer[consolecirclebufferplace], bigtempprint, size);
	free(bigtempprint);

	consolecirclebufferplace++;
	consolecirclebufferplace = (consolecirclebufferplace % maxconsolebufferlines); // add one to the position
	if(consolestringsprinted < maxconsolebufferlines)consolestringsprinted++; //add one to how "full" the buffer is

	//maybe call a function to update vbos for the console or something


	console_displayneedsupdate = TRUE;
	return done;
}
int console_printBackwards(void){
	printf("Console buffer backwards : \n");
	int n, p = consolecirclebufferplace;
	for(n = 0; n < consolestringsprinted; n++){
		p--;
		if(p < 0) p = maxconsolebufferlines-1;
		if(consoleoutputbuffer)printf(consoleoutputbuffer[p]);
	}
	return TRUE;
}
