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
#include "worldmanager.h"
#include "renderqueue.h"
#include "lightmanager.h"
#include "textmanager.h"

#include <tgmath.h>


float degnumber;

//state rendering stuff
viewport_t * currentvp;
shaderpermutation_t * currentsp;
char currentMatNeeds = 0;

unsigned long totalface, totalcount, totalvert;
int camid = 0;
int wireshaderid = 0; //todo redo
int lightshaderid = 0;
viewport_t * cam = 0;
int lightvbo = 0;
int textvbo = 0; //temporary
int textshaderid = 0; // temporary
//GLfloat fsquadpoints[12] = {-1.0, -1.0, 	1.0, -1.0, 	 1.0, 1.0,
//			    -1.0, -1.0, 	1.0,  1.0, 	-1.0, 1.0};
GLfloat fsquadpoints[16] = {-1.0, -1.0, 0.0, 0.0,   1.0, -1.0, 1.0, 0.0,   1.0, 1.0, 1.0, 1.0,   -1.0, 1.0, 0.0, 1.0};
GLuint rectangleindices[6] = { 0, 1, 2, 0, 2, 3};
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
	initFramebufferSystem();
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
	initLightSystem();
	if(!lightsOK){
		//todo call some sort of shutdown of everything
		return FALSE;
	}
	initViewportSystem();
	if(!viewportsOK){
		//todo call some sort of shutdown of everything
		return FALSE;
	}
	initTextSystem();
	if(!textOK){
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
//	glViewport(0, 0, 800, 600);
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	glEnable(GL_TEXTURE_2D);

	cam = createAndAddViewportRPOINT("cam", 1);
	camid = cam->myid;
	cam->outfbid = findFramebufferByNameRINT("screen");
	cam->dfbid = createAndAddFramebufferRINT("screend", 3);
	resizeViewport(cam, 800, 600);

//	wireshaderid = createAndAddShaderRINT("wireframe");
	lightshaderid = createAndAddShaderRINT("deferredlight");

	vbo_t * lvbo = createAndAddVBORPOINT("lights", 2);
	lightvbo = lvbo->myid;

	//temporary
	vbo_t * tvbo = createAndAddVBORPOINT("text", 2);
	textvbo = tvbo->myid;
	textshaderid = createAndAddShaderRINT("text");


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
	totalface += tvbo->numfaces;
	totalvert += tvbo->numverts;
	totalcount++;
	return tvbo->numfaces;
}
int loadLeafIntoQueue(worldleaf_t * l, renderbatche_t * batch, viewport_t *v){
	int num = l->numobjects;
	int mynum=0;
	worldobject_t * list = l->list;
	int i;
	for(i = 0; i < num; i++){
		if(testBBoxPInFrustum(v, list[i].bboxp)){
			addObjectToRenderbatche(&list[i], batch);
			mynum++;
		}
	}
	//todo cull these out
	worldleaf_t **children = l->children;
	for(i = 0; i < 4; i++){
		if(children[i] && testBBoxPInFrustum(v, children[i]->bboxp)){
			mynum+= loadLeafIntoQueue(children[i], batch, v);
		}
	}
	return mynum;
}
int loadWorldIntoQueue(renderbatche_t * batch, viewport_t *v){
	if(!worldroot ||  !worldNumObjects) return FALSE;
	return loadLeafIntoQueue(worldroot, batch, v);
}
int loadEntitiesIntoQueue(renderbatche_t * batch, viewport_t * v){
	int i;
	int count = 0;
	int cullcount = 0;
	for(i =0; i <= entityArrayLastTaken; i++){
		entity_t *e = &entitylist[i];
		if(e->type < 2)continue;
		if(!e->modelid)continue;

		//entity worldspace bboxp method
		//best way to cull atm

		if(testBBoxPInFrustum(v, e->bboxp)){
			count++;
			addEntityToRenderbatche(e, batch);
		} else {
			cullcount++;
		}

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
		glDrawModel(m, &batch->matlist[i], &cam->viewproj);
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
void glDrawFSQuad(void){
	glVertexPointer(2, GL_FLOAT, 0, fsquadpoints);
	glDrawArrays(GL_TRIANGLES, 0, 2);
}
GLuint tris[36] = {
			0, 2, 3,
			0, 3, 1,
			4, 5, 7,
			4, 7, 6,
			2, 0, 4,
			2, 4, 6,
			0, 1, 5,
			0, 5, 4,
			1, 3, 7,
			1, 7, 5,
			3, 2, 6,
			3, 6, 7
	};
int glDrawLights(viewport_t *v){
	framebuffer_t *df = returnFramebufferById(v->dfbid);
	framebuffer_t *of = returnFramebufferById(v->outfbid);
	vbo_t * lvbo = returnVBOById(lightvbo);
	if(!df || !of || !lvbo) return FALSE;
	int i, count = 0;
//	lightbatche_t outlights;
	GLuint *indices = 0;
	GLfloat *points = 0;
	for(i = 0; i <= lightArrayLastTaken; i++){
		light_t * l = &lightlist[i];
		if(!l->type) continue;
		//todo make into a special function that will return something else if the light intersects the nearplane
		//todo also make a special case for non point lights
		if(testSphereInFrustum(v, l->pos, l->scale)){
			//set indices up
			unsigned int j = 36 * count;
			int t;
			unsigned int bump = 8 * count;
			count++;
			indices = realloc(indices, 36 * count * sizeof(GLuint));
			for(t = 0; t < 36; t++, j++){
				indices[j] = tris[t] + bump;
			}
			//copy bboxp
			points = realloc(points, 24 * count * sizeof(GLfloat));

			memcpy(&points[24*(count-1)], l->bboxp, 24*sizeof(GLfloat)); // size of 1

			//do i really need a lightbatch? cant i just generate stuff here and then render?
//			addLightToLightbatche(l->myid, &outlights);
		}
	}
	if(!count) return FALSE;

	shaderprogram_t * shader = returnShaderById(lightshaderid);
	shaderpermutation_t * perm = addPermutationToShader(shader, 0);
	if(perm->compiled < 2) return FALSE;
	currentsp = perm;
	bindShaderPerm(perm);


	//todo can i do this more efficiently
	glBindVertexArray(lvbo->vaoid);
	glBindBuffer(GL_ARRAY_BUFFER, lvbo->vboid);
	glBufferData(GL_ARRAY_BUFFER, count * 24 * sizeof(GLfloat), points, GL_STATIC_DRAW); // change to stream?

	glEnableVertexAttribArray(POSATTRIBLOC);
	glVertexAttribPointer(POSATTRIBLOC, 3, GL_FLOAT, GL_FALSE, 3* sizeof(GLfloat), 0); // may not be needed every time

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lvbo->indicesid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * count * sizeof(GLuint), indices, GL_STATIC_DRAW);
	lvbo->numfaces = 12 * count;
	lvbo->numverts = 8 * count;



	glBindFramebuffer(GL_FRAMEBUFFER, of->id);
	glDepthMask(GL_FALSE);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT| GL_STENCIL_BUFFER_BIT);//todo set OF to use the same renderbuffer for depth as DF
//	glClearBufferfi(of->rb​, GLint drawBuffer​, GLfloat depth​, GLint stencil​);
	glViewport(0, 0, of->width, of->height);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, df->id0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, df->id1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, df->id2);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_ONE);



//	glDisable(GL_DEPTH_TEST);
//	glDisable(GL_CULL_FACE);
	GLfloat out[16];
	Matrix4x4_ToArrayFloatGL(&v->viewproj, out);
	glUniformMatrix4fv(currentsp->unimat40, 1, GL_FALSE, out);
	glUniform2f(currentsp->uniscreensizefix, 1.0/of->width, 1.0/of->height);
	glDrawElements(GL_TRIANGLES, count * 36, GL_UNSIGNED_INT, 0);
//	glDrawArrays(GL_POINTS, 0, count * 8);
//	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_CULL_FACE);


//may not be needed, but its a good sanity check
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);



	return count;
}
int glDrawViewport(viewport_t *v){
	framebuffer_t *df = returnFramebufferById(v->dfbid);
	framebuffer_t *of = returnFramebufferById(v->outfbid);
	if(!df || !of) return FALSE;
	glBindFramebuffer(GL_FRAMEBUFFER, df->id);
//	glBindFramebuffer(GL_FRAMEBUFFER, of->id);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT| GL_STENCIL_BUFFER_BIT);//todo set OF to use the same renderbuffer for depth as DF
	glViewport(0, 0, df->width, df->height);
//	glViewport(0, 0, of->width, of->height);

	renderbatche_t b;
	memset(&b, 0, sizeof(renderbatche_t));

	loadEntitiesIntoQueue(&b, v);
	loadWorldIntoQueue(&b, v);
	drawEntitiesR(&b);

	cleanupRenderbatche(&b);

	glDrawLights(v);
//	glBindFramebuffer(GL_FRAMEBUFFER, of->id);
//	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//	glDrawFSQuad();

	return TRUE;
}
int glMainDraw(void){
	totalface = 0;
	totalcount = 0;
	totalvert = 0;
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	degnumber = degnumber+0.1;
	if(degnumber>360.0) degnumber -=360.0;
	vec3_t pos = {0.0, 9.0, 15.0};
	vec3_t angle = {30.0, 0.0, 0.0};
	pos[0] = sin(degnumber *(-M_PI / 180.0))*15.0;
	pos[2] = cos(degnumber *(-M_PI / 180.0))*15.0;
//	angle[2] = -90.0;
	angle[1] = degnumber;
	cam = returnViewportById(camid);

	recalcViewport(cam, pos, angle, 90.0, cam->aspect, 1.0, 1000.0);
	glDrawViewport(cam);

//temporary
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	glViewport(0, 0, 800, 600);
	text_t * t = createAndAddTextRPOINT("Text Rendering Works!", "FreeMono.ttf");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, t->textureid);
	vbo_t * tvbo = returnVBOById(textvbo);
	shaderprogram_t * shader = returnShaderById(textshaderid);
	shaderpermutation_t * perm = addPermutationToShader(shader, 0);
	if(perm->compiled < 2) return FALSE;
	currentsp = perm;
	bindShaderPerm(perm);
//	GLfloat * blah = malloc(16 * sizeof(GLfloat));
//	memcpy(blah, fsquadpoints, 16 * sizeof(GLfloat));
//	GLfloat * bleh = malloc(6 * sizeof(GLuint));
//	memcpy(bleh, rectangleindices, 6 * sizeof(GLuint));
	glBindVertexArray(tvbo->vaoid);
	glBindBuffer(GL_ARRAY_BUFFER, tvbo->vboid);
	glEnableVertexAttribArray(POSATTRIBLOC);
	glVertexAttribPointer(POSATTRIBLOC, 2, GL_FLOAT, GL_FALSE, 4* sizeof(GLfloat), 0); // may not be needed every time
	glEnableVertexAttribArray(TCATTRIBLOC);
	glVertexAttribPointer(TCATTRIBLOC, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2*sizeof(GLfloat))); // may not be needed every time
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(GLfloat), fsquadpoints, GL_STATIC_DRAW); // change to stream?
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tvbo->indicesid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), rectangleindices, GL_STATIC_DRAW);
	tvbo->numfaces = 2;
	tvbo->numverts = 4;

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


//	free(bleh);
//	free(blah);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);





//	glDrawFSQuad();
	swapBuffers();
//	consolePrintf("Faces: %li Verts: %li Objects: %li\n", totalface, totalvert, totalcount);
	return TRUE;
}
int glResizeViewports(int width, int height){
	if(!viewportsOK) return FALSE;
	if(!width) width = 1;
	if(!height) height = 1;
	int count =0;
	count+=resizeViewport(findViewportByNameRPOINT("cam"), width, height);
	return count;
}
