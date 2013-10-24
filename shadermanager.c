//gloabl includes
#include <GL/glew.h>
#include <GL/gl.h>

//local includes
#include "globaldefs.h"
#include "shadermanager.h"
#include "filemanager.h"

int programnumber = 0; //the first is an error one
shaderprogram_t *programlist;

int initShaderSystem(void){
	if(programlist) free(programlist);
	programlist = malloc(programnumber * sizeof(shaderprogram_t));
	if(!programlist) memset(programlist, 0 , programnumber * sizeof(shaderprogram_t));
	addProgramToList("default", 0, 0, 0);
	return TRUE; // todo error check
}

int addProgramToList(char *name, GLuint id, GLuint vertexid, GLuint fragmentid){
	int current = programnumber;
	programnumber++;
	programlist = realloc(programlist, (programnumber)*sizeof(shaderprogram_t));
	programlist[current].name = name;
	programlist[current].id = id;
	programlist[current].vertexid = vertexid;
	programlist[current].fragmentid = fragmentid;
	//todo
	return current;
}
shaderprogram_t returnShader(int id){
	if(id >= programnumber) return programlist[id];
	return programlist[id];
}
int findProgramByName(char * name){
	int i;
	for(i = 0; i<programnumber; i++){
		if(!strcmp(name, programlist[i].name)) return i;
	}
	return 0;
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
	glLinkProgram(programid);
	//TODO errorcheck
	printProgramLogStatus(programid);
	int id = addProgramToList(name, programid, vertid, fragid);
	printf("%d\n", id);
	return id; //so far i am assuming that it works
}

int printProgramLogStatus(int id){
	GLint blen = 0;
	glGetProgramiv(id, GL_INFO_LOG_LENGTH, &blen);
	if(blen > 1){
		GLchar *log = (GLchar *) malloc(blen);
		glGetProgramInfoLog(id, blen, 0, log);
		printf("program log: %s \n", log);
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

