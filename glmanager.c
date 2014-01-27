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
#include "particlemanager.h"
#include "console.h"

model_t * dragon;
shaderprogram_t * staticmodel;



int glInit(void){
	GLenum glewError = glewInit();
	if(glewError != GLEW_OK){
		consolePrintf("ERROR with the glew: %s\n", glewGetErrorString(glewError));
		return FALSE;
	}
	initShaderSystem();
	if(!shadersOK){
		//todo call some sort of shutdown of everything
		 return FALSE;
	}
	initTextureSystem();
	if(!texturesOK){
		//todo call some sort of shutdown of everything
		 return FALSE;
	}
	initFrameBufferSystem();
	if(!framebuffersOK){
		//todo call some sort of shutdown of everything
		 return FALSE;
	}
	initVBOSystem();
	if(!vboOK){
		return FALSE;
		//todo call some sort of shutdown of everything
	}

	initModelSystem();
	if(!modelsOK){
		//todo call some sort of shutdown of everything
		 return FALSE;
	}
	initParticleSystem(128);
	if(!particlesOK){
		//todo call some sort of shutdown of everything
		 return FALSE;
	}
	staticmodel = createAndAddShader("staticmodel");
//	createAndAddShader("console");
	dragon = createAndAddModel("dragon");


	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0);
	glClearColor(0, 0, 0, 0);
	glDisable(GL_FOG);
	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LESS);
//	glEnable(GL_CULL_FACE);
//	glCullFace(GL_BACK);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	return TRUE; // so far so good
}
int glMainDraw(void){
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, 800, 600);
	glUseProgram(staticmodel->id);
//	glUseProgram(3);

//	glBindVertexArray(dragon->vbo->vaoid);
//	glDrawElements(GL_TRIANGLES, dragon->vbo->numfaces*3, GL_UNSIGNED_INT, 0);
	sleep(1);

	glDrawElements(GL_TRIANGLES, 1000000*3, GL_UNSIGNED_INT, 0);

//	updateConsoleVBO();
//	glUseProgram(2);
//	glDrawArrays(GL_QUADS, 0, 40);

//	glDrawArrays(GL_POINTS, 0, 3644);
//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	swapBuffers();
	return TRUE;
}
