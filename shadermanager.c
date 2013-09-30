//gloabl includes
#include <GL/glew.h>
#include <GL/gl.h>

//local includes
#include "globaldefs.h"
#include "shadermanager.h"

int programnumber = 0;
shaderprogram_t *programlist;

int initShaderSystem(void){
	if(programlist) free(programlist);
	programlist = malloc(programnumber * sizeof(shaderprogram_t));
	if(!programlist) memset(programlist, 0 , programnumber * sizeof(shaderprogram_t));
	return TRUE; // todo error check
}

int createAndLoadShader(char * name){
	int current = programnumber;
	programnumber++;
	programlist = realloc(programlist, (programnumber)*sizeof(shaderprogram_t));
	programlist[current].name = name;
	//todo
	return current;
}

int printProgramLogStatus(shaderprogram_t program){
	GLint blen = 0;
	glGetProgramiv(program.id, GL_INFO_LOG_LENGTH, &blen);
	if(blen > 1){
		GLchar *log = (GLchar *) malloc(blen);
		glGetProgramInfoLog(program.id, blen, 0, log);
		printf("program log: %s \n", log);
		free(log);
		return FALSE;
	}
	return TRUE;
}
//UNTESTED
int getProgramLogStatus(shaderprogram_t program, char ** output){
	GLint length = 0;
	glGetProgramiv(program.id, GL_INFO_LOG_LENGTH, &length);
	if(length > 1){
		*output = malloc(length);
		glGetProgramInfoLog(program.id, length, 0, *output);
	}
	return length;
}

