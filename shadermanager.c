//gloabl includes
#include <GL/glew.h>
#include <GL/gl.h>

//local includes
#include "globaldefs.h"
#include "shadermanager.h"
#include "filemanager.h"
#include "console.h"

int programnumber = 0; //the first is an error one
shaderprogram_t *programlist;

int initShaderSystem(void){
	shaderprogram_t none = {"default", 0, 0, 0};
	if(programlist) free(programlist);
	programlist = malloc(1 * sizeof(shaderprogram_t));
	if(!programlist) memset(programlist, 0 , 1 * sizeof(shaderprogram_t));
	addProgramToList(none);
	return TRUE; // todo error check
}
int addProgramToList(shaderprogram_t prog){
	int current = programnumber;
	programnumber++;
	programlist = realloc(programlist, programnumber*sizeof(shaderprogram_t));
	programlist[current].id = prog.id;
	programlist[current].vertexid = prog.vertexid;
	programlist[current].fragmentid = prog.fragmentid;
	programlist[current].name = malloc(sizeof(*prog.name));
	strcpy(programlist[current].name, prog.name);
	return current;
}
shaderprogram_t * returnShader(int id){
	if(id >= programnumber) return &programlist[0];
	return & programlist[id];
}
shaderprogram_t * findProgramByName(char * name){
	int i;
	for(i = 0; i<programnumber; i++){
		if(!strcmp(name, programlist[i].name)) return &programlist[i];
	}
	return &programlist[0]; // return first one
}

int createAndLoadShader(char * name){
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
	glBindFragDataLocation(programid, 0, "fragColor"); //todo move this
	glLinkProgram(programid);
	//TODO errorcheck
	printProgramLogStatus(programid);
	shaderprogram_t prog = {name, programid, vertid, fragid};
	int id = addProgramToList(prog);
	consolePrintf("shader %s has id %d\n", name, id);
	return id; //so far i am assuming that it works
}
int printProgramLogStatus(int id){
	GLint blen = 0;
	glGetProgramiv(id, GL_INFO_LOG_LENGTH, &blen);
	if(blen > 1){
		GLchar *log = (GLchar *) malloc(blen);
		glGetProgramInfoLog(id, blen, 0, log);
		consoleNPrintf(blen + 16, "program log: %s \n", log); //too much for the console
		free(log);
		return FALSE;
	}
	return TRUE;
}
//UNTESTED
int getProgramLogStatus(int id, char ** output){
	GLint length = 0;
	glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
	if(length > 1){
		*output = malloc(length);
		glGetProgramInfoLog(id, length, 0, *output);
	}
	return length;
}
GLint findShaderAttribPos(shaderprogram_t * shader, char * name){
	GLint id = glGetAttribLocation(shader->id, name);
	return id;
}
