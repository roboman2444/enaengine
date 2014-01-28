//gloabl includes
#include <GL/glew.h>
#include <GL/gl.h>

//local includes
#include "globaldefs.h"
#include "matrixlib.h"
#include "viewportmanager.h"

int vpnumber = 0; //the first is an error one/screen
int viewportsOK = 0;
viewport_t *vplist;

int initViewportSystem(void){
	//todo have it figure out screen aspect for the default
			//	name	id	width	height	aspect	fov
	viewport_t screen = {"default"	,0 	,0	,0	,1	, 90};
	if(vplist) free(vplist);
	vplist = malloc(vpnumber * sizeof(viewport_t));
	if(!vplist) memset(vplist, 0 , vpnumber * sizeof(viewport_t));
	addViewportToList(screen);
	defaultViewport = &vplist[0];
	viewportsOK = TRUE;
	return TRUE; // todo error check
}
viewport_t *  addViewportToList(viewport_t vp){ //todo have this return a viewport pointa
	int current = vpnumber;
	vpnumber++;
	vplist = realloc(vplist, vpnumber * sizeof(viewport_t));
	vplist[current] = vp;
	//vplist[current].name = malloc(sizeof(*vp.name));
	//strcpy(vplist[current].name, vp.name);
	return &vplist[current];
}
/*
viewport_t createViewport (char * name){
//todo
}
*/
viewport_t * returnViewport(int id){
	if(id >= vpnumber) return &vplist[0];
	return &vplist[id];
}
viewport_t * findViewportByName(char * name){
	int i;
	for(i = 0; i<vpnumber; i++){
		if(!strcmp(name, vplist[i].name)) return &vplist[i];
	}
	return &vplist[0];
}

/*
int createViewport(char * name){
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
