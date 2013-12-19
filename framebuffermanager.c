//gloabl includes
#include <GL/glew.h>
#include <GL/gl.h>

//local includes
#include "globaldefs.h"
#include "framebuffermanager.h"

int fbnumber = 0; //the first is an error one/screen
framebuffer_t *fblist;

int initFrameBufferSystem(void){
	//todo have it figure out screen aspect for the default
			//	name	id	width	height	aspect	fov	texid
	framebuffer_t screen = {"default"	,0 	,0	,0	,1	,0	,0	};
	if(fblist) free(fblist);
	fblist = malloc(fbnumber * sizeof(framebuffer_t));
	if(!fblist) memset(fblist, 0 , fbnumber * sizeof(framebuffer_t));
	addFrameBufferToList(screen);
	defaultFrameBuffer = &fblist[0];
	return TRUE; // todo error check
}
int addFrameBufferToList(framebuffer_t fb){
	int current = fbnumber;
	fbnumber++;
	fblist = realloc(fblist, fbnumber * sizeof(framebuffer_t));
	fblist[current] = fb;
	return current;
}
/*
framebuffer_t createFrameBuffer (char * name){
//todo
}
*/
framebuffer_t * returnFrameBuffer(int id){
	if(id >= fbnumber) return &fblist[0];
	return &fblist[id];
}
framebuffer_t * findFrameBufferByName(char * name){
	int i;
	for(i = 0; i<fbnumber; i++){
		if(!strcmp(name, fblist[i].name)) return &fblist[i];
	}
	return &fblist[0];
}

/*
int createFramebuffer(char * name){
	char * vertname = malloc(strlen(name) + 5);

	strcpy(vertname, name);strcat(vertname, ".vert");

	char * vertstring;
	int vertlength;

	loadFileString(vertname, &vertstring, &vertlength, 2);

	free(vertname);
	if(vertlength == 0){	//error
		free(vertstring);
		return FALSE;
	}
	char * fragname = malloc(strlen(name) + 5); //add on 5 extra characters for .frag
	strcpy(fragname,name); strcat(fragname, ".frag");
	char * fragstring;
	int fraglength;
	loadFileString(fragname, &fragstring, &fraglength, 2);
	free(fragname);
	if(fraglength == 0){	//error
		free(fragstring);
		return FALSE;
	}
	GLuint vertid = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragid = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertid, 1, (const GLchar**) &vertstring, &vertlength);
	glShaderSource(fragid, 1, (const GLchar**) &fragstring, &fraglength);
	free(vertstring); free(fragstring);
	//TODO errorcheck
	glCompileShader(vertid);
	glCompileShader(fragid);
	//TODO errorcheck
	//TODO errorcheck for compilation

	GLuint programid = glCreateProgram();
	//TODO errorcheck
	glAttachShader(programid, vertid);
	glAttachShader(programid, fragid);
	glLinkProgram(programid);
	//TODO errorcheck
	printProgramLogStatus(programid);
	int id = addProgramToList(name, programid, vertid, fragid);
	return id; //so far i am assuming that it works
}
*/
