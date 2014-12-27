//global includes
#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>
//local includes
#include "globaldefs.h"
#include "matrixlib.h"
#include "mathlib.h"
#include "viewportmanager.h"
#include "lightmanager.h"
#include "lighttile.h"
#include "renderqueue.h"
#include "shadermanager.h"
#include "texturemanager.h"
#include "framebuffermanager.h"
#include "modelmanager.h"
#include "vbomanager.h"
#include "glstates.h"
#include "console.h"
#include "glmanager.h"

unsigned int lighttile_count = 0;
lighttile_t * lighttile_list;

void addLightToTile(const unsigned int l, lighttile_t *t){
	unsigned int mycount = t->count;
	t->count++;
	if(t->count > t->size){
		t->list = realloc(t->list, t->count * sizeof(unsigned int));
		t->size = t->count;
	}
	t->list[mycount] = l;
}

void lighttile_tileLights(const viewport_t *v, const unsigned int width, const unsigned int height, const lightlistpoint_t l){
	unsigned int mycount = width * height;
	unsigned int k, kmax = (mycount > lighttile_count) ? lighttile_count : mycount;
	//reset
	for(k = 0; k < kmax; k++){
		lighttile_list[k].count = 0;
	}
	if(!l.count) return;
	//resize tilelist if needed
	if(mycount > lighttile_count){
		lighttile_list = realloc(lighttile_list, mycount * sizeof(lighttile_t));
		memset(lighttile_list + (lighttile_count * sizeof(lighttile_t)), 0, (mycount - lighttile_count) * sizeof(lighttile_t));
		lighttile_count = mycount;
	}
	//loop through lights, add to tiles
	unsigned int i;
	for(i = 0; i < l.count; i++){
		light_t *mylight = l.list[i];
		//transform light bboxp to screen, then get scissor rect
		float maxx = -1.0;
		float maxy = -1.0;
		float minx =  1.0;
		float miny =  1.0;
		vec3_t tvec;
		unsigned int j;
		for(j = 0; j < 24; j+=3){
			Matrix4x4_Transform(&v->viewproj, &mylight->bboxp[j], tvec);
			if(tvec[0] > maxx) maxx = tvec[0];
			if(tvec[0] < minx) minx = tvec[0];
			if(tvec[1] > maxy) maxy = tvec[1];
			if(tvec[1] < miny) miny = tvec[1];
		}
		//find bounds
		unsigned int loopmaxx = (int)(((maxx+1.0)/2.0) * width) + 1;
		if(loopmaxx > width) loopmaxx = width;
		unsigned int loopmaxy = (int)(((maxy+1.0)/2.0) * height)+ 1;
		if(loopmaxy > height) loopmaxy = height;
		unsigned int loopminx = (int)(((minx+1.0)/2.0) * width);
		if(loopminx < 0) loopminx = 0;
		unsigned int loopminy = (int)(((miny+1.0)/2.0) * height);
		if(loopminy < 0) loopminy = 0;
		//use bounds and rectangle insert into tiles
		unsigned int lx, ly;
		for(ly = loopminy; ly < loopmaxy; ly++){
			lighttile_t * start = &lighttile_list[ly * width];
			for(lx = loopminx; lx < loopmaxx; lx++){
				addLightToTile(i, &start[lx]);
			}
		}
	}
}
#define PERTILEMAXLIGHTS 8
typedef struct pLightUBOStruct_s {
	GLfloat pos[3];
	GLfloat size;
} pLightUBOStruct_t;
typedef struct tileUBOStruct_s {
	unsigned int x; //can probably change to something smaller, such as an unsigned char
	unsigned int y;
	unsigned int lcount;	//can probably make to an unsigned char
//	unsigned int lights[PERTILEMAXLIGHTS]; //can probably make to a 16 bit int/unsigned char
	pLightUBOStruct_t lights [PERTILEMAXLIGHTS];
} tileUBOStruct_t;
typedef struct renderTileCallbackData_s {
	GLuint shaderprogram;
	unsigned int shaderperm;
	unsigned int shaderid;
	shaderpermutation_t * perm;
	unsigned char numsamples;
	unsigned int modelid;
	unsigned int ubodataoffset;
	unsigned int ubodataoffset2;
	viewport_t *v;
	tileUBOStruct_t deta;
	unsigned int totalwidth;
	unsigned int totalheight;
} renderTileCallbackData_t;


void drawTileCallback(renderlistitem_t *ilist, unsigned int count){
	renderTileCallbackData_t *d = ilist->data;
	shaderpermutation_t * perm = d->perm;
	if(shader_bindPerm(perm) == 2){
	//TODODODODODO
		viewport_t *v = d->v;
		//also have to set some basic uniforms?
	//	framebuffer_t *df = returnFramebufferById(v->dfbid);
		framebuffer_t *of = returnFramebufferById(v->outfbid);

		GLfloat mout[16];
		Matrix4x4_ToArrayFloatGL(&v->viewproj, mout);
		glUniformMatrix4fv(perm->unimat40, 1, GL_FALSE, mout);
		Matrix4x4_ToArrayFloatGL(&v->view, mout);
		glUniformMatrix4fv(perm->unimat41, 1, GL_FALSE, mout);
		glUniform2f(perm->uniscreensizefix, 1.0/of->width, 1.0/of->height);
		float far = v->far;
		float near = v->near;
		glUniform2f(perm->uniscreentodepth, far/(far-near),far*near/(near-far));
		glUniform2f(perm->univec20, d->totalwidth, d->totalheight);
		unsigned char numsamples = d->numsamples;
		if(numsamples) glUniform1i(perm->uniint0, numsamples);
	}

	model_t *m = model_returnById(d->modelid);
	vbo_t *v = returnVBOById(m->vbo);

//	states_bindVertexArray(v->vaoid);
	unsigned int mysize = ((count * sizeof(tileUBOStruct_t)));
//	states_bindBufferRange(GL_UNIFORM_BUFFER, 0, renderqueueuboid, d->ubodataoffset, mysize);
	glstate_t s = {STATESENABLECULLFACE | STATESENABLEBLEND, GL_ONE, GL_ONE, GL_LESS, GL_FRONT, GL_FALSE, GL_LESS, 0.0, v->vaoid, renderqueueuboid, GL_UNIFORM_BUFFER, 0, d->ubodataoffset, mysize, perm->id};
	states_setState(s);
	//states_cullFace(GL_FRONT);
	CHECKGLERROR
	glDrawElementsInstanced(GL_TRIANGLES, v->numfaces * 3, GL_UNSIGNED_INT, 0, count);




//heavy todo


}

//you need the same list here to make sure you have light data proper and all
void lighttile_addToRenderQueue(const unsigned int width, const unsigned int height, const lightlistpoint_t list){
	//stupidly simple test... one tile covers entire screen
	//todo


	unsigned int lx, ly;
	for(ly = 0; ly < height; ly++){
//		lighttile_t * start = &lighttile_list[ly * width];
		for(lx = 0; lx < width;  lx++){
//			renderlistitem_t r;
			///heavy todo
		}
	}
}
