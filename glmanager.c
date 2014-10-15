#include <GL/glew.h>
#include <GL/gl.h>

#include "globaldefs.h"
#include "glmanager.h"
#include "matrixlib.h"
#include "shadermanager.h"
#include "texturemanager.h"
#include "framebuffermanager.h"
#include "vbomanager.h"
#include "modelmanager.h"
#include "sdlmanager.h"
#include "particlemanager.h"
#include "console.h"
#include "viewportmanager.h"
#include "entitymanager.h"
#include "worldmanager.h"
#include "renderqueue.h"
#include "lightmanager.h"
#include "textmanager.h"
#include "ubomanager.h"
#include "glstates.h"
#include "animmanager.h"
#include <tgmath.h> //for sin and cos
#include "cvarmanager.h"

#define CHECKGLERROR {if (TRUE){if (FALSE) console_printf("CHECKGLERROR at %s:%d\n", __FILE__, __LINE__);errornumber = glGetError();if (errornumber) GL_PrintError(errornumber, __FILE__, __LINE__);}}

float degnumber;

//state rendering stuff
viewport_t * currentvp;
char currentMatNeeds = 0;

unsigned long totalface, totalcount, totalvert;
int camid = 0;
int wireshaderid = 0; //todo redo
int lightshaderid = 0;
viewport_t * cam = 0;
GLuint instancevbo = 0;
GLuint instancevbo2 = 0;
int textvbo = 0; //temporary
int textshaderid = 0; // temporary
int cubeModel = 0; // todo move this as well as the other primitives into modelmanager
unsigned int currentflags = 0;
//GLfloat fsquadpoints[12] = {-1.0, -1.0, 	1.0, -1.0, 	 1.0, 1.0,
//			    -1.0, -1.0, 	1.0,  1.0, 	-1.0, 1.0};
GLfloat fsquadpoints[16] = {-1.0, -1.0, 0.0, 0.0,   1.0, -1.0, 1.0, 0.0,   1.0, 1.0, 1.0, 1.0,   -1.0, 1.0, 0.0, 1.0};
GLuint rectangleindices[6] = { 0, 1, 2, 0, 2, 3};

int fsquadmodel = 0;

vbo_t * consoleVBO = 0; //temp

cvar_t cvar_gl_MSAA_Samples = {CVAR_SAVEABLE, "gl_MSAA_Samples", "How many samples to use with multisample AA", "0"};

//temp?
renderqueue_t forward;
renderqueue_t deferred;
int glShutdown(void){
	return FALSE;
}
int errornumber = 0;

void GL_PrintError(int errornumber, const char *filename, int linenumber){
	switch(errornumber){
#ifdef GL_INVALID_ENUM
	case GL_INVALID_ENUM:
		console_printf("GL_INVALID_ENUM at %s:%i\n", filename, linenumber);
		break;
#endif
#ifdef GL_INVALID_VALUE
	case GL_INVALID_VALUE:
		console_printf("GL_INVALID_VALUE at %s:%i\n", filename, linenumber);
		break;
#endif
#ifdef GL_INVALID_OPERATION
	case GL_INVALID_OPERATION:
		console_printf("GL_INVALID_OPERATION at %s:%i\n", filename, linenumber);
		break;
#endif
#ifdef GL_STACK_OVERFLOW
	case GL_STACK_OVERFLOW:
		console_printf("GL_STACK_OVERFLOW at %s:%i\n", filename, linenumber);
		break;
#endif
#ifdef GL_STACK_UNDERFLOW
	case GL_STACK_UNDERFLOW:
		console_printf("GL_STACK_UNDERFLOW at %s:%i\n", filename, linenumber);
		break;
#endif
#ifdef GL_OUT_OF_MEMORY
	case GL_OUT_OF_MEMORY:
		console_printf("GL_OUT_OF_MEMORY at %s:%i\n", filename, linenumber);
		break;
#endif
#ifdef GL_TABLE_TOO_LARGE
	case GL_TABLE_TOO_LARGE:
		console_printf("GL_TABLE_TOO_LARGE at %s:%i\n", filename, linenumber);
		break;
#endif
#ifdef GL_INVALID_FRAMEBUFFER_OPERATION
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		console_printf("GL_INVALID_FRAMEBUFFER_OPERATION at %s:%i\n", filename, linenumber);
		break;
#endif
	default:
		console_printf("GL UNKNOWN (%i) at %s:%i\n", errornumber, filename, linenumber);
		break;
	}
}


extern void drawModelSetMax(void);
int glInit(void){
	cvar_register(&cvar_gl_MSAA_Samples);
	cvar_pset(&cvar_gl_MSAA_Samples, "0");
//	console_printf("cvar id is %i\n", cvar_gl_MSAA_Samples.myid);
//	console_printf("cvar name is %s\n", cvar_gl_MSAA_Samples.name);
//	console_printf("cvar value is %s\n", cvar_gl_MSAA_Samples.valuestring);
	GLenum glewError = glewInit();
	if(glewError != GLEW_OK){
		console_printf("ERROR with the glew: %s\n", glewGetErrorString(glewError));
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
	initUBOSystem();
	if(!uboOK){
		return FALSE;
		//todo call some sort of shutdown of everything
	}
	anim_init();
	if(!anim_ok){
		//todo call some sort of shutdown of everything
		return FALSE;
	}
	initModelSystem();
	if(!modelsOK){
		//todo call some sort of shutdown of everything
		 return FALSE;
	}
/*
	initParticleSystem(128);
	if(!particlesOK){
		//todo call some sort of shutdown of everything
		 return FALSE;
	}
*/
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

	states_enableForce(GL_MULTISAMPLE);
	glClearDepth(1.0);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	states_disableForce(GL_FOG);
	states_enableForce(GL_DEPTH_TEST);
//	glDepthFunc(GL_LESS);
	states_depthFunc(GL_LESS);
	states_enableForce(GL_CULL_FACE);
	states_cullFace(GL_BACK);

	int maxSamples, maxIntSamples, maxColorTextureSamples, maxDepthTextureSamples;
	glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
	glGetIntegerv(GL_MAX_INTEGER_SAMPLES, &maxIntSamples);
	glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &maxColorTextureSamples);
	glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &maxDepthTextureSamples);
	maxMSAASamples = maxSamples;
	if(maxIntSamples < maxMSAASamples) maxMSAASamples = maxIntSamples;
	if(maxColorTextureSamples < maxMSAASamples) maxMSAASamples = maxColorTextureSamples;
	if(maxDepthTextureSamples < maxMSAASamples) maxMSAASamples = maxDepthTextureSamples;
	console_printf("Max multisample samples: %i\n", maxMSAASamples);


	cam = createAndAddViewportRPOINT("cam", 1);
	camid = cam->myid;
	cam->outfbid = findFramebufferByNameRINT("screen");

	unsigned char dflags[] = {3, 7, 3};
//	unsigned char drb = FRAMEBUFFERRBFLAGSDEPTH| 2;
	unsigned char drb = FRAMEBUFFERRBFLAGSDEPTH;
	unsigned char dcount = 3;
	cam->dfbid = createAndAddFramebufferRINT("screend", dcount, drb, dflags);
	resizeViewport(cam, 800, 600);
	screenWidth = 800;
	screenHeight = 600;

	lightshaderid = createAndAddShaderRINT("deferredlight");
	glGenBuffers(1, &instancevbo);
	glGenBuffers(1, &instancevbo2);

	//temporary
	vbo_t * tvbo = createAndAddVBORPOINT("text", 2);
	textvbo = tvbo->myid;
	textshaderid = createAndAddShaderRINT("text");
	cubeModel = findModelByNameRINT("cube");
	fsquadmodel = findModelByNameRINT("fsquad");

	readyRenderQueueBuffers();

	drawModelSetMax();


	return TRUE; // so far so good
}
//todo move this junk
typedef struct renderModelCallbackData_s {
	unsigned int modelid;
	unsigned int shaderid; //do i need this?
	GLuint shaderprogram;
	unsigned int shaderperm; //todo do i need these, or can i change to a pointer?
	unsigned int texturegroupid;
	unsigned int ubodataoffset;
	matrix4x4_t mvp;
	matrix4x4_t mv;
} renderModelCallbackData_t;
typedef struct modelUBOStruct_s {
	GLfloat mvp[16];
	GLfloat mv[16];
} modelUBOStruct_t;
modelUBOStruct_t * modelUBOData;
unsigned int modelMaxSize = 0;

void drawModelSetMax(void){
	modelMaxSize = maxUBOSize / sizeof(modelUBOStruct_t);
	console_printf("Max model instance count is %i\n", modelMaxSize);
	modelUBOData = malloc(modelMaxSize * sizeof(modelUBOStruct_t));
}

void drawModelCallback(renderlistitem_t * ilist, unsigned int count){

	renderModelCallbackData_t *d = ilist->data;
	model_t *m = returnModelById(d->modelid);
	vbo_t *v = returnVBOById(m->vbo);
	unsigned int mysize = (count * sizeof(modelUBOStruct_t));
	glstate_t s = {STATESENABLEDEPTH|STATESENABLECULLFACE, GL_ONE, GL_ONE, GL_LESS, GL_BACK, GL_TRUE, GL_LESS, 0.0, v->vaoid, renderqueueuboid, GL_UNIFORM_BUFFER, 0, d->ubodataoffset, mysize, d->shaderprogram};
//	states_setState(s);
//	CHECKGLERROR
	CHECKGLERROR
	texturegroup_t *t = returnTexturegroupById(d->texturegroupid);
	if(t){
		unsigned int total = t->num;
		unsigned int i;
		texture_t *texturespointer = t->textures;
		if(texturespointer){
			for(i = 0; i < total; i++){
				int type = texturespointer[i].type - 1;
				if(type > -1){
					s.enabledtextures = s.enabledtextures | 1<<type;
					s.textureunitid[type] = texturespointer[i].id;
					s.textureunittarget[type] = GL_TEXTURE_2D;
				}
			}
		}
	}
//	bindTexturegroup(t);
	states_setState(s);
	CHECKGLERROR

	glDrawElementsInstanced(GL_TRIANGLES, v->numfaces * 3, GL_UNSIGNED_INT, 0, count);
	CHECKGLERROR
//	printf("Rendered %i\n", count);
}
void setupModelCallback(renderlistitem_t * ilist, unsigned int count){

	if(count > 1){
		//TODO alloc to max size that it can be, slow, but i may have a resizeablebuffer or a fixed size (MAXINSTANCECOUNT)
//		modelUBOStruct_t * ubodata = malloc(count * sizeof(modelUBOStruct_t));
//		modelUBOStruct_t ubodata[MAXINSTANCESIZE]; //TODO figure out per=callback max object sizes
	//todo get max per shader ubodata max size
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
//		free(ubodata);
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
//	console_printf("Setup %i\n", count);
}

void addObjectToRenderqueue(const worldobject_t *o, renderqueue_t * q, const viewport_t * v){
	renderlistitem_t r;
	unsigned int shaderperm = o->shaderperm;
	unsigned int shaderid = o->shaderid;
	unsigned int modelid = o->modelid;
	unsigned int texturegroupid = o->texturegroupid;
	renderModelCallbackData_t d;
	d.shaderperm = shaderperm;
	d.shaderid = shaderid;
	shaderprogram_t *s = returnShaderById(shaderid);
//	shaderpermutation_t * sp = findShaderPermutation(s, d->shaderperm);
	shaderpermutation_t * sp = addPermutationToShader(s, shaderperm);
	d.shaderprogram = sp->id;

	d.modelid = modelid;
	d.texturegroupid = texturegroupid;
	d.ubodataoffset = 0;
	Matrix4x4_Concat(&d.mvp, &v->viewproj, &o->mat);
	Matrix4x4_Concat(&d.mv, &v->view, &o->mat);
	r.sort[0] = (d.shaderprogram >> 0) & 0xFF;
	r.sort[1] = (d.shaderprogram >> 8) & 0xFF;
	r.sort[2] = (d.shaderprogram >> 16) & 0xFF;
	r.sort[3] = (d.shaderprogram >> 24) & 0xFF;
	r.sort[4] = (modelid >> 0) & 0xFF;
	r.sort[5] = (modelid >> 8) & 0xFF;
	r.sort[6] = (texturegroupid >> 0) & 0xFF;
	r.sort[7] = (texturegroupid >> 8) & 0xFF;
	r.sort[8] = 0; //could be distance data in here...
	r.sort[9] = 0;
	r.setup = setupModelCallback;
	r.draw = drawModelCallback;

	r.datasize = sizeof(renderModelCallbackData_t);
	r.flags = 2 | 4; //copyable, instanceable
//	r.flags = 2;
	r.data = &d;



	addRenderlistitem(q, r);
//	console_printf("ADDED\n");
}

int addAllChildrenLeafIntoQueues(worldleaf_t *l, renderqueue_t * forwardqueue, renderqueue_t * deferredqueue, viewport_t *v){
	int num = l->numobjects;
	worldobject_t * list = l->list;
	int mynum = 0;
	int i;
	for(i = 0; i < num; i++){
		if(list[i].flags & DEFERREDFLAG)
			addObjectToRenderqueue(&list[i], deferredqueue, v);
		if(list[i].flags & FORWARDFLAG)
			addObjectToRenderqueue(&list[i], forwardqueue, v);
		mynum++;
	}

	worldleaf_t **children = l->children;

	int j;
	for(j = 0; j < 4; j++){
		if(children[j]){
			mynum+= addAllChildrenLeafIntoQueues(children[j], forwardqueue, deferredqueue, v);
		}
	}
	return mynum;
}
int loadLeafIntoQueues(worldleaf_t * l, renderqueue_t * forwardqueue, renderqueue_t * deferredqueue, viewport_t *v){
	int num = l->numobjects;
	int mynum=0;
	worldobject_t * list = l->list;
	int i;
	for(i = 0; i < num; i++){
		if(testBBoxPInFrustum(v, list[i].bboxp)){
			if(list[i].flags & DEFERREDFLAG)
				addObjectToRenderqueue(&list[i], deferredqueue, v);
			if(list[i].flags & FORWARDFLAG)
				addObjectToRenderqueue(&list[i], forwardqueue, v);
			mynum++;
		}
	}
	//todo cull these out
	worldleaf_t **children = l->children;
/*
	for(i = 0; i < 4; i++){
		if(children[i] && testBBoxPInFrustum(v, children[i]->bboxp)){
			mynum+= loadLeafIntoQueues(children[i], forwardbatch, deferredbatch, v);
		}
	}
*/
	int j;
	for(j = 0; j < 4; j++){
		i = v->dir[j];

/*
		if(children[i]){
			int result = testBBoxPInFrustumCheckWhole(v, children[i]->bboxp);
			if(result == 1){
				mynum+= loadLeafIntoQueues(children[i], forwardbatch, deferredbatch, v);
			} else if(result ==2){
				addAllChildrenLeafIntoQueues(children[i], forwardbatch, deferredbatch);
			}
		}
*/

		if(children[i] && testBBoxPInFrustum(v, children[i]->bboxp)){
			mynum+= loadLeafIntoQueues(children[i], forwardqueue, deferredqueue, v);
		}

	}

	return mynum;
}
int loadWorldIntoQueues(renderqueue_t * forwardqueue, renderqueue_t * deferredqueue, viewport_t *v){
	if(!worldroot ||  !worldNumObjects) return FALSE;
	return loadLeafIntoQueues(worldroot, forwardqueue, deferredqueue, v);
}
int loadLeafIntoQueue(worldleaf_t * l, renderqueue_t * queue, viewport_t *v){
	int num = l->numobjects;
	int mynum=0;
	worldobject_t * list = l->list;
	int i;
	for(i = 0; i < num; i++){
		if(testBBoxPInFrustum(v, list[i].bboxp)){
			addObjectToRenderqueue(&list[i], queue, v);
			mynum++;
		}
	}
	//todo cull these out
	worldleaf_t **children = l->children;
	/*
	for(i = 0; i < 4; i++){
		if(children[i] && testBBoxPInFrustum(v, children[i]->bboxp)){
			mynum+= loadLeafIntoQueue(children[i], queue, v);
		}
	}
	*/
	int j;
	for(j = 0; j < 4; j++){
		i = v->dir[j];
		if(children[i] && testBBoxPInFrustum(v, children[i]->bboxp)){
			mynum+= loadLeafIntoQueue(children[i], queue, v);
		}
	}

	return mynum;
}
int loadWorldIntoQueue(renderqueue_t * queue, viewport_t *v){
	if(!worldroot ||  !worldNumObjects) return FALSE;
	return loadLeafIntoQueue(worldroot, queue, v);
}
void addEntityToRenderqueue(const entity_t *e, renderqueue_t * q, const viewport_t * v){
	renderlistitem_t r;
	unsigned int shaderperm = e->shaderperm;
	unsigned int shaderid = e->shaderid;
	unsigned int modelid = e->modelid;
	unsigned int texturegroupid = e->texturegroupid;
	renderModelCallbackData_t d;
	d.shaderperm = shaderperm;
	d.shaderid = shaderid;
	d.modelid = modelid;
	d.texturegroupid = texturegroupid;
	d.ubodataoffset = 0;
	shaderprogram_t *s = returnShaderById(shaderid);
//	shaderpermutation_t * sp = findShaderPermutation(s, d->shaderperm);
//	printf("%i, %i, %i\n", shaderperm, shaderid, s);
	shaderpermutation_t * sp = addPermutationToShader(s, shaderperm);
	d.shaderprogram = sp->id;

	Matrix4x4_Concat(&d.mvp, &v->viewproj, &e->mat);
	Matrix4x4_Concat(&d.mv, &v->view, &e->mat);
	r.sort[0] = (d.shaderprogram >> 0) & 0xFF;
	r.sort[1] = (d.shaderprogram >> 8) & 0xFF;
	r.sort[2] = (d.shaderprogram >> 16) & 0xFF;
	r.sort[3] = (d.shaderprogram >> 24) & 0xFF;
	r.sort[4] = (modelid >> 0) & 0xFF;
	r.sort[5] = (modelid >> 8) & 0xFF;
	r.sort[6] = (texturegroupid >> 0) & 0xFF;
	r.sort[7] = (texturegroupid >> 8) & 0xFF;
	r.sort[8] = 0;
	r.sort[9] = 0;
	r.setup = setupModelCallback;
	r.draw = drawModelCallback;


	r.datasize = sizeof(renderModelCallbackData_t);
	r.flags = 2 | 4; //copyable, freeable
//	r.flags = 1; //freeable
//	r.data = malloc(r.datasize);
//	memcpy(r.data, &d, r.datasize);
	r.data = &d;

	addRenderlistitem(q, r);
//	console_printf("ADDED\n");
}
int loadEntitiesIntoQueue(renderqueue_t * queue, viewport_t * v){
	int i;
	int count = 0;
	int cullcount = 0;
	for(i =0; i <= entity_arraylasttaken; i++){
		entity_t *e = &entity_list[i];
		if(e->type < 2)continue;
		if(!e->modelid)continue;

		//entity worldspace bboxp method
		//best way to cull atm
		if(testBBoxPInFrustum(v, e->bboxp)){
			count++;
			addEntityToRenderqueue(e, queue, v);
		} else {
			cullcount++;
		}
	}
	return count;
}
int loadEntitiesIntoQueues(renderqueue_t * forwardqueue, renderqueue_t * deferredqueue, viewport_t * v){
	int i;
	int count = 0;
	int cullcount = 0;
	for(i =0; i <= entity_arraylasttaken; i++){
		entity_t *e = &entity_list[i];
		if(e->type < 2)continue;
		if(!e->modelid)continue;

		//entity worldspace bboxp method
		//best way to cull atm
		if(testBBoxPInFrustum(v, e->bboxp)){
			count++;
			if(!(e->flags & FORWARDFLAG)) //todo check up on if the ! is supposed to be here..
				addEntityToRenderqueue(e, forwardqueue, v);
			if(!(e->flags & DEFERREDFLAG))
				addEntityToRenderqueue(e, deferredqueue, v);
		} else {
			cullcount++;
		}
	}
	return count;
}
int loadEntitiesIntoQueueForward(renderqueue_t * queue, viewport_t * v){
	int i;
	int count = 0;
	int cullcount = 0;
	for(i =0; i <= entity_arraylasttaken; i++){
		entity_t *e = &entity_list[i];
		if(e->type < 2)continue;
		if(!(e->flags & FORWARDFLAG))continue;
		if(!e->modelid)continue;

		//entity worldspace bboxp method
		//best way to cull atm
		if(testBBoxPInFrustum(v, e->bboxp)){
			count++;
			addEntityToRenderqueue(e, queue, v);
		} else {
			cullcount++;
		}
	}
	return count;
}
int loadEntitiesIntoQueueDeferred(renderqueue_t * queue, viewport_t * v){
	int i;
	int count = 0;
	int cullcount = 0;
	for(i =0; i <= entity_arraylasttaken; i++){
		entity_t *e = &entity_list[i];
		if(e->type < 2)continue;
		if(!(e->flags & DEFERREDFLAG))continue; //test if its "deferred" flag is set
		if(!e->modelid)continue;

		//entity worldspace bboxp method
		//best way to cull atm
		if(testBBoxPInFrustum(v, e->bboxp)){
			count++;
			addEntityToRenderqueue(e, queue, v);
		} else {
			cullcount++;
		}
	}
	return count;
}
//deprecated
void glDrawFSQuad(void){
	model_t * m = returnModelById(fsquadmodel);
	vbo_t * tvbo = returnVBOById(m->vbo);
	states_bindVertexArray(tvbo->vaoid);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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


typedef struct pLightPUBOStruct_s {
	GLfloat pos[3];
	GLfloat size;
} pLightUBOStruct_t;

typedef struct renderPLightCallbackData_s {
	//todo?
	GLuint shaderprogram;
	unsigned int shaderid;
	unsigned int shaderperm;
	shaderpermutation_t * perm;
	unsigned char numsamples;
	unsigned int modelid;
	unsigned int ubodataoffset;
	viewport_t *v;
	pLightUBOStruct_t light;
} renderPLightCallbackData_t;


void drawPLightOCallback(renderlistitem_t * ilist, unsigned int count){

	states_disable(GL_DEPTH_TEST);
	states_cullFace(GL_FRONT);

	renderPLightCallbackData_t *d = ilist->data;
	states_blendFunc(GL_ONE, GL_ONE);
	states_cullFace(GL_BACK);
	shaderpermutation_t * perm = d->perm;
	if(bindShaderPerm(perm) == 2){
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
		unsigned char numsamples = d->numsamples;
		if(numsamples) glUniform1i(perm->uniint0, numsamples);
	}
	model_t *m = returnModelById(d->modelid);
	vbo_t *v = returnVBOById(m->vbo);
	states_bindVertexArray(v->vaoid);
	unsigned int mysize = ((count * sizeof(pLightUBOStruct_t)));
	states_bindBufferRange(GL_UNIFORM_BUFFER, 0, renderqueueuboid, d->ubodataoffset, mysize);
	glDrawElementsInstanced(GL_TRIANGLES, v->numfaces * 3, GL_UNSIGNED_INT, 0, count);

		states_cullFace(GL_BACK);
		states_enable(GL_DEPTH_TEST);
	//todo
}
void setupPLightOCallback(renderlistitem_t * ilist, unsigned int count){
	if(count > 1){
		pLightUBOStruct_t ubodata[MAXINSTANCESIZE];
		unsigned int i = 0;
		while(i < count){
			renderPLightCallbackData_t *d = ilist[i].data;
			unsigned int counter = 0;
			ubodata[0] = d[0].light;
			unsigned int max = count-i;
			if(max > MAXINSTANCESIZE) max = MAXINSTANCESIZE;
//COMMENT THIS LINE IF YOU WANT INSTANCE
			max = 1;
//TODO^^
			for(counter = 1; counter < max; counter++){
				ubodata[counter] = d[counter].light;
			}
			int t = pushDataToUBOCache(counter * sizeof(pLightUBOStruct_t), ubodata);
			d->ubodataoffset = t;
			ilist[i].counter = counter; // reset counter, likely wont be needed in this case;
			i+=counter;
		}
	} else if(count == 1){
		renderPLightCallbackData_t *d = ilist->data;
		int t = pushDataToUBOCache(sizeof(pLightUBOStruct_t), &d->light);
		d->ubodataoffset = t;
	} else {
		console_printf("ERROR: PLIGHT SETUP CALLBACK WITH 0 AS COUNT!\n");
	}
}
void drawPLightICallback(renderlistitem_t * ilist, unsigned int count){

	renderPLightCallbackData_t *d = ilist->data;
	states_blendFunc(GL_ONE, GL_ONE);
	states_cullFace(GL_BACK);
	shaderpermutation_t * perm = d->perm;
	if(bindShaderPerm(perm) == 2){
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
		unsigned char numsamples = d->numsamples;
		if(numsamples) glUniform1i(perm->uniint0, numsamples);
	}
	model_t *m = returnModelById(d->modelid);
	vbo_t *v = returnVBOById(m->vbo);
	states_bindVertexArray(v->vaoid);
	unsigned int mysize = ((count * sizeof(pLightUBOStruct_t)));
	states_bindBufferRange(GL_UNIFORM_BUFFER, 0, renderqueueuboid, d->ubodataoffset, mysize);
	glDrawElementsInstanced(GL_TRIANGLES, v->numfaces * 3, GL_UNSIGNED_INT, 0, count);

	//todo
}
void setupPLightICallback(renderlistitem_t * ilist, unsigned int count){
	if(count > 1){
		pLightUBOStruct_t ubodata[MAXINSTANCESIZE];
		unsigned int i = 0;
		while(i < count){
			renderPLightCallbackData_t *d = ilist[i].data;
			unsigned int counter = 0;
			ubodata[0] = d[0].light;
			unsigned int max = count-i;
			if(max > MAXINSTANCESIZE) max = MAXINSTANCESIZE;
//COMMENT THIS LINE IF YOU WANT INSTANCE
			max = 1;
//TODO^^
			for(counter = 1; counter < max; counter++){
				ubodata[counter] = d[counter].light;
			}
			int t = pushDataToUBOCache(counter * sizeof(pLightUBOStruct_t), ubodata);
			d->ubodataoffset = t;
			ilist[i].counter = counter; // reset counter, likely wont be needed in this case;
			i+=counter;
		}
	} else if(count == 1){
		renderPLightCallbackData_t *d = ilist->data;
		int t = pushDataToUBOCache(sizeof(pLightUBOStruct_t), &d->light);
		d->ubodataoffset = t;
	} else {
		console_printf("ERROR: PLIGHT SETUP CALLBACK WITH 0 AS COUNT!\n");
	}
}

int glAddLightsToQueue(viewport_t *v, renderqueue_t * q, unsigned int numsamples){
	shaderprogram_t * shader = returnShaderById(lightshaderid);
	unsigned int permutation = 0;
	shaderpermutation_t * perm;

//	framebuffer_t *df = returnFramebufferById(v->dfbid);
//	framebuffer_t *of = returnFramebufferById(v->outfbid);
//	if(!df || !of) return FALSE;

//	unsigned int numsamples = df->rbflags & FRAMEBUFFERRBFLAGSMSCOUNT;

	if(numsamples){
		numsamples = 1<<numsamples;
//		resolveMultisampleFramebuffer(df); //only resolves if multisampled
//		resolveMultisampleFramebufferSpecify(df, 4);
		permutation = 2;
	}
	perm = addPermutationToShader(shader, permutation);



	lightrenderout_t out = readyLightsForRender(v, 50, 0);
	if(!out.lin.count && !out.lout.count) return FALSE;
	int i;
	for(i = 0; i < out.lin.count; i++){
		renderPLightCallbackData_t pl;
		pl.light.size = out.lin.list[i]->scale;
		pl.light.pos[0] = out.lin.list[i]->pos[0];
		pl.light.pos[1] = out.lin.list[i]->pos[1];
		pl.light.pos[2] = out.lin.list[i]->pos[2];
		pl.modelid = cubeModel;
		pl.shaderid = lightshaderid;
		pl.shaderperm = permutation;
		pl.perm = perm;
		pl.numsamples = numsamples;
		pl.shaderprogram = perm->id;
		pl.v = v;

		renderlistitem_t r;
		r.sort[0] = 0; //first to be drawn in this queue
		r.sort[1] = 0;
		r.sort[2] = 0;
		r.sort[3] = 0;
		r.sort[4] = (pl.shaderprogram >> 0) & 0xFF;
		r.sort[5] = (pl.shaderprogram >> 8) & 0xFF;
		r.sort[6] = (pl.shaderprogram >> 16) & 0xFF;
		r.sort[7] = (pl.shaderprogram >> 24) & 0xFF;
		r.sort[8] = (pl.modelid >> 0) & 0xFF;
		r.sort[9] = (pl.modelid >> 8) & 0xFF;
		r.setup = setupPLightICallback;
		r.draw = drawPLightICallback;
		r.flags = 2 | 4; //copyable, instanceable
		r.datasize = sizeof(renderPLightCallbackData_t);
		r.data = &pl;
		addRenderlistitem(q, r);
	}
	for(i = 0; i < out.lout.count; i++){
		renderPLightCallbackData_t pl;
		pl.light.size = out.lout.list[i]->scale;
		pl.light.pos[0] = out.lout.list[i]->pos[0];
		pl.light.pos[1] = out.lout.list[i]->pos[1];
		pl.light.pos[2] = out.lout.list[i]->pos[2];
		pl.modelid = cubeModel;
		pl.shaderid = lightshaderid;
		pl.shaderperm = permutation;
		pl.perm = perm;
		pl.shaderprogram = perm->id;
		pl.v = v;
		pl.numsamples = numsamples;


		renderlistitem_t r;
		r.sort[0] = 1; //second to be drawn in this queue
		r.sort[1] = 0;
		r.sort[2] = 0;
		r.sort[3] = 0;
		r.sort[4] = (pl.shaderprogram >> 0) & 0xFF;
		r.sort[5] = (pl.shaderprogram >> 8) & 0xFF;
		r.sort[6] = (pl.shaderprogram >> 16) & 0xFF;
		r.sort[7] = (pl.shaderprogram >> 24) & 0xFF;
		r.sort[8] = (pl.modelid >> 0) & 0xFF;
		r.sort[9] = (pl.modelid >> 8) & 0xFF;

		r.setup = setupPLightOCallback;
		r.draw = drawPLightOCallback;
		r.flags = 2 | 4; //copyable, instanceable
		r.datasize = sizeof(renderPLightCallbackData_t);
		r.data = &pl;
		addRenderlistitem(q, r);
	}
	//todo
	return TRUE;
}
int glDeferredLighting(viewport_t *v, renderqueue_t * q){
	framebuffer_t *df = returnFramebufferById(v->dfbid);
	framebuffer_t *of = returnFramebufferById(v->outfbid);
	unsigned int numsamples = df->rbflags & FRAMEBUFFERRBFLAGSMSCOUNT;
	if(numsamples){
		numsamples = 1<<numsamples;
//		resolveMultisampleFramebuffer(df); //only resolves if multisampled
		resolveMultisampleFramebufferSpecify(df, 4);
	}
	bindFramebuffer(of);
//	glDepthMask(GL_FALSE);
	states_depthMask(GL_FALSE);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT| GL_STENCIL_BUFFER_BIT);//todo set OF to use the same renderbuffer for depth as DF
//	glClearBufferfi(of->rb​, GLint drawBuffer​, GLfloat depth​, GLint stencil​);
	glViewport(0, 0, of->width, of->height);


	if(numsamples){
//		glUniform1i(shaderCurrentBound->uniint0, numsamples);

		states_bindActiveTexture(0, GL_TEXTURE_2D_MULTISAMPLE, df->multisampletextures[0].id);
		states_bindActiveTexture(1, GL_TEXTURE_2D_MULTISAMPLE, df->multisampletextures[1].id);
		states_bindActiveTexture(2, GL_TEXTURE_2D_MULTISAMPLE, df->multisampletextures[2].id);
	} else {
		states_bindActiveTexture(0, GL_TEXTURE_2D, df->textures[0].id);
		states_bindActiveTexture(1, GL_TEXTURE_2D, df->textures[1].id);
		states_bindActiveTexture(2, GL_TEXTURE_2D, df->textures[2].id);
	}
	glAddLightsToQueue(v, q, numsamples);
	renderqueueRadixSort(q);
	renderqueueSetup(q);
	renderqueueDraw(q);
//	glDepthMask(GL_TRUE);
	states_depthMask(GL_TRUE);

	return TRUE;
}
//deprecated
int glDrawLights(viewport_t *v){
	lightrenderout_t out = readyLightsForRender(v, 50, 0);
	if(!out.lin.count && !out.lout.count) return FALSE;

	framebuffer_t *df = returnFramebufferById(v->dfbid);
	framebuffer_t *of = returnFramebufferById(v->outfbid);
//	vbo_t * lvbo = returnVBOById(lightvbo); //todo
	if(!df || !of) return FALSE;
	shaderprogram_t * shader = returnShaderById(lightshaderid);
	shaderpermutation_t * perm;
	unsigned int numsamples = df->rbflags & FRAMEBUFFERRBFLAGSMSCOUNT;
	if(numsamples){
		numsamples = 1<<numsamples;
//		resolveMultisampleFramebuffer(df); //only resolves if multisampled
		resolveMultisampleFramebufferSpecify(df, 4);
		perm = addPermutationToShader(shader, 2);
	} else {
		perm = addPermutationToShader(shader, 0);
	}
	if(!bindShaderPerm(perm)) return FALSE;


	//glBindFramebuffer(GL_FRAMEBUFFER, of->id);
	bindFramebuffer(of);
//	glDepthMask(GL_FALSE);
	states_depthMask(GL_FALSE);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT| GL_STENCIL_BUFFER_BIT);//todo set OF to use the same renderbuffer for depth as DF
//	glClearBufferfi(of->rb​, GLint drawBuffer​, GLfloat depth​, GLint stencil​);
	glViewport(0, 0, of->width, of->height);


	if(numsamples){
//		glUniform1i(shaderCurrentBound->uniint0, numsamples);
		glUniform1i(perm->uniint0, numsamples);

		states_bindActiveTexture(0, GL_TEXTURE_2D_MULTISAMPLE, df->multisampletextures[0].id);
		states_bindActiveTexture(1, GL_TEXTURE_2D_MULTISAMPLE, df->multisampletextures[1].id);
		states_bindActiveTexture(2, GL_TEXTURE_2D_MULTISAMPLE, df->multisampletextures[2].id);
	} else {
		states_bindActiveTexture(0, GL_TEXTURE_2D, df->textures[0].id);
		states_bindActiveTexture(1, GL_TEXTURE_2D, df->textures[1].id);
		states_bindActiveTexture(2, GL_TEXTURE_2D, df->textures[2].id);
	}


	states_enable(GL_BLEND);
	states_blendFunc(GL_ONE,GL_ONE);
	model_t * cuber = returnModelById(cubeModel);
	vbo_t * cvbo = returnVBOById(cuber->vbo);
	if(!cvbo) return FALSE;


	GLfloat mout[16];

	Matrix4x4_ToArrayFloatGL(&v->viewproj, mout);
//	glUniformMatrix4fv(shaderCurrentBound->unimat40, 1, GL_FALSE, mout);
	glUniformMatrix4fv(perm->unimat40, 1, GL_FALSE, mout);
	Matrix4x4_ToArrayFloatGL(&v->view, mout);
//	glUniformMatrix4fv(shaderCurrentBound->unimat41, 1, GL_FALSE, mout);
	glUniformMatrix4fv(perm->unimat41, 1, GL_FALSE, mout);
//	glUniform2f(shaderCurrentBound->uniscreensizefix, 1.0/of->width, 1.0/of->height);
	glUniform2f(perm->uniscreensizefix, 1.0/of->width, 1.0/of->height);

	float far = v->far;
	float near = v->near;
//	glUniform2f(shaderCurrentBound->uniscreentodepth, far/(far-near),far*near/(near-far));
	glUniform2f(perm->uniscreentodepth, far/(far-near),far*near/(near-far));





	states_bindVertexArray(cvbo->vaoid);

	//todo can i do this more efficiently

	states_bindBuffer(GL_ARRAY_BUFFER, instancevbo);


/*
	glEnableVertexAttribArray(INSTANCEATTRIBLOC); //tell the location
	glVertexAttribPointer( INSTANCEATTRIBLOC, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0 ); //tell other data
	glVertexAttribDivisor( INSTANCEATTRIBLOC, 1 ); //is it instanced?
*/
//	console_printf("lcount is %i:%i\n", out.lin.count, out.lout.count);
	unsigned int iPerUBOBlock = maxUBOSize / (4*sizeof(GLfloat));

	if(out.lin.count){
		size_t instancedatasize = 4 * out.lin.count * sizeof(GLfloat);


//		unsigned int numUBOBlock = ceil(instancedatasize/maxUBOSize);

		GLfloat * instancedata = malloc(instancedatasize);
		int i;
		for(i = 0; i < out.lin.count; i++){
			int burp = 4*i;
			instancedata[burp] = out.lin.list[i]->pos[0];
			instancedata[burp+1] = out.lin.list[i]->pos[1];
			instancedata[burp+2] = out.lin.list[i]->pos[2];
			instancedata[burp+3] = out.lin.list[i]->scale;
		}
		free(out.lin.list);

		glBufferData(GL_ARRAY_BUFFER, instancedatasize, instancedata, GL_DYNAMIC_DRAW); // change to stream?
		free(instancedata);


//		glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, out.lin.count);
		if(TRUE){
			unsigned int rendered = 0;
			unsigned int torender = iPerUBOBlock;
			while(rendered < out.lin.count){
				if((rendered+torender)>out.lin.count) torender = out.lin.count - rendered;
				states_bindBufferRange(GL_UNIFORM_BUFFER, 0 , instancevbo, (rendered * 4*sizeof(GLfloat)),(torender*4*sizeof(GLfloat)));
				glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, torender);
				rendered+=torender;
			}
		}
	}


	if(out.lout.count){
		size_t instancedatasize = 4 * out.lout.count * sizeof(GLfloat);
		GLfloat * instancedata = malloc(instancedatasize);
		int i;
		for(i = 0; i < out.lout.count; i++){
			int burp = 4*i;
			instancedata[burp] = out.lout.list[i]->pos[0];
			instancedata[burp+1] = out.lout.list[i]->pos[1];
			instancedata[burp+2] = out.lout.list[i]->pos[2];
			instancedata[burp+3] = out.lout.list[i]->scale;
		}
		free(out.lout.list);

		glBufferData(GL_ARRAY_BUFFER, instancedatasize, instancedata, GL_STREAM_DRAW); // change to stream?
		free(instancedata);


		states_disable(GL_DEPTH_TEST);
		states_cullFace(GL_FRONT);


//		glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, out.lout.count);


		if(TRUE){
			unsigned int rendered = 0;
			unsigned int torender = iPerUBOBlock;
			while(rendered < out.lout.count){
				if((rendered+torender)>out.lout.count) torender = out.lout.count - rendered;
				states_bindBufferRange(GL_UNIFORM_BUFFER, 0 , instancevbo, (rendered * 4*sizeof(GLfloat)),(torender*4*sizeof(GLfloat)));
				glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, torender);
				rendered+=torender;
			}
		}

		states_cullFace(GL_BACK);
		states_enable(GL_DEPTH_TEST);


	}


//	glDisable(GL_DEPTH_TEST);
//	glDisable(GL_CULL_FACE);


//	glDepthMask(GL_TRUE);
	states_depthMask(GL_TRUE);
	states_disable(GL_BLEND);
	return TRUE;
}
int glDrawViewport(viewport_t *v){

	framebuffer_t *df = returnFramebufferById(v->dfbid);
	framebuffer_t *of = returnFramebufferById(v->outfbid);
	if(!df || !of) return FALSE;
	//glBindFramebuffer(GL_FRAMEBUFFER, df->id);
	bindFramebuffer(df);

//	GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
//	glDrawBuffers(3, buffers);
//	glBindFramebuffer(GL_FRAMEBUFFER, of->id);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT| GL_STENCIL_BUFFER_BIT);//todo set OF to use the same renderbuffer for depth as DF
//	glViewport(0, 0, df->width, df->height);

//	glStencilFunc(GL_ALWAYS, 1, 0xFF);
//	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
//	glStencilMask(0xFF);
//	glViewport(0, 0, of->width, of->height);

//	renderbatche_t b;
//	memset(&b, 0, sizeof(renderbatche_t));

//	loadEntitiesIntoQueues(&forward, &deferred, v);
//	loadWorldIntoQueues(&forward, &deferred, v);
	loadEntitiesIntoQueue(&deferred, v);
	loadWorldIntoQueue(&deferred, v);
	renderqueueRadixSort(&deferred);
	renderqueueSetup(&deferred);
	renderqueueDraw(&deferred);
//	renderqueueCleanup(&deferred);
	//todo queue stuff

//	glStencilFunc(GL_EQUAL, 1, 0xFF);
//	glStencilMask(0x00);

	glDrawLights(v);
//	glDeferredLighting(v, &forward);




	//todo actually redo this sorta stuffs
	shaderprogram_t * shader = returnShaderById(textshaderid);
	shaderpermutation_t * perm = addPermutationToShader(shader, 0);
	if(!bindShaderPerm(perm)) return FALSE;
	states_enable(GL_BLEND);
	states_blendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	states_bindActiveTexture(0, GL_TEXTURE_2D, df->textures[2].id);
	glDrawFSQuad();
	states_disable(GL_BLEND);




//	glBindFramebuffer(GL_FRAMEBUFFER, of->id);
//	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//	glDrawFSQuad();

	return TRUE;
}
int updateConsoleVBO(unsigned int offset){
	console_updateText(offset);
	if(!consoleVBO){
		consoleVBO = createAndAddVBORPOINT("console", 2);
		states_bindVertexArray(consoleVBO->vaoid);
		states_bindBuffer(GL_ARRAY_BUFFER, consoleVBO->vboid);
                glEnableVertexAttribArray(POSATTRIBLOC);
                glVertexAttribPointer(POSATTRIBLOC, 2, GL_FLOAT, GL_FALSE, 4* sizeof(GLfloat), 0);
                glEnableVertexAttribArray(TCATTRIBLOC);
                glVertexAttribPointer(TCATTRIBLOC, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));
	}

	GLfloat * points = malloc(16 * console_drawlines * sizeof(GLfloat));
//	memset(points, 0.0 , 16* console_drawlines * sizeof(GLfloat));
	GLuint * indices = malloc(6 * console_drawlines * sizeof(GLuint));

	unsigned int currentheight = 0;
	int i;
	for(i = 0; i < console_drawlines; i++){
		int k;
		for(k = 0; k <6; k++) indices[(i*6)+k] = rectangleindices[k] + 4*i;
//		indices[0+i*6] = 0 + 4*i;
//		indices[1+i*6] = 1 + 4*i;
//		indices[2+i*6] = 2 + 4*i;
//		indices[3+i*6] = 0 + 4*i;
//		indices[4+i*6] = 1 + 4*i;
//		indices[5+i*6] = 2 + 4*i;
		for(k = 0; k <16; k++) points[(i*16)+k] = fsquadpoints[k];
//		memcpy(&points[i*16], fsquadpoints, 16 * sizeof(GLfloat));
		points[i*16 +13] = 1.0 - 2.0*((float)currentheight/(float)screenHeight);
		points[i*16 +9] = 1.0 - 2.0*((float)currentheight/(float)screenHeight);
		currentheight += console_texttracker[i].height;
//		currentheight += 10;
//		currentheight = i*10 +10;
		points[i*16 +5] = 1.0 - 2.0*((float)currentheight/(float)screenHeight);
		points[i*16 +1] = 1.0 - 2.0*((float)currentheight/(float)screenHeight);

		points[i*16+4] = 2.0*((float)console_texttracker[i].width/(float)screenWidth) -1.0;
//		points[i*16+4] = 1.0-2.0*((float)console_texttracker[i].width/(float)screenWidth);
		points[i*16+8] = 2.0*((float)console_texttracker[i].width/(float)screenWidth) - 1.0;

//		points[i*16+0] = -0.9;
//		points[i*16+12] = -0.9;
	}
	states_bindVertexArray(consoleVBO->vaoid);
	states_bindBuffer(GL_ARRAY_BUFFER, consoleVBO->vboid);
	glBufferData(GL_ARRAY_BUFFER, 16 * console_drawlines * sizeof(GLfloat), points, GL_STATIC_DRAW); // change to stream?
	states_bindBuffer(GL_ELEMENT_ARRAY_BUFFER, consoleVBO->indicesid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 *console_drawlines* sizeof(GLuint), indices, GL_STATIC_DRAW);
	free(points);
	free(indices);
	consoleVBO->numfaces = 2*console_drawlines;
	consoleVBO->numverts = 4*console_drawlines;
	console_displayneedsupdate = 0;

	return TRUE;
}
int glDrawConsole(void){
	if(console_displayneedsupdate)updateConsoleVBO(console_offset);

	states_activeTexture(0);
//	states_bindTexture(GL_TEXTURE_2D, t->textureid);
//	vbo_t * tvbo = returnVBOById(textvbo);
	shaderprogram_t * shader = returnShaderById(textshaderid);
	shaderpermutation_t * perm = addPermutationToShader(shader, 0);
	if(!bindShaderPerm(perm)) return FALSE;
	states_bindVertexArray(consoleVBO->vaoid);
//	printf("consoledraw = %i\n", console_drawlines);
	int i;
	for(i =0; i < console_drawlines; i++){
//		text_t *t = returnTextById(console_texttracker[i].textid);
		states_bindTexture(GL_TEXTURE_2D, console_texttracker[i].textureid);
//		states_bindTexture(GL_TEXTURE_2D, t->textureid);
		glDrawRangeElements(GL_TRIANGLES, i*4, (i*4)+4, 6, GL_UNSIGNED_INT,  (void*)(i*6*sizeof(GLuint)));
//		console_texttracker[i].textureid;
	}
//	glDrawElements(GL_TRIANGLES, 6*console_drawlines, GL_UNSIGNED_INT, 0);

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
	states_enable(GL_BLEND);
	states_blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	states_disable(GL_DEPTH_TEST);

	//very temp
	framebuffer_t * outfb = returnFramebufferById(cam->outfbid);
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	bindFramebuffer(outfb);
//	glViewport(0, 0, 800, 600);

	glDrawConsole();


	states_disable(GL_BLEND);
	states_enable(GL_DEPTH_TEST);





//	glDrawFSQuad();
	swapBuffers();
//	console_printf("Faces: %li Verts: %li Objects: %li\n", totalface, totalvert, totalcount);
	return TRUE;
}
int glResizeViewports(int width, int height){
	if(!viewportsOK) return FALSE;
	if(!width) width = 1;
	if(!height) height = 1;
	int count =0;
	count+=resizeViewport(findViewportByNameRPOINT("cam"), width, height);
	screenHeight = height;
	screenWidth = width;
	console_displayneedsupdate = 1;
//	updateConsoleVBO();
	return count;
}
