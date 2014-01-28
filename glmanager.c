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
#include "matrixlib.h"

model_t * dragon;
shaderprogram_t * staticmodel;
GLuint modelmat4;
int degnumber;



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
//	glUseProgram(staticmodel->id);
	modelmat4 = glGetUniformLocation(staticmodel->id, "modelMat");
	if(modelmat4<0) consolePrintf("cant find uniform!\n");
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
	glViewport(0, 0, 800, 600);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(staticmodel->id);

	return TRUE; // so far so good
}
int glDrawModel(model_t * model, matrix4x4_t * modworld){
	GLfloat out[16];
	Matrix4x4_ToArrayFloatGL(modworld, out);
	glUniformMatrix4fv(modelmat4, 1, GL_FALSE, out);
	glBindVertexArray(model->vbo->vaoid);
	glDrawElements(GL_TRIANGLES, model->vbo->numfaces*3, GL_UNSIGNED_INT, 0);
	return model->vbo->numfaces;
}
int glMainDraw(void){
	degnumber = degnumber+1 % 360;
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	matrix4x4_t mat;
	Matrix4x4_CreateFromQuakeEntity(&mat, 0.0, -0.5, 0.0, degnumber, 0.0, -30.0, 0.1);
	glDrawModel(dragon, &mat);
//	glBindVertexArray(dragon->vbo->vaoid);
//	glDrawElements(GL_TRIANGLES, dragon->vbo->numfaces*3, GL_UNSIGNED_INT, 0);


//	updateConsoleVBO();
//	glDrawArrays(GL_QUADS, 0, 40);

//	glDrawArrays(GL_POINTS, 0, 3644);
//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	swapBuffers();
	return TRUE;
}
