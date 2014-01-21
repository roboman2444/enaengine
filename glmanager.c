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
#include "console.h"


int texturesOK;
int framebuffersOK;
int modelsOK;
int vboOK;
int shadersOK;
int particlesOK;

//todo move these to globals in their respective .hfiles

int glInit(void){
	GLenum glewError = glewInit();
	if(glewError != GLEW_OK){
		consolePrintf("ERROR with the glew: %s\n", glewGetErrorString(glewError));
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
	createAndLoadShader("console");

	createAndAddModel("teapot");


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	return TRUE; // so far so good
}
int glMainDraw(void){
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, 800, 600);
	glUseProgram(1);
	glDrawElements(GL_TRIANGLES, 6320*3, GL_UNSIGNED_INT, 0);
//	updateConsoleVBO();
//	glUseProgram(2);
//	glDrawArrays(GL_QUADS, 0, 40);

//	glDrawArrays(GL_POINTS, 0, 3644);
//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	swapBuffers();
	return TRUE;
}
