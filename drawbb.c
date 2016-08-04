//global includes
#include <GL/glew.h>
#include <GL/gl.h>
//local includes
#include "globaldefs.h"
#include "matrixlib.h"
#include "mathlib.h"
#include "viewportmanager.h"
#include "shadermanager.h"
#include "vbomanager.h"
#include "renderqueue.h"
#include "glstates.h"
#include "glmanager.h"
#include "console.h"

#include "drawbb.h"


int drawbb_ok = 0;
unsigned int drawbbshaderid;


extern vboseperate_t renderqueuevbo;

int drawbb_init(void){
	drawbbshaderid = shader_createAndAddRINT("wireframe");
	drawbb_ok = TRUE;
	return TRUE;
}
typedef struct wireframeUBOStruct_s {
	GLfloat mvp[16];
	GLfloat color[3];
} wireframeUBOStruct_t;
typedef struct renderBBCallbackData_s {
	GLuint shaderprogram;
	unsigned int ubodataoffset;
	unsigned int startf;
	vec_t bboxp[24];
	wireframeUBOStruct_t ubo;
} renderBBCallbackData_t;


GLuint bboxpfaces[24] = {0, 1, 0, 2, 1, 3, 2, 3,
			 4, 5, 4, 6, 5, 7, 6, 7,
			 0, 4, 1, 5, 2, 6, 3, 7};



void drawbb_drawCallback(renderlistitem_t *ilist, unsigned int count){
//	printf("draw %i %i\n", renderqueuevbo.vaoid, count);
	renderBBCallbackData_t *d = ilist->data;
	unsigned int mysize = count * sizeof(wireframeUBOStruct_t);
	glstate_t s = {STATESENABLEDEPTH|STATESENABLECULLFACE|STATESENABLEBLEND, GL_ONE, GL_ONE, GL_LESS, GL_BACK, GL_FALSE, GL_LESS, 0.0, renderqueuevbo.vaoid, 0, 0, 0, 0, 0, d->shaderprogram, 0, {0}, {0}, {renderqueueuboid, 0}, {d->ubodataoffset, 0}, {mysize, 0}};
//	glstate_t s = {STATESENABLEDEPTH|STATESENABLECULLFACE|STATESENABLEBLEND, GL_ONE, GL_ONE, GL_LESS, GL_BACK, GL_FALSE, GL_LESS, 0.0, renderqueuevbo.vaoid, 0, 0, 0, 0, 0, d->shaderprogram, 0, {0}, {0}, {renderqueueuboid, 0}, {d->ubodataoffset, 0}, {mysize, 0}};
	states_setState(s);

//	states_bindVertexArray(renderqueuevbo.vaoid);
//	states_bindVertexArray(renderqueuevbo.0);
//	states_useProgram(d->shaderprogram);
//	states_bindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	CHECKGLERROR
//	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, bboxpfaces);
//	glDrawElements(GL_LINES, 24 * count, GL_UNSIGNED_INT, bboxpfaces);
	glDrawElements(GL_LINES, 24 * count, GL_UNSIGNED_INT, (const void *) (d->startf * sizeof(GLuint)));
//	glDrawElements(GL_LINES, 24 * count, GL_UNSIGNED_INT, 0);
}
void drawbb_setupCallback(renderlistitem_t *ilist, unsigned int count){
//	printf("setup\n");
	if(count > 1){
		unsigned int i = 0;
		//shitty no-batching method
		while(i < count){
			renderBBCallbackData_t * d = ilist[i].data;
			int t = pushDataToVertCache(8, 24, bboxpfaces, d->bboxp, 0, 0, 0, 0, 0);
			if(t < 0) printf("BAAAD\n");
			d->startf = t;
			t = pushDataToUBOCache(sizeof(renderBBCallbackData_t), &d->ubo);
			if(t < 0) printf("BAAAD\n");
			d->ubodataoffset = t;
			ilist[i].counter = 1;
			i+= 1;
		}
	} else if (count == 1){
		renderBBCallbackData_t * d = ilist->data;
		int t = pushDataToVertCache(8, 24, bboxpfaces, d->bboxp, 0, 0, 0, 0, 0);
		if(t < 0) printf("BAAAD\n");
		d->startf = t;
		t = pushDataToUBOCache(sizeof(renderBBCallbackData_t), &d->ubo);
		if(t < 0) printf("BAAAD\n");
		d->ubodataoffset = t;
	} else {
		printf("fuckyou\n");
	}
}


unsigned int drawbb_addToRenderQueue(viewport_t *v, renderqueue_t *q, const vec_t *bboxp, float colr, float colg, float colb){
	renderlistitem_t r ={0};
	renderBBCallbackData_t d;
	shaderprogram_t *s = shader_returnById(drawbbshaderid);
	shaderpermutation_t *sp = shader_addPermutationToProgram(s, 0);
	d.shaderprogram = sp->id;
	d.ubodataoffset = 0;
	Matrix4x4_ToArrayFloatGL(&v->viewproj, d.ubo.mvp);
	d.ubo.color[0] = colr;
	d.ubo.color[1] = colg;
	d.ubo.color[2] = colb;
	memcpy(d.bboxp, bboxp, 24 * sizeof(vec_t));
	r.sort[0] = (d.shaderprogram >> 0) && 0xFF;
	r.sort[1] = (d.shaderprogram >> 8) && 0xFF;
	r.sort[2] = (d.shaderprogram >> 16) && 0xFF;
	r.sort[3] = (d.shaderprogram >> 24) && 0xFF;

	r.setup = drawbb_setupCallback;
	r.draw = drawbb_drawCallback;
	r.datasize = sizeof(renderBBCallbackData_t);
	r.flags = 2 | 4; // copy data. instanceable
//	r.flags = 2;// | 4; // copy data. instanceable
	r.data = &d;

	addRenderlistitem(q, r);

	return FALSE;
}
