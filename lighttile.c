//global includes
#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>
//local includes
#include "globaldefs.h"
#include "matrixlib.h"
#include "mathlib.h"
#include "viewportmanager.h"
#include "lightmanager.h"
#include "renderqueue.h"
#include "shadermanager.h"
#include "texturemanager.h"
#include "framebuffermanager.h"
#include "modelmanager.h"
#include "vbomanager.h"
#include "glstates.h"
#include "console.h"
#include "glmanager.h"

#include "lighttile.h"

lighttilebuffer_t maintilebuff= {0};
int lighttile_ok = 0;

unsigned int lighttileshaderid;
unsigned int lighttilemodelid;


int lighttile_init(void){
	lighttileshaderid = shader_createAndAddRINT("tilelight");
	lighttilemodelid = model_findByNameRINT("qsquadtr");
	lighttile_ok = TRUE;
	//todo errorcheck
	return TRUE;
}

void addPLightToTile(vec3_t pos, float size, lighttile_t *t){
	t->plcount++;
	if(t->plcount > t->plsize*PERTILEMAXLIGHTS){
		t->plsize++;
		t->pllist = realloc(t->pllist, t->plsize * sizeof(tileUBOStruct_t));
		//mcount shoooould be 0 at this time
	}
	unsigned int mplace = (t->plcount-1)/PERTILEMAXLIGHTS;
	unsigned int mcount = (t->plcount-1)%PERTILEMAXLIGHTS;
	if(!mcount){
		//GLfloat tx = t->x;
		t->pllist[mplace].offset[0]=t->x;
		t->pllist[mplace].offset[1]=t->y;
	}
	pTileLightUBOStruct_t *plst = &t->pllist[mplace].lights[mcount];
	t->pllist[mplace].lcount = mcount+1;
	plst->pos[0] = pos[0];
	plst->pos[1] = pos[1];
	plst->pos[2] = pos[2];
	plst->size = size;
}

unsigned int lighttile_tileLights(lighttilebuffer_t *b, const viewport_t *v, const unsigned int width, const unsigned int height, const lightlistpoint_t l){
	unsigned int mycount = width * height;
//	unsigned int k, kmax = (mycount > b->count) ? b->count : mycount;
	//resize tilelist if needed
	if(mycount > b->count){
		unsigned int start = b->count;
		unsigned int size = mycount - start;
		b->list = realloc(b->list, mycount * sizeof(lighttile_t));
		b->count = mycount;
		memset(&b->list[start], 0, size * sizeof(lighttile_t));
	}
	lighttile_t *list = b->list;

	GLfloat xwidth = 2.0f/(GLfloat)width;
	GLfloat xheight = 2.0f/(GLfloat)height;
	unsigned int x, y;
	//reset tile lightcounts and fix x/y
	for(y = 0; y < height; y++){
		GLfloat passy = ((GLfloat)y *xheight)-1.0f;
		for(x = 0; x < width; x++){
			lighttile_t *t = &list[y * width + x];
			GLfloat passx = ((GLfloat)x *xwidth)-1.0f;
			t->plcount = 0;
			t->slcount = 0;
			t->x = passx;
			t->y = passy;
		}
	}
	if(!l.count) return FALSE;

	unsigned int retval = 0;
	//loop through lights, add to tiles
	unsigned int i;
//	for(i = 0; i < l.count; i++){
	for(i=0; i < 1; i++){
		light_t *mylight = l.list[i];
		//get light in viewspace
		vec3_t vspace;
		Matrix4x4_Transform(&v->view, mylight->pos, vspace);

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
		unsigned int loopmaxy = (int)(((maxy+1.0)/2.0) * height) + 1;
		if(loopmaxy > height) loopmaxy = height;
		unsigned int loopminx = (int)(((minx+1.0)/2.0) * width);
		if(loopminx < 0) loopminx = 0;
		unsigned int loopminy = (int)(((miny+1.0)/2.0) * height);
		if(loopminy < 0) loopminy = 0;
		//use bounds and rectangle insert into tiles
		unsigned int lx, ly;
//		for(ly = loopminy; ly < loopmaxy; ly++){
		for(ly = 0; ly < height; ly++){
//			for(lx = loopminx; lx < loopmaxx; lx++){
			for(lx = 0; lx < width; lx++){
				retval++; // i can move this outside of loop with some nice math
				addPLightToTile(vspace, mylight->scale,&list[ly*width + lx]);
			}
		}
	}
	return retval;
}
typedef struct renderTileCallbackData_s {
	GLuint shaderprogram;
	unsigned int shaderperm;
	unsigned int shaderid;
	shaderpermutation_t * perm;
	unsigned char numsamples;
	unsigned int modelid;
	unsigned int ubodataoffset;
//	unsigned int ubodataoffset2;
	viewport_t *v;
	tileUBOStruct_t data;
	GLfloat totalwidth;
	GLfloat totalheight;
} renderTileCallbackData_t;

void setupTileCallback(renderlistitem_t *ilist, unsigned int count){
	if(count > 1){
		tileUBOStruct_t ubodata[MAXINSTANCESIZE];
		unsigned int i = 0;
		while(i < count){
			renderTileCallbackData_t *d = ilist[i].data;
			unsigned int counter = 0;
			ubodata[0] = d[0].data;
			unsigned int max = count-i;
			if(max > MAXINSTANCESIZE) max = MAXINSTANCESIZE;
			for(counter = 1; counter < max; counter++){
				ubodata[counter] = d[counter].data;
			}
			int t = pushDataToUBOCache(counter * sizeof(tileUBOStruct_t), ubodata);
			d->ubodataoffset = t;
			ilist[i].counter = counter; // reset counter, likely wont be needed in this case;
			i+=counter;
		}
	} else if(count == 1){
		renderTileCallbackData_t *d = ilist->data;
		int t = pushDataToUBOCache(sizeof(tileUBOStruct_t), &d->data);
		d->ubodataoffset = t;
	} else {
		console_printf("ERROR: TILE SETUP CALLBACK WITH 0 AS COUNT!\n");
	}
}


void drawTileCallback(renderlistitem_t *ilist, unsigned int count){
	//printf("Got here!\n");
	renderTileCallbackData_t *d = ilist->data;
	shaderpermutation_t * perm = d->perm;
	//todo check if the viewport changed as well
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
		glUniform2f(perm->univec21, v->projection.m[0][0], v->projection.m[1][1]);
		float far = v->far;
		float near = v->near;
		glUniform2f(perm->uniscreentodepth, far/(far-near),far*near/(near-far));
//		glUniform2i(perm->univec20, d->totalwidth, d->totalheight);
		glUniform2f(perm->univec20, d->totalwidth, d->totalheight);
		unsigned char numsamples = d->numsamples;
		if(numsamples) glUniform1i(perm->uniint0, numsamples);
	}

	model_t *m = model_returnById(d->modelid);
	vbo_t *v = returnVBOById(m->vbo);

//	states_bindVertexArray(v->vaoid);
	unsigned int mysize = (count * sizeof(tileUBOStruct_t));
//	states_bindBufferRange(GL_UNIFORM_BUFFER, 0, renderqueueuboid, d->ubodataoffset, mysize);
	glstate_t s = {STATESENABLECULLFACE | STATESENABLEBLEND, GL_ONE, GL_ONE, GL_LESS, GL_BACK, GL_FALSE, GL_LESS, 0.0, v->vaoid, renderqueueuboid, GL_UNIFORM_BUFFER, 0, d->ubodataoffset, mysize, perm->id};
	states_setState(s);
	//states_cullFace(GL_FRONT);
	CHECKGLERROR
	glDrawElementsInstanced(GL_TRIANGLES, v->numfaces * 3, GL_UNSIGNED_INT, 0, count);




//heavy todo


}

//you need the same list here to make sure you have light data proper and all
unsigned int lighttile_addToRenderQueue(viewport_t *v, renderqueue_t *q, const unsigned int width, const unsigned int height){
	lightrenderout_t out = readyLightsForRender(v,50,0);
	if(!out.lin.count && !out.lout.count) return FALSE;

//	printf("got here! %i\n", out.lout.count);
	unsigned int retval = lighttile_tileLights(&maintilebuff, v, width, height, out.lin);
	if(!retval) return FALSE;
	lighttile_t *list = maintilebuff.list;
	unsigned int mysize = width * height;

	//todo
	//set up texture stuff for states
//	GLuint textureunitid[STATESTEXTUREUNITCOUNT];
//	framebuffer_t *of = returnFramebufferById(v->outfbid);
	shaderprogram_t *s = shader_returnById(lighttileshaderid);
	shaderpermutation_t *sp = shader_addPermutationToProgram(s, 0);
	renderlistitem_t r = {0};
	r.setup = setupTileCallback;
	r.draw = drawTileCallback;
	r.datasize = sizeof(renderTileCallbackData_t);
//	r.flags = 2|4; //copyable, instanceable
	r.flags = 2; //copyable



	GLfloat xwidth = 2.0f/width;
	GLfloat xheight = 2.0f/height;
	renderTileCallbackData_t d = {0};
	d.shaderid = lighttileshaderid;
	d.shaderperm = 0;
	d.perm = sp;
	d.modelid = lighttilemodelid;
	d.v = v;
	d.totalwidth = xwidth;
	d.totalheight = xheight;
	r.data = &d;
	unsigned int i;
	for(i = 0; i < mysize; i++){
		lighttile_t * ltile = &list[i];
		unsigned int plcount = ltile->plcount;
		unsigned int slcount = ltile->slcount;
		int j, k;
		for(k = j = 0; j < plcount; j+= PERTILEMAXLIGHTS, k++){
			d.data = ltile->pllist[k];
			addRenderlistitem(q,r);
		}
		for(k = j = 0; j < slcount; j+= PERTILEMAXLIGHTS, k++){
			d.data = ltile->sllist[k]; //todo probably gonna have a different "r"
			addRenderlistitem(q,r);
		}
	}
	if(out.lin.list) free(out.lin.list);
	if(out.lout.list) free(out.lout.list);

	return retval;
}



//REDOOOOO


//recieve light list
//allocate lighttilebuffer if needed
//get bbox of each light, add to tilebuffer
//loop through lighttilebuffer, add tiles to queue
//render tiles
