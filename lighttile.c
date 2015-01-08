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

unsigned int lighttile_count = 0;
lighttile_t * lighttile_list;

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
	//resize tilelist if needed
	if(mycount > lighttile_count){
		lighttile_list = realloc(lighttile_list, mycount * sizeof(lighttile_t));
		memset(lighttile_list + (lighttile_count * sizeof(lighttile_t)), 0, (mycount - lighttile_count) * sizeof(lighttile_t));
		lighttile_count = mycount;
	}
	//loop through lights, add to tiles
	if(!l.count) return;

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
//			lighttile_t * start = &lighttile_list[ly * width];
			for(lx = loopminx; lx < loopmaxx; lx++){
//				addLightToTile(i, &start[lx]);
				addLightToTile(i, &lighttile_list[ly*width + lx]);
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
//	unsigned int x; //can probably change to something smaller, such as an unsigned char
//	unsigned int y;
	GLfloat x; //todo look into different methods for this
	GLfloat y;
//	unsigned int numx;
//	unsigned int numy; //carried out by univec2
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
	tileUBOStruct_t data;
//	unsigned int totalwidth;
//	unsigned int totalheight;
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
	unsigned int mysize = ((count * sizeof(tileUBOStruct_t)));
//	states_bindBufferRange(GL_UNIFORM_BUFFER, 0, renderqueueuboid, d->ubodataoffset, mysize);
	glstate_t s = {STATESENABLECULLFACE | STATESENABLEBLEND, GL_ONE, GL_ONE, GL_LESS, GL_BACK, GL_FALSE, GL_LESS, 0.0, v->vaoid, renderqueueuboid, GL_UNIFORM_BUFFER, 0, d->ubodataoffset, mysize, perm->id};
	states_setState(s);
	//states_cullFace(GL_FRONT);
	CHECKGLERROR
	glDrawElementsInstanced(GL_TRIANGLES, v->numfaces * 3, GL_UNSIGNED_INT, 0, count);




//heavy todo


}

//you need the same list here to make sure you have light data proper and all
int lighttile_addToRenderQueue(viewport_t *v, renderqueue_t *q, const unsigned int width, const unsigned int height){
	lightrenderout_t out = readyLightsForRender(v,50,0);
	if(!out.lin.count && !out.lout.count) return FALSE;

	lightlistpoint_t list = out.lin;
//	printf("got here! %i\n", out.lout.count);
	lighttile_tileLights(v, width, height, list);

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
	r.flags = 2; //copyable, instanceable



	unsigned int lx, ly;
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
	for(ly = 0; ly < height; ly++){
		for(lx = 0; lx < width; lx++){
			lighttile_t * ltile = &lighttile_list[(ly * width) + lx];
			unsigned int tcount = ltile->count;
			unsigned int i = 0;
			while(tcount > 0){
//				printf("got here!\n");
//unneeded		d.shaderprogram =
//				d.data.x = (lx*2)-width;
//				d.data.y = (ly*2)-height;
				d.data.x = (lx*xwidth)-1.0f;
				d.data.y = (ly*xheight)-1.0f;
				d.data.lcount = tcount > PERTILEMAXLIGHTS ? tcount : PERTILEMAXLIGHTS;

				int j;

				for(j = 0; j < PERTILEMAXLIGHTS && tcount > 0; j++, i++, tcount--){
					pLightUBOStruct_t * dl = &d.data.lights[j];
					light_t * ll = list.list[ltile->list[i]];
					dl->pos[0] = ll->pos[0];
					dl->pos[1] = ll->pos[1];
					dl->pos[2] = ll->pos[2];
					dl->size = ll->scale;
				}
				//copies, so i shouldnt need to re-write every time
				addRenderlistitem(q,r);

			}
		}
	}
	if(out.lin.list) free(out.lin.list);
	if(out.lout.list) free(out.lout.list);

	return TRUE;
}
