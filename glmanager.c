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

//temp?
renderqueue_t forward;
renderqueue_t deferred;
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
	initUBOSystem();
	if(!uboOK){
		return FALSE;
		//todo call some sort of shutdown of everything
	}
	initAnimSystem();
	if(!animsOK){
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

	statesEnableForce(GL_MULTISAMPLE);
	glClearDepth(1.0);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	statesDisableForce(GL_FOG);
	statesEnableForce(GL_DEPTH_TEST);
//	glDepthFunc(GL_LESS);
	statesDepthFunc(GL_LESS);
	statesEnableForce(GL_CULL_FACE);
	statesCullFace(GL_BACK);

	int maxSamples, maxIntSamples, maxColorTextureSamples, maxDepthTextureSamples;
	glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
	glGetIntegerv(GL_MAX_INTEGER_SAMPLES, &maxIntSamples);
	glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &maxColorTextureSamples);
	glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &maxDepthTextureSamples);
	maxMSAASamples = maxSamples;
	if(maxIntSamples < maxMSAASamples) maxMSAASamples = maxIntSamples;
	if(maxColorTextureSamples < maxMSAASamples) maxMSAASamples = maxColorTextureSamples;
	if(maxDepthTextureSamples < maxMSAASamples) maxMSAASamples = maxDepthTextureSamples;
	consolePrintf("Max multisample samples: %i\n", maxMSAASamples);


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

//	setMSAA(16);

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
void drawModelCallback(void ** data, unsigned int count){
	renderModelCallbackData_t *d = (renderModelCallbackData_t *)*data;
	//todo make instancing support
	model_t *m = returnModelById(d->modelid);
	vbo_t *v = returnVBOById(m->vbo);
	statesBindVertexArray(v->vaoid);
//	shaderprogram_t *s = returnShaderById(d->shaderid);
//	shaderpermutation_t * sp = findShaderPermutation(s, d->shaderperm);
//	shaderpermutation_t * sp = addPermutationToShader(s, d->shaderperm);

//	bindShaderPerm(sp);
	shaderUseProgram(d->shaderprogram);
	texturegroup_t *t = returnTexturegroupById(d->texturegroupid);
	bindTexturegroup(t);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, renderqueueuboid, d->ubodataoffset, 32 * sizeof(GLfloat));

	glDrawElements(GL_TRIANGLES, v->numfaces * 3, GL_UNSIGNED_INT, 0);
//	glDrawElementsInstanced(GL_TRIANGLES, v->numfaces * 3, GL_UNSIGNED_INT, 0, 1);
//	consolePrintf("Rendered\n");
}
void setupModelCallback(void ** data, unsigned int count){
	renderModelCallbackData_t *d = (renderModelCallbackData_t *)*data;
	//todo make instancing support?
	GLfloat ubodata[32];
	Matrix4x4_ToArrayFloatGL(&d->mvp, &ubodata[0]);
	Matrix4x4_ToArrayFloatGL(&d->mv,  &ubodata[16]);
	int t = pushDataToUBOCache(32 * sizeof(GLfloat), ubodata);
	if(t < 0) printf("BAAAD\n");
	d->ubodataoffset = t;
//	consolePrintf("Setup\n");
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
	r.sort[0] = d.shaderprogram & 0x000000FF;
	r.sort[1] = d.shaderprogram & 0x0000FF00;
	r.sort[2] = d.shaderprogram & 0x00FF0000;
	r.sort[3] = d.shaderprogram & 0xFF000000;
	r.sort[4] = modelid & 0x00FF;
	r.sort[5] = modelid & 0xFF00;
	r.sort[6] = texturegroupid & 0x00FF;
	r.sort[7] = texturegroupid & 0xFF00;
	r.sort[8] = 0; //could be distance data in here...
	r.sort[9] = 0;
	r.setup = setupModelCallback;
	r.draw = drawModelCallback;

	r.datasize = sizeof(renderModelCallbackData_t);
	r.flags = 2; //copyable
//	r.flags = 1; //freeable
//	r.data = malloc(r.datasize);
//	memcpy(r.data, &d, r.datasize);
	r.data = &d;



	addRenderlistitem(q, r);
//	consolePrintf("ADDED\n");
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
	r.sort[0] = d.shaderprogram & 0x000000FF;
	r.sort[1] = d.shaderprogram & 0x0000FF00;
	r.sort[2] = d.shaderprogram & 0x00FF0000;
	r.sort[3] = d.shaderprogram & 0xFF000000;
	r.sort[4] = modelid & 0x00FF;
	r.sort[5] = modelid & 0xFF00;
	r.sort[6] = texturegroupid & 0x00FF;
	r.sort[7] = texturegroupid & 0xFF00;
	r.sort[8] = 0;
	r.sort[9] = 0;
	r.setup = setupModelCallback;
	r.draw = drawModelCallback;


	r.datasize = sizeof(renderModelCallbackData_t);
	r.flags = 2; //copyable
//	r.flags = 1; //freeable
//	r.data = malloc(r.datasize);
//	memcpy(r.data, &d, r.datasize);
	r.data = &d;

	addRenderlistitem(q, r);
//	consolePrintf("ADDED\n");
}
int loadEntitiesIntoQueue(renderqueue_t * queue, viewport_t * v){
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
	for(i =0; i <= entityArrayLastTaken; i++){
		entity_t *e = &entitylist[i];
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
	for(i =0; i <= entityArrayLastTaken; i++){
		entity_t *e = &entitylist[i];
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
	for(i =0; i <= entityArrayLastTaken; i++){
		entity_t *e = &entitylist[i];
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
/*
//deprecaaaated
int drawEntitiesM(modelbatche_t * batch){

	//todo i need something to go through all the model batches and double check that there is more than x ents in there to draw. If not, it repositions it into a new shaderbatch where it does not use instancing
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

//	for(i = 0; i < count; i++){
//		glDrawModel(m, &batch->matlist[i], &cam->viewproj);
//	}


//	glBindBuffer(GL_ARRAY_BUFFER, instancevbo);
		size_t sizePerInstance = 16*sizeof(GLfloat);
//		size_t sizePerInstance2 = 12*sizeof(GLfloat);
		size_t instancedatasize = count * sizePerInstance;
//		size_t instancedatasize2 = count * sizePerInstance2;

//		for(i = 0; i < 4; i++){
//			glEnableVertexAttribArray(INSTANCEATTRIBLOC+i); //tell the location
//			glVertexAttribPointer(INSTANCEATTRIBLOC+i, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(GLfloat), (char *)(i*4*sizeof(GLfloat)) ); //tell other data
//			glVertexAttribDivisor(INSTANCEATTRIBLOC+i, 1); //is it instanced?
//		}

		unsigned int iPerUBOBlock = maxUBOSize / sizePerInstance;
		GLfloat * instancedata = malloc(instancedatasize);
//		GLfloat * instancedata2 = malloc(instancedatasize2);
		GLfloat * instancedata2 = malloc(instancedatasize);


		for(i = 0; i < count; i++){
//		glDrawModel(m, &batch->matlist[i], &cam->viewproj);

			unsigned int bump = i * 16;
			unsigned int bump2 = i * 12;
			matrix4x4_t outmat, outmatrot;
		//	Matrix4x4_Concat(&outmat, modworld, viewproj);
			Matrix4x4_Concat(&outmat, &cam->viewproj, &batch->matlist[i]);
			Matrix4x4_ToArrayFloatGL(&outmat, &instancedata[bump]);

			Matrix4x4_Concat(&outmat, &cam->view, &batch->matlist[i]);
//			Matrix4x4_CopyRotateOnly(&outmatrot, &outmat);
//			Matrix4x4_ToArray12FloatGL(&outmatrot, &instancedata2[bump2]);
			Matrix4x4_ToArrayFloatGL(&outmat, &instancedata2[bump]);
			//todo

		}

		glBindBuffer(GL_ARRAY_BUFFER, instancevbo);
		glBufferData(GL_ARRAY_BUFFER, instancedatasize, instancedata, GL_DYNAMIC_DRAW); // change to stream?
		free(instancedata);

		glBindBuffer(GL_ARRAY_BUFFER, instancevbo2);
//		glBufferData(GL_ARRAY_BUFFER, instancedatasize2, instancedata2, GL_DYNAMIC_DRAW); // change to stream?
		glBufferData(GL_ARRAY_BUFFER, instancedatasize, instancedata2, GL_DYNAMIC_DRAW); // change to stream?
		free(instancedata2);

//		glDrawElementsInstanced(GL_TRIANGLES, tvbo->numfaces * 3, GL_UNSIGNED_INT, 0, count);

                if(TRUE){
			unsigned int rendered = 0;
			unsigned int torender = iPerUBOBlock;
			unsigned int vertdrawcount = tvbo->numfaces*3;
                        while(rendered < count){
				if((rendered+torender)>count) torender = count - rendered;
				glBindBufferRange(GL_UNIFORM_BUFFER, 0 , instancevbo, (rendered * sizePerInstance),(torender*sizePerInstance));
//				glBindBufferRange(GL_UNIFORM_BUFFER, 1 , instancevbo2, (rendered * sizePerInstance2),(torender*sizePerInstance2));
				glBindBufferRange(GL_UNIFORM_BUFFER, 1 , instancevbo2, (rendered * sizePerInstance),(torender*sizePerInstance));
				glDrawElementsInstanced(GL_TRIANGLES, vertdrawcount, GL_UNSIGNED_INT, 0, torender);
				rendered+=torender;
			}
		}
		totalface += tvbo->numfaces * count;
		totalvert += tvbo->numverts * count;
		totalcount+= count;

	return count;
}
int drawEntitiesT(texturebatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	if(!count || !batch->modelbatch) return FALSE;
	int i;
	//stuff here
	texturegroup_t * tex = returnTexturegroupById(batch->textureid);
	bindTexturegroup(tex); //todo make it bind error texture
//		consolePrintf("error:%i\n", glGetError());
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
	//todo have something to load the shader if that check fails

	shaderpermutation_t * perm = addPermutationToShader(shader, batch->shaderperm);
	if(!bindShaderPerm(perm)) return FALSE;
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
*/
void glDrawFSQuad(void){
	model_t * m = returnModelById(fsquadmodel);
	vbo_t * tvbo = returnVBOById(m->vbo);
	glBindVertexArray(tvbo->vaoid);
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

typedef struct renderLightCallbackData_s {
	//todo?
	unsigned int ubodataoffset;
	unsigned char numsamples;
} renderLightCallbackData_t;
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
	glDepthMask(GL_FALSE);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT| GL_STENCIL_BUFFER_BIT);//todo set OF to use the same renderbuffer for depth as DF
//	glClearBufferfi(of->rb​, GLint drawBuffer​, GLfloat depth​, GLint stencil​);
	glViewport(0, 0, of->width, of->height);


	if(numsamples){
		glUniform1i(shaderCurrentBound->uniint0, numsamples);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, df->multisampletextures[0].id);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, df->multisampletextures[1].id);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, df->multisampletextures[2].id);
	} else {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, df->textures[0].id);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, df->textures[1].id);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, df->textures[2].id);
	}


	statesEnable(GL_BLEND);
	statesBlendFunc(GL_ONE,GL_ONE);
	model_t * cuber = returnModelById(cubeModel);
	vbo_t * cvbo = returnVBOById(cuber->vbo);
	if(!cvbo) return FALSE;


	GLfloat mout[16];

	Matrix4x4_ToArrayFloatGL(&v->viewproj, mout);
	glUniformMatrix4fv(shaderCurrentBound->unimat40, 1, GL_FALSE, mout);
	Matrix4x4_ToArrayFloatGL(&v->view, mout);
	glUniformMatrix4fv(shaderCurrentBound->unimat41, 1, GL_FALSE, mout);
	glUniform2f(shaderCurrentBound->uniscreensizefix, 1.0/of->width, 1.0/of->height);

	float far = v->far;
	float near = v->near;
	glUniform2f(shaderCurrentBound->uniscreentodepth, far/(far-near),far*near/(near-far));





	glBindVertexArray(cvbo->vaoid);

	//todo can i do this more efficiently

	glBindBuffer(GL_ARRAY_BUFFER, instancevbo);


/*
	glEnableVertexAttribArray(INSTANCEATTRIBLOC); //tell the location
	glVertexAttribPointer( INSTANCEATTRIBLOC, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0 ); //tell other data
	glVertexAttribDivisor( INSTANCEATTRIBLOC, 1 ); //is it instanced?
*/
//	consolePrintf("lcount is %i:%i\n", out.lin.count, out.lout.count);
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
				glBindBufferRange(GL_UNIFORM_BUFFER, 0 , instancevbo, (rendered * 4*sizeof(GLfloat)),(torender*4*sizeof(GLfloat)));
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


		statesDisable(GL_DEPTH_TEST);
		statesCullFace(GL_FRONT);


//		glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, out.lout.count);


		if(TRUE){
			unsigned int rendered = 0;
			unsigned int torender = iPerUBOBlock;
			while(rendered < out.lout.count){
				if((rendered+torender)>out.lout.count) torender = out.lout.count - rendered;
				glBindBufferRange(GL_UNIFORM_BUFFER, 0 , instancevbo, (rendered * 4*sizeof(GLfloat)),(torender*4*sizeof(GLfloat)));
				glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, torender);
				rendered+=torender;
			}
		}


		statesCullFace(GL_BACK);
		statesEnable(GL_DEPTH_TEST);


	}


//	glDisable(GL_DEPTH_TEST);
//	glDisable(GL_CULL_FACE);


	glDepthMask(GL_TRUE);
	statesDisable(GL_BLEND);
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
	renderqueueCleanup(&deferred);
	//todo queue stuff

//	glStencilFunc(GL_EQUAL, 1, 0xFF);
//	glStencilMask(0x00);

	glDrawLights(v);




	//todo actually redo this sorta stuffs
	shaderprogram_t * shader = returnShaderById(textshaderid);
	shaderpermutation_t * perm = addPermutationToShader(shader, 0);
	if(!bindShaderPerm(perm)) return FALSE;
	statesEnable(GL_BLEND);
	statesBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, df->textures[2].id);
	glDrawFSQuad();
	statesDisable(GL_BLEND);




//	glBindFramebuffer(GL_FRAMEBUFFER, of->id);
//	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//	glDrawFSQuad();

	return TRUE;
}
int updateConsoleVBO(unsigned int offset){
	updateConsoleText(offset);
	if(!consoleVBO){
		consoleVBO = createAndAddVBORPOINT("console", 2);
		glBindVertexArray(consoleVBO->vaoid);
		glBindBuffer(GL_ARRAY_BUFFER, consoleVBO->vboid);
                glEnableVertexAttribArray(POSATTRIBLOC);
                glVertexAttribPointer(POSATTRIBLOC, 2, GL_FLOAT, GL_FALSE, 4* sizeof(GLfloat), 0);
                glEnableVertexAttribArray(TCATTRIBLOC);
                glVertexAttribPointer(TCATTRIBLOC, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));
	}

	GLfloat * points = malloc(16 * consoleDrawLines * sizeof(GLfloat));
//	memset(points, 0.0 , 16* consoleDrawLines * sizeof(GLfloat));
	GLuint * indices = malloc(6 * consoleDrawLines * sizeof(GLuint));

	unsigned int currentheight = 0;
	int i;
	for(i = 0; i < consoleDrawLines; i++){
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
		currentheight += texttracker[i].height;
//		currentheight += 10;
//		currentheight = i*10 +10;
		points[i*16 +5] = 1.0 - 2.0*((float)currentheight/(float)screenHeight);
		points[i*16 +1] = 1.0 - 2.0*((float)currentheight/(float)screenHeight);

		points[i*16+4] = 2.0*((float)texttracker[i].width/(float)screenWidth) -1.0;
//		points[i*16+4] = 1.0-2.0*((float)texttracker[i].width/(float)screenWidth);
		points[i*16+8] = 2.0*((float)texttracker[i].width/(float)screenWidth) - 1.0;

//		points[i*16+0] = -0.9;
//		points[i*16+12] = -0.9;
	}
	glBindVertexArray(consoleVBO->vaoid);
	glBindBuffer(GL_ARRAY_BUFFER, consoleVBO->vboid);
	glBufferData(GL_ARRAY_BUFFER, 16 * consoleDrawLines * sizeof(GLfloat), points, GL_STATIC_DRAW); // change to stream?
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, consoleVBO->indicesid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 *consoleDrawLines* sizeof(GLuint), indices, GL_STATIC_DRAW);
	free(points);
	free(indices);
	consoleVBO->numfaces = 2*consoleDrawLines;
	consoleVBO->numverts = 4*consoleDrawLines;
	consoleDisplayNeedsUpdate = 0;

	return TRUE;
}
int glDrawConsole(void){
	if(consoleDisplayNeedsUpdate)updateConsoleVBO(consoleoffset);

	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, t->textureid);
//	vbo_t * tvbo = returnVBOById(textvbo);
	shaderprogram_t * shader = returnShaderById(textshaderid);
	shaderpermutation_t * perm = addPermutationToShader(shader, 0);
	if(!bindShaderPerm(perm)) return FALSE;
	glBindVertexArray(consoleVBO->vaoid);
//	printf("consoledraw = %i\n", consoleDrawLines);
	int i;
	for(i =0; i < consoleDrawLines; i++){
//		text_t *t = returnTextById(texttracker[i].textid);
		glBindTexture(GL_TEXTURE_2D, texttracker[i].textureid);
//		glBindTexture(GL_TEXTURE_2D, t->textureid);
		glDrawRangeElements(GL_TRIANGLES, i*4, (i*4)+4, 6, GL_UNSIGNED_INT,  (void*)(i*6*sizeof(GLuint)));
//		texttracker[i].textureid;
	}
//	glDrawElements(GL_TRIANGLES, 6*consoleDrawLines, GL_UNSIGNED_INT, 0);

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
	statesEnable(GL_BLEND);
	statesBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	statesDisable(GL_DEPTH_TEST);

	//very temp
	framebuffer_t * outfb = returnFramebufferById(cam->outfbid);
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	bindFramebuffer(outfb);
//	glViewport(0, 0, 800, 600);

	glDrawConsole();


	statesDisable(GL_BLEND);
	statesEnable(GL_DEPTH_TEST);





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
	screenHeight = height;
	screenWidth = width;
	consoleDisplayNeedsUpdate = 1;
//	updateConsoleVBO();
	return count;
}
