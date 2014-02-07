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
#include "viewportmanager.h"
#include "entitymanager.h"

#include <tgmath.h>


//texturegroup_t * tcoil;
shaderprogram_t * staticmodel, * staticmodeltextured;
GLuint modelmat4, viewmat4, projectionmat4;
float degnumber;
	viewport_t cam;



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
//	staticmodeltextured = createAndAddShader("staticmodeltextured");
//	glUseProgram(staticmodel->id);
	modelmat4 = glGetUniformLocation(staticmodel->id, "modelMat");
	if(modelmat4<0) consolePrintf("cant find uniform!\n");
	viewmat4 = glGetUniformLocation(staticmodel->id, "viewMat");
	if(viewmat4<0) consolePrintf("cant find uniform!\n");
	projectionmat4 = glGetUniformLocation(staticmodel->id, "projectionMat");
	if(projectionmat4<0) consolePrintf("cant find uniform!\n");

//	createAndAddShader("console");
//	addTextureGroupToList(createAndLoadTextureGroup("coil"));
	createAndAddModel("teapot");
	createAndAddModel("dragon");
	createAndAddModel("coil");


	glEnable(GL_MULTISAMPLE);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST );
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST );
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glClearDepth(1.0);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glDisable(GL_FOG);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
//	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glViewport(0, 0, 800, 600);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(staticmodel->id);

	cam = createViewport("cam");

	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, tcoil->textures->id);
//	consolePrintf("texture %s id: %i\n", tcoil->name, tcoil->textures->id);


	return TRUE; // so far so good
}
int glDrawModel(model_t * model, matrix4x4_t * modworld){
	GLfloat out[16];
	Matrix4x4_ToArrayFloatGL(modworld, out);
	glUniformMatrix4fv(modelmat4, 1, GL_FALSE, out);
	glBindVertexArray(model->vbo->vaoid);
//	glDrawElements(GL_TRIANGLES, model->numfaces[1]*3, GL_UNSIGNED_INT, (void*)(model->numfaces[0]*3*sizeof(GLuint)));
	glDrawElements(GL_TRIANGLES, model->vbo->numfaces*3, GL_UNSIGNED_INT, 0);
	return model->vbo->numfaces;
}
int drawEntities(void){
	int i;
	int count = 0;
	for(i =0; i <= entityArrayLastTaken; i++){
		entity_t *e = &entitylist[i];
		if(e->type < 2)continue;
		if(!e->model)continue;
		//todo sort and whatnot
/*		if(e->texturegroup){
			bindTextureGroup(e->texturegroup);
//			glUseProgram(staticmodeltextured->id);
		} else {
			glUseProgram(staticmodel->id);
		}
*/
		glDrawModel(e->model, &e->mat);
		count++;
	}
	return count;
}
int glMainDraw(void){
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	degnumber = degnumber+0.1;
	if(degnumber>360.0) degnumber -=360.0;
	vec3_t pos = {0.0, 9.0, 15.0};
	vec3_t angle = {30.0, 0.0, 0.0};
	pos[0] = sin(degnumber *(-M_PI / 180.0))*15.0;
	pos[2] = cos(degnumber *(-M_PI / 180.0))*15.0;
	angle[1] = degnumber;
//	angle[2] = -90.0;
	recalcViewport(&cam, pos, angle, 90.0, 4.0/3.0, 1.0, 1000.0);

	GLfloat out[16];
	Matrix4x4_ToArrayFloatGL(&cam.view, out);
	glUniformMatrix4fv(viewmat4, 1, GL_FALSE, out);
	Matrix4x4_ToArrayFloatGL(&cam.projection, out);
	glUniformMatrix4fv(projectionmat4, 1, GL_FALSE, out);

	drawEntities();
	swapBuffers();
	return TRUE;
}
