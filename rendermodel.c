#include <GL/glew.h>
#include <GL/gl.h>

#include "globaldefs.h"


#include "matrixlib.h"
#include "renderqueue.h"
#include "shadermanager.h"
#include "texturemanager.h"
#include "framebuffermanager.h"
#include "modelmanager.h"
#include "vbomanager.h"
#include "glstates.h"
#include "glmanager.h"
#include "console.h"
#include "ubomanager.h"

#include "rendermodel.h"


/*
Depth only model rendering
all da bland stuff

*/
//todo

typedef struct modelDepthUBOStruct_s {
	GLfloat mvp[16];
//	GLfloat mv[16]; //unlikely that i need this
} modelDepthUBOStruct_t;
modelDepthUBOStruct_t * modelDepthUBOData;
unsigned int modelDepthMaxSize = 0;




void rendermodel_drawMDCallback(renderlistitem_t * ilist, unsigned int count){

	renderModelDepthCallbackData_t *d = ilist->data;
	model_t *m = model_returnById(d->modelid);
	vbo_t *v = returnVBOById(m->vbo);
	unsigned int mysize = (count * sizeof(modelDepthUBOStruct_t));
	//todo finish the state
	glstate_t s = {STATESENABLEDEPTH|STATESENABLECULLFACE, GL_ONE, GL_ONE, GL_LESS, GL_BACK, GL_TRUE, GL_LESS, 0.0, v->vaoid, 0, 0, 0, 0, 0, d->shaderprogram, 0, {0}, {0}, {renderqueueuboid, 0}, {d->ubodataoffset, 0}, {mysize, 0}};
//	glstate_t s = {STATESENABLEDEPTH|STATESENABLECULLFACE, GL_ONE, GL_ONE, GL_LESS, GL_BACK, GL_TRUE, GL_LESS, 0.0, v->vaoid, renderqueueuboid, GL_UNIFORM_BUFFER, 0, d->ubodataoffset, mysize, d->shaderprogram};
//	states_setState(s);
	states_setState(s);
	//temporary hack until mask bits are in states
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	CHECKGLERROR
	glDrawElementsInstanced(GL_TRIANGLES, v->numfaces * 3, GL_UNSIGNED_INT, 0, count);
	//temporary hack until mask bits are in states
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}
void rendermodel_setupMDCallback(renderlistitem_t * ilist, unsigned int count){
	if(count > 1){
		unsigned int i = 0;
		while(i < count){
			renderModelDepthCallbackData_t *d = ilist[i].data;

			unsigned int counter = 0;
			Matrix4x4_ToArrayFloatGL(&d->mvp, modelDepthUBOData->mvp);
//			Matrix4x4_ToArrayFloatGL(&d->mv,  modelDepthUBOData->mv);

			unsigned int max = count-i;
			if(max > modelDepthMaxSize) max = modelDepthMaxSize;
			unsigned int currentmodelid = d->modelid;
			unsigned int currentshaderprogram = d->shaderprogram;
			for(counter = 1; counter < max; counter++){
				renderModelCallbackData_t *dl = ilist[i+counter].data;
				if(currentmodelid != dl->modelid || currentshaderprogram != dl->shaderprogram) break;
				Matrix4x4_ToArrayFloatGL(&dl->mvp, modelDepthUBOData[counter].mvp);
			}
			int t = pushDataToUBOCache(counter * sizeof(modelDepthUBOStruct_t), modelDepthUBOData);
			if(t < 0) printf("BAAAD\n");
			d->ubodataoffset = t;

			ilist[i].counter = counter;//reset instance size to whats right
			i+=counter;
		}
	} else if (count == 1){
		renderModelDepthCallbackData_t *d = ilist->data;

		modelDepthUBOStruct_t ubodata;
		Matrix4x4_ToArrayFloatGL(&d->mvp, ubodata.mvp);
		int t = pushDataToUBOCache(sizeof(modelDepthUBOStruct_t), &ubodata);
		d->ubodataoffset = t;
	} else {
		console_printf("ERROR: MODEL DEPTH SETUP CALLBACK WITH 0 AS COUNT!\n");
	}
}




/*
Textured model rendering
all da fun stuff

*/


typedef struct modelUBOStruct_s {
	GLfloat mvp[16];
	GLfloat mv[16];
} modelUBOStruct_t;
modelUBOStruct_t * modelUBOData;
unsigned int modelMaxSize = 0;


void rendermodel_drawMCallback(renderlistitem_t * ilist, unsigned int count){
	renderModelCallbackData_t *d = ilist->data;
	model_t *m = model_returnById(d->modelid);
	vbo_t *v = returnVBOById(m->vbo);
	unsigned int mysize = (count * sizeof(modelUBOStruct_t));
//	glstate_t s = {STATESENABLEDEPTH|STATESENABLECULLFACE, GL_ONE, GL_ONE, GL_LESS, GL_BACK, GL_TRUE, GL_LESS, 0.0, v->vaoid, renderqueueuboid, GL_UNIFORM_BUFFER, 0, d->ubodataoffset, mysize, d->shaderprogram};
	glstate_t s = {STATESENABLEDEPTH|STATESENABLECULLFACE, GL_ONE, GL_ONE, GL_LESS, GL_BACK, GL_TRUE, GL_LESS, 0.0, v->vaoid, 0, 0, 0, 0, 0, d->shaderprogram, 0, {0}, {0}, {renderqueueuboid, 0}, {d->ubodataoffset, 0}, {mysize, 0}};
//	states_setState(s);
	texturegroup_t *t = returnTexturegroupById(d->texturegroupid);
	if(t){
		unsigned int total = t->num;
		unsigned int i;
		texture_t *texturespointer = t->textures;
		if(texturespointer){
			for(i = 0; i < total; i++){
				int type = texturespointer[i].type - 1;
				if(type > -1){
//					s.enabledtextures = s.enabledtextures | 1<<type;
					s.textureunitid[type] = texturespointer[i].id;
					s.textureunittarget[type] = GL_TEXTURE_2D;
				}
			}
		}
	}
	states_setState(s);
	CHECKGLERROR
	glDrawElementsInstanced(GL_TRIANGLES, v->numfaces * 3, GL_UNSIGNED_INT, 0, count);
}
void rendermodel_setupMCallback(renderlistitem_t * ilist, unsigned int count){
	if(count > 1){
		unsigned int i = 0;
		while(i < count){
			renderModelCallbackData_t *d = ilist[i].data;

			unsigned int counter = 0;
			Matrix4x4_ToArrayFloatGL(&d->mvp, modelUBOData->mvp);
			Matrix4x4_ToArrayFloatGL(&d->mv,  modelUBOData->mv);

			unsigned int max = count-i;
			if(max > modelMaxSize) max = modelMaxSize;
			unsigned int currentmodelid = d->modelid;
			unsigned int currentshaderprogram = d->shaderprogram;
			unsigned int currenttexturegroupid = d->texturegroupid;
			for(counter = 1; counter < max; counter++){
				renderModelCallbackData_t *dl = ilist[i+counter].data;
				if(currentmodelid != dl->modelid || currentshaderprogram != dl->shaderprogram || currenttexturegroupid != dl->texturegroupid) break;
				Matrix4x4_ToArrayFloatGL(&dl->mvp, modelUBOData[counter].mvp);
				Matrix4x4_ToArrayFloatGL(&dl->mv,  modelUBOData[counter].mv);
			}
			int t = pushDataToUBOCache(counter * sizeof(modelUBOStruct_t), modelUBOData);
			if(t < 0) printf("BAAAD\n");
			d->ubodataoffset = t;


			ilist[i].counter = counter;//reset instance size to whats right
			i+=counter;
		}
	} else if (count == 1){
		renderModelCallbackData_t *d = ilist->data;

		modelUBOStruct_t ubodata;
		Matrix4x4_ToArrayFloatGL(&d->mvp, ubodata.mvp);
		Matrix4x4_ToArrayFloatGL(&d->mv,  ubodata.mv);
		int t = pushDataToUBOCache(sizeof(modelUBOStruct_t), &ubodata);
		d->ubodataoffset = t;
	} else {
		console_printf("ERROR: MODEL SETUP CALLBACK WITH 0 AS COUNT!\n");
	}
}
/*
Alpha Textured model rendering
all da fun stuff
likely only gonna use forward

NOTES ON ALPHA LIGHTING
im not gonna be a scrub and use only ambient light (like darkplaces uses only lightmap lookup)... have to do it "multi-pass" or be smart with a forward+ style
I want pretty lit particles that can recieve shadows (and cast them, but not onto other particles... i have a cool solution for that that can also handle colors)
first render alpha blending ambient (or directional or just whatever first light you find). lighting calc is rgb, alpha of model is a... maybe use premultiplied... maybe not
for all the other lights effecting object, render addative. Maybe premult
*/

void rendermodel_drawMACallback(renderlistitem_t * ilist, unsigned int count){
	renderModelAlphaCallbackData_t *d = ilist->data;
	model_t *m = model_returnById(d->modelid);
	vbo_t *v = returnVBOById(m->vbo);
	unsigned int mysize = (count * sizeof(modelUBOStruct_t));
	glstate_t s = {STATESENABLEDEPTH|STATESENABLECULLFACE|STATESENABLEBLEND, d->blendsource, d->blenddest, GL_LESS, GL_BACK, GL_FALSE, GL_LESS, 0.0, v->vaoid, 0, 0, 0, 0, 0, d->shaderprogram, 0, {0}, {0}, {renderqueueuboid, 0}, {d->ubodataoffset, 0}, {mysize, 0}};
//	glstate_t s = {STATESENABLEDEPTH|STATESENABLECULLFACE|STATESENABLEBLEND, d->blendsource, d->blenddest, GL_LESS, GL_BACK, GL_FALSE, GL_LESS, 0.0, v->vaoid, renderqueueuboid, GL_UNIFORM_BUFFER, 0, d->ubodataoffset, mysize, d->shaderprogram};
//	states_setState(s);
	texturegroup_t *t = returnTexturegroupById(d->texturegroupid);
	if(t){
		unsigned int total = t->num;
		unsigned int i;
		texture_t *texturespointer = t->textures;
		if(texturespointer){
			for(i = 0; i < total; i++){
				int type = texturespointer[i].type - 1;
				if(type > -1){
//					s.enabledtextures = s.enabledtextures | 1<<type;
					s.textureunitid[type] = texturespointer[i].id;
					s.textureunittarget[type] = GL_TEXTURE_2D;
				}
			}
		}
	}
	states_setState(s);
	CHECKGLERROR
	glDrawElementsInstanced(GL_TRIANGLES, v->numfaces * 3, GL_UNSIGNED_INT, 0, count);
}
void rendermodel_setupMACallback(renderlistitem_t * ilist, unsigned int count){
	if(count > 1){
		unsigned int i = 0;
		while(i < count){
			renderModelAlphaCallbackData_t *d = ilist[i].data;

			unsigned int counter = 0;
			Matrix4x4_ToArrayFloatGL(&d->mvp, modelUBOData->mvp);
			Matrix4x4_ToArrayFloatGL(&d->mv,  modelUBOData->mv);

			unsigned int max = count-i;
			if(max > modelMaxSize) max = modelMaxSize;
			unsigned int currentmodelid = d->modelid;
			unsigned int currentshaderprogram = d->shaderprogram;
			unsigned int currenttexturegroupid = d->texturegroupid;
			unsigned int currentblends = d->blendsource;
			unsigned int currentblendd = d->blenddest;
			for(counter = 1; counter < max; counter++){
				renderModelAlphaCallbackData_t *dl = ilist[i+counter].data;
				if(currentmodelid != dl->modelid || currentshaderprogram != dl->shaderprogram || currenttexturegroupid != dl->texturegroupid ||
				currentblends != dl->blendsource || currentblendd != dl->blenddest) break;
				Matrix4x4_ToArrayFloatGL(&dl->mvp, modelUBOData[counter].mvp);
				Matrix4x4_ToArrayFloatGL(&dl->mv,  modelUBOData[counter].mv);
			}
			int t = pushDataToUBOCache(counter * sizeof(modelUBOStruct_t), modelUBOData);
			if(t < 0) printf("BAAAD\n");
			d->ubodataoffset = t;


			ilist[i].counter = counter;//reset instance size to whats right
			i+=counter;
		}
	} else if (count == 1){
		renderModelAlphaCallbackData_t *d = ilist->data;

		modelUBOStruct_t ubodata;
		Matrix4x4_ToArrayFloatGL(&d->mvp, ubodata.mvp);
		Matrix4x4_ToArrayFloatGL(&d->mv,  ubodata.mv);
		int t = pushDataToUBOCache(sizeof(modelUBOStruct_t), &ubodata);
		d->ubodataoffset = t;
	} else {
		console_printf("ERROR: MODEL ALPHA SETUP CALLBACK WITH 0 AS COUNT!\n");
	}
}

void rendermodel_init(void){
	modelMaxSize = ubo_maxsize / sizeof(modelUBOStruct_t);
	console_printf("rendermodel : Max model instance count is %i\n", modelMaxSize);
	modelUBOData = malloc(modelMaxSize * sizeof(modelUBOStruct_t));

	modelDepthMaxSize = ubo_maxsize / sizeof(modelDepthUBOStruct_t);
	console_printf("rendermodel : Max modeldepth instance count is %i\n", modelDepthMaxSize);
	modelDepthUBOData = malloc(modelDepthMaxSize * sizeof(modelDepthUBOStruct_t));

}
