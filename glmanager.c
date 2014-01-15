#include <GL/glew.h>
#include <GL/gl.h>

#include "globaldefs.h"
#include "glmanager.h"
#include "shadermanager.h"
#include "texturemanager.h"
#include "framebuffermanager.h"
#include "vbomanager.h"
#include "modelmanager.h"
#include "sdlmanager.h"


int texturesOK;
int framebuffersOK;
int modelsOK;
int vboOK;
int shadersOK;

//todo move these to globals in their respective .hfiles

int glInit(void){
	GLenum glewError = glewInit();
	if(glewError != GLEW_OK){
		printf("ERROR with the glew: %s\n", glewGetErrorString(glewError));
		return FALSE;
	}
	shadersOK = initShaderSystem();
	if(!shadersOK){
		//todo call some sort of shutdown of everything
		 return FALSE;
	}
	texturesOK = initTextureSystem();
	if(!texturesOK){
		//todo call some sort of shutdown of everything
		 return FALSE;
	}
	framebuffersOK = initFrameBufferSystem();
	if(!framebuffersOK){
		//todo call some sort of shutdown of everything
		 return FALSE;
	}
	vboOK = initVBOSystem();
	if(!vboOK){
		return FALSE;
		//todo call some sort of shutdown of everything
	}
	modelsOK = initModelSystem();
	if(!modelsOK){
		//todo call some sort of shutdown of everything
		 return FALSE;
	}
	createAndLoadShader("staticmodel");

	createAndAddModel("teapot");

	return TRUE; // so far so good
}
int glMainDraw(void){
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 800, 600);
	glUseProgram(1);
//	glDrawElements(GL_TRIANGLES, 6320, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_POINTS, 0, 3644);
	swapBuffers();
	return TRUE;
}
