//gloabl includes
#include <GL/glew.h>
#include <GL/gl.h>

//local includes
#include "globaldefs.h"
#include "shadermanager.h"

int printFragShaderLogStatus(shader_t shader){
	GLint blen = 0;
	glGetShaderiv(shader.id, GL_INFO_LOG_LENGTH, &blen);
	if(blen > 1){
		GLchar *log = (GLchar *) malloc(blen);
		glGetShaderInfoLog(shader.id, blen, 0, log);
		printf("shader log: %s \n", log);
		free(log);
		return FALSE;
	}
	return TRUE;
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
int getShaderLogStatus(shader_t shader, char ** output){
	GLint length = 0;
	glGetShaderiv(shader.id, GL_INFO_LOG_LENGTH, &length);
	if(length > 1){
		*output = malloc(length);
		glGetShaderInfoLog(shader.id, length, 0, *output);
	}
	return length;

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

