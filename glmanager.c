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
#include "renderqueue.h"

#include <tgmath.h>


float degnumber;

viewport_t * currentvp;
shaderpermutation_t * currentsp;

viewport_t cam;

int glShutdown(void){
	return FALSE;
}



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
//	glEnable(GL_TEXTURE_2D);

	cam = createViewport("cam", 1);


	return TRUE; // so far so good
}
int glDrawModel(model_t * model, matrix4x4_t * modworld, matrix4x4_t * viewproj){
	vbo_t * tvbo = returnVBOById(model->vbo);
	if(!tvbo) return FALSE;
	matrix4x4_t outmat;
//	Matrix4x4_Concat(&outmat, modworld, viewproj);
	Matrix4x4_Concat(&outmat, viewproj, modworld);

	GLfloat out[16];
	Matrix4x4_ToArrayFloatGL(&outmat, out);
	glUniformMatrix4fv(currentsp->unimat40, 1, GL_FALSE, out);
//	glBindVertexArray(tvbo->vaoid);

	glDrawElements(GL_TRIANGLES, tvbo->numfaces*3, GL_UNSIGNED_INT, 0);
	return tvbo->numfaces;
}
int loadEntitiesIntoQueue(renderbatche_t * batch, viewport_t * v){
	int i;
	int count = 0;
//	int cullcount =0;
	for(i =0; i <= entityArrayLastTaken; i++){
		entity_t *e = &entitylist[i];
		if(e->type < 2)continue;
		if(!e->modelid)continue;


		//entity modelspace model bbox method (generated from model verts in modelmanager.c)
		model_t * mod = returnModelById(e->modelid);
		vec_t * bbox = mod->bbox;
		int j;
		vec3_t point;
		vec3_t testvec;
		for(j = 0; j < 8; j++){
			point[0] = bbox[(j&1)+0];
			point[1] = bbox[(j&2)+2];
			point[2] = bbox[(j&4)+4];
//			consolePrintf("point: %f %f %f\t", point[0], point[1], point[2]);
			Matrix4x4_Transform(&e->mat, point, testvec);
//			consolePrintf("point: %f %f %f\t", testvec[0], testvec[1], testvec[2]);

			if(testPointInFrustum(v, testvec)){
				addEntityToRenderbatche(e, batch);
				count++;
				j = 9;
			}
		}
//		consolePrintf("\n\n");

		//entity worldspace bbox method
/*
		vec3_t org;
		Matrix4x4_OriginFromMatrix(&e->mat, org);


		vec3_t testvec;
		testvec[0] = e->maxs[0] + org[0];
		testvec[1] = e->maxs[1] + org[1];
		testvec[2] = e->maxs[2] + org[2];
		if(testPointInFrustum(v, testvec)){
			addEntityToRenderbatche(e, batch);
			count++;
			continue;
		}
		testvec[0] = e->maxs[0] + org[0];
		testvec[1] = e->mins[1] + org[1];
		testvec[2] = e->maxs[2] + org[2];
		if(testPointInFrustum(v, testvec)){
			addEntityToRenderbatche(e, batch);
			count++;
			continue;
		}
		testvec[0] = e->maxs[0] + org[0];
		testvec[1] = e->mins[1] + org[1];
		testvec[2] = e->mins[2] + org[2];
		if(testPointInFrustum(v, testvec)){
			addEntityToRenderbatche(e, batch);
			count++;
			continue;
		}
		testvec[0] = e->maxs[0] + org[0];
		testvec[1] = e->maxs[1] + org[1];
		testvec[2] = e->mins[2] + org[2];
		if(testPointInFrustum(v, testvec)){
			addEntityToRenderbatche(e, batch);
			count++;
			continue;
		}
		testvec[0] = e->mins[0] + org[0];
		testvec[1] = e->maxs[1] + org[1];
		testvec[2] = e->maxs[2] + org[2];
		if(testPointInFrustum(v, testvec)){
			addEntityToRenderbatche(e, batch);
			count++;
			continue;
		}
		testvec[0] = e->mins[0] + org[0];
		testvec[1] = e->mins[1] + org[1];
		testvec[2] = e->maxs[2] + org[2];
		if(testPointInFrustum(v, testvec)){
			addEntityToRenderbatche(e, batch);
			count++;
			continue;
		}
		testvec[0] = e->mins[0] + org[0];
		testvec[1] = e->mins[1] + org[1];
		testvec[2] = e->mins[2] + org[2];
		if(testPointInFrustum(v, testvec)){
			addEntityToRenderbatche(e, batch);
			count++;
			continue;
		}
		testvec[0] = e->mins[0] + org[0];
		testvec[1] = e->maxs[1] + org[1];
		testvec[2] = e->mins[2] + org[2];
		if(testPointInFrustum(v, testvec)){
			addEntityToRenderbatche(e, batch);
			count++;
			continue;
		}
*/
	}

//	if(cullcount)consolePrintf("cullcount:%i\n", cullcount);
	return count;

}
int drawEntitiesM(modelbatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	if(!count || !batch->matlist) return FALSE;
	int i;
	//todo
	//stuff here
	model_t * m = returnModelById(batch->modelid);
	vbo_t * tvbo = returnVBOById(m->vbo);
	if(!tvbo) return FALSE;
	glBindVertexArray(tvbo->vaoid);
	for(i = 0; i < count; i++){
	//todo
		glDrawModel(m, &batch->matlist[i], &cam.viewproj);
	}
	return count;
}
int drawEntitiesT(texturebatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	if(!count || !batch->modelbatch) return FALSE;
	int i;
	//stuff here
	texturegroup_t * tex = returnTexturegroupById(batch->textureid);
	if(tex){
		bindTexturegroup(tex);
//		consolePrintf("error:%i\n", glGetError());
	}
	for(i = 0; i < count; i++){
		drawEntitiesM(&batch->modelbatch[i]);
	}
	return count;
}
int drawEntitiesS(shaderbatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	if(!count || !batch->texturebatch) return FALSE;
	int i;
	//stuff here
	shaderprogram_t * shader = returnShaderById(batch->shaderid);
//	if(!shader) return FALSE; it does this anyway when i ask for perm
//	shaderpermutation_t * perm = findShaderPermutation(shader, batch->shaderperm);
//	if(!perm) return FALSE;
//	if(!perm) perm = addPermutationToShader(shader, batch->shaderperm);

	shaderpermutation_t * perm = addPermutationToShader(shader, batch->shaderperm);
	if(!perm) return FALSE;
//	consolePrintf("count:%i\n",1);

	if(perm->compiled < 2) return FALSE;

	//
	currentsp = perm;

	bindShaderPerm(perm);
//	glUseProgram(perm->id);
//	glUniform1i(glGetUniformLocation(perm->id, "texture0"), 0);

	for(i = 0; i < count; i++){
		drawEntitiesT(&batch->texturebatch[i]);
	}

	return count;
}
int drawEntitiesR(renderbatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	if(!count || !batch->shaderbatch) return FALSE;
	int i;
	//stuff here
	for(i = 0; i < count; i++){
		drawEntitiesS(&batch->shaderbatch[i]);
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
//	angle[2] = -90.0;
	angle[1] = degnumber;

	recalcViewport(&cam, pos, angle, 90.0, 4.0/3.0, 1.0, 1000.0);
	renderbatche_t b;
	memset(&b, 0, sizeof(renderbatche_t));
	loadEntitiesIntoQueue(&b, &cam);
	drawEntitiesR(&b);
	cleanupRenderbatche(&b);
	swapBuffers();
	return TRUE;
}
