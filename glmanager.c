#include <GL/glew.h>
#include <GL/gl.h>

#include "globaldefs.h"
#include "glmanager.h"
#include "texturemanager.h"
#include "framebuffermanager.h"
#include "modelmanager.h"


int texturesOK;
int framebuffersOK;
int modelsOK;

GLuint vertexarrayobject;
//todo move these to globals in their respective .hfiles

int initVAO(GLuint *vaoid){
	glGenVertexArrays(1, vaoid);
	glBindVertexArray(*vaoid);
	return *vaoid;
}

int glInit(void){
	GLenum glewError = glewInit();
	if(glewError != GLEW_OK){
		printf("ERROR with the glew: %s\n", glewGetErrorString(glewError));
		return FALSE;
	}
	texturesOK = initTextureSystem();
	if(!texturesOK){
		//todo call some sort of shutdown of everything
		 return texturesOK;
	}
	framebuffersOK = initFrameBufferSystem();
	if(!framebuffersOK){
		//todo call some sort of shutdown of everything
		 return framebuffersOK;
	}
	initVAO(&vertexarrayobject);
	if(vertexarrayobject){
		return vertexarrayobject;
		//todo call some sort of shutdown of everything
	}

	modelsOK = initModelSystem();
	if(!modelsOK){
		//todo call some sort of shutdown of everything
		 return modelsOK;
	}
	return TRUE; // so far so good
}
