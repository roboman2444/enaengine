//global includes
#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>
#include <tgmath.h>
//local includes
#include "globaldefs.h"
#include "hashtables.h"
#include "matrixlib.h"
#include "entitymanager.h"
#include "mathlib.h"
#include "viewportmanager.h"
#include "lightmanager.h"
#include "console.h"
#include "mathlib.h"

#include "renderqueue.h"
#include "shadermanager.h"
#include "texturemanager.h"
#include "framebuffermanager.h"
#include "modelmanager.h"
#include "vbomanager.h"
#include "glstates.h"
#include "glmanager.h"

int lightcount = 0;
int lightArrayFirstOpen = 0;
int lightArrayLastTaken = -1;
int lightArraySize = 0;
int light_ok = 0;
light_t *lightlist;
hashbucket_t lighthashtable[MAXHASHBUCKETS];

vec_t unitbox[24] = {
	-1.0, -1.0, -1.0,	1.0, -1.0, -1.0,	-1.0, 1.0, -1.0,	1.0, 1.0, -1.0,
	-1.0, -1.0, 1.0,	1.0, -1.0, 1.0,		-1.0, 1.0, 1.0,		1.0, 1.0, 1.0 };

int cubeModel = 0; // todo move this as well as the other primitives into modelmanager
int sphereModel = 0; //todo move this
int lightconeModel = 0; //todo move this
int lightshaderid = 0;

GLuint spotfaces[39]={	0,2,1, 0,3,2, 0,4,3, 0,5,4, //side faces
			0,6,5, 0,7,6, 0,8,7,	    //side faces
			1,2,3, 1,3,4, 1,4,5, 1,5,6, //cap faces
			1,6,7, 1,7,8,}; //cap faces
vec_t spotverts[24] = {0.0};

void recalcLightBBox(light_t *l){
	//spot light
	if(l->type == 2){
		//odl version
		/*vec3_t t;
		Matrix4x4_Transform(&l->camproj, unitbox, t);
		l->bboxp[0] = t[0];
		l->bboxp[1] = t[1];
		l->bboxp[2] = t[2];
		l->bbox[0] = t[0];
		l->bbox[1] = t[0];
		l->bbox[2] = t[1];
		l->bbox[3] = t[1];
		l->bbox[4] = t[2];
		l->bbox[5] = t[2];
		int i;
		for(i = 1; i < 8; i++){
			Matrix4x4_Transform(&l->camproj, &unitbox[i*3], t);
			l->bboxp[i*3+0] = t[0];
			l->bboxp[i*3+1] = t[1];
			l->bboxp[i*3+2] = t[2];
			if(t[0] < l->bbox[0]) l->bbox[0] = t[0];
			else if(t[0] > l->bbox[1]) l->bbox[1] = t[0];
			if(t[1] < l->bbox[2]) l->bbox[2] = t[1];
			else if(t[1] > l->bbox[3]) l->bbox[3] = t[1];
			if(t[2] < l->bbox[4]) l->bbox[4] = t[2];
			else if(t[2] > l->bbox[5]) l->bbox[5] = t[2];
		}*/
		//new method
		l->bbox[0] = l->pos[0];
		l->bbox[1] = l->pos[0];
		l->bbox[2] = l->pos[1];
		l->bbox[3] = l->pos[1];
		l->bbox[4] = l->pos[2];
		l->bbox[5] = l->pos[2];
		l->bboxp[0]= l->pos[0];
		l->bboxp[1]= l->pos[1];
		l->bboxp[2]= l->pos[2];
		int i;
		for(i = 1; i < 8; i++){
			vec3_t t;
			Matrix4x4_Transform(&l->camproj, &spotverts[i*3], t);
			l->bboxp[i*3+0] = t[0];
			l->bboxp[i*3+1] = t[1];
			l->bboxp[i*3+2] = t[2];
			if(t[0] < l->bbox[0]) l->bbox[0] = t[0];
			else if(t[0] > l->bbox[1]) l->bbox[1] = t[0];
			if(t[1] < l->bbox[2]) l->bbox[2] = t[1];
			else if(t[1] > l->bbox[3]) l->bbox[3] = t[1];
			if(t[2] < l->bbox[4]) l->bbox[4] = t[2];
			else if(t[2] > l->bbox[5]) l->bbox[5] = t[2];
		}
	} else {
		float scale = l->scale;
		l->bbox[0] = l->pos[0] - scale;
		l->bbox[1] = l->pos[0] + scale;
		l->bbox[2] = l->pos[1] - scale;
		l->bbox[3] = l->pos[1] + scale;
		l->bbox[4] = l->pos[2] - scale;
		l->bbox[5] = l->pos[2] + scale;
		getBBoxPFromBBox(l->bbox, l->bboxp);
	}
}
int recheckLightLeaf(light_t *l){
	//todo
	return TRUE;
}
void recalcLightViewMats(light_t *l){
	//todo if it is attached, just grab the attached matrix (same as entities do it)
	Matrix4x4_CreateRotate(&l->view, l->angle[2], 0.0f, 0.0f, 1.0f);
	Matrix4x4_ConcatRotate(&l->view, l->angle[0], 1.0f, 0.0f, 0.0f);
	Matrix4x4_ConcatRotate(&l->view, l->angle[1], 0.0f, 1.0f, 0.0f);
	Matrix4x4_ConcatTranslate(&l->view, -l->pos[0], -l->pos[1], -l->pos[2]);
//	Matrix4x4_CreateFromQuakeEntity(&l->cam, l->pos[0], l->pos[1], l->pos[2], l->angle[2], l->angle[1], l->angle[0], 1.0);
	Matrix4x4_CreateFromQuakeEntity(&l->cam, l->pos[0], l->pos[1], l->pos[2], l->angle[2], l->angle[1], l->angle[0], l->scale);
}
void recalcLightProjMats(light_t *l){
	//todo do i really need the whole seperate x and y cotangent radians sines, etc?
	double sinex, siney, cotangentx, cotangenty, deltaZ;
	double radiansx = l->fovx / 2.0 * M_PI / 180.0;
	double radiansy = l->fovy / 2.0 * M_PI / 180.0;

	deltaZ = l->far - l->near;
	sinex = sin(radiansx);
	siney = sin(radiansy);
	if ((deltaZ == 0) || (sinex == 0) || (siney == 0) || (l->fovy == 0) || (l->fovx == 0)) {
		return;
	}
	cotangentx = cos(radiansx) / sinex;
	cotangenty = cos(radiansy) / siney;

	l->projection.m[0][0] = cotangentx;
	l->projection.m[1][1] = cotangenty;
	l->projection.m[2][2] = -(l->far + l->near) / deltaZ;
	l->projection.m[2][3] = -1.0;
	l->projection.m[3][2] = -2.0 * l->near * l->far / deltaZ;
	l->projection.m[3][3] = 0;

	l->fixproj.m[0][0] = 1.0;//TODO
	l->fixproj.m[1][1] = 1.0;//TODO
	l->fixproj.m[2][2] = l->far;
/*
	l->fixproj.m[0][0] = 1.0/cotangentx;
	l->fixproj.m[1][1] = 1.0/cotangenty;
	l->fixproj.m[2][2] = 1.0/l->projection.m[2][2];
	l->fixproj.m[2][3] = -1.0;
	l->fixproj.m[3][2] = 1.0/l->projection.m[3][2];
	l->fixproj.m[3][3] = 0;
*/
	//todo for fixproj?
}
void recalcLightMats(light_t *l){
	if(l->needsupdate & 1)recalcLightViewMats(l);
	if(l->needsupdate & 2)recalcLightProjMats(l);
	Matrix4x4_Concat(&l->viewproj, &l->projection, &l->view);
	Matrix4x4_Concat(&l->camproj, &l->cam, &l->fixproj);
}
int lightLoop(void){
	int count = 0, i;
	for(i = 0; i <= lightArrayLastTaken; i++){
		light_t *l = &lightlist[i];
		entity_t *e = entity_returnById(l->attachmentid);
		if(e){
			//todo something for non point lights to check and set angle as well
//			vec3_t tvec;
//			Matrix4x4_OriginFromMatrix(&e->mat, tvec);
			if(e->needsmatupdate || e->needsbboxupdate){
//			if(!vec3comp(tvec, l->pos)){
//				l->pos[0] = tvec[0];
//				l->pos[1] = tvec[1];
//				l->pos[2] = tvec[2];
				Matrix4x4_OriginFromMatrix(&e->mat, l->pos);
				//recalcLightMats(l);
				recalcLightBBox(l);
				recheckLightLeaf(l);
				count++;
				l->needsupdate = 0;
//	console_printf("updated light\n");
			}
		} else if(l->needsupdate){
				//recalcLightMats(l);
				recalcLightBBox(l);
				recheckLightLeaf(l);
				count++;
				l->needsupdate = 0;
		}
	}
	return count;
}


int light_init(void){
	memset(lighthashtable, 0, MAXHASHBUCKETS * sizeof(hashbucket_t));
	if(lightlist) free(lightlist);
	lightlist = 0;
//	lightlist = malloc(lightcount * sizeof(light_t));
//	if(!lightlist) memset(lightlist, 0 , lightcount * sizeof(light_t));
//	light_addRINT("default");
	//todo make this useful, gonna need a large list to put the lights in viewport into, then sort front to back and remove back ones
	maxVisLights = 50;
	maxShadowLights = 20;
	cubeModel = model_findByNameRINT("cube");
	sphereModel = model_createAndAddRINT("sphere");
	lightshaderid = shader_createAndAddRINT("deferredlight");
	float mvrad = cos(M_PI/7.0f);
	float myinc = M_PI * 2.0/7.0;
	int i;
	for(i = 1; i < 8; i++){
		spotverts[i*3+0] = cos((i-1)*myinc) * mvrad;
		spotverts[i*3+1] = sin((i-1)*myinc) * mvrad;
		spotverts[i*3+2] = 1.0;
	}
	model_t *cm = model_createAndAddRPOINT("LightCone");
	lightconeModel = cm->myid;
	cm->type = 1;
	vbo_t *cvbo = createAndAddVBORPOINT("LightCone", 1);
	if(!cvbo) return FALSE;
	cm->vbo = cvbo->myid;
	states_bindBuffer(GL_ARRAY_BUFFER, cvbo->vboid);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), spotverts, GL_STATIC_DRAW);
	cvbo->numverts = 24;
	glEnableVertexAttribArray(POSATTRIBLOC);
	glVertexAttribPointer(POSATTRIBLOC, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
	states_bindBuffer(GL_ELEMENT_ARRAY_BUFFER, cvbo->indicesid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 39 *sizeof(GLuint), spotfaces, GL_STATIC_DRAW);
	cvbo->numfaces = 13;
	light_ok = TRUE;
	return TRUE; // todo error check
}
lightlistpoint_t findLightsByNameRPOINT(const char * name){
	lightlistpoint_t ret;
	int hash = getHash(name);
	hashbucket_t * hb = &lighthashtable[hash];
	if(!hb->name) return ret;
        for(; hb; hb = hb->next){
		if(strcmp(hb->name, name)==0){
//			return returnLightById(hb->id);
			ret.count++;
			ret.list = realloc(ret.list, ret.count * sizeof(light_t *));
			ret.list[ret.count-1] = returnLightById(hb->id);
		}
        }
	return ret;
}
lightlistint_t findLightsByNameRINT(const char * name){
	lightlistint_t ret;
	int hash = getHash(name);
	hashbucket_t * hb = &lighthashtable[hash];
	if(!hb->name) return ret;
        for(; hb; hb = hb->next){
		if(strcmp(hb->name, name)==0){
//			return returnLightById(hb->id);
			ret.count++;
			ret.list = realloc(ret.list, ret.count * sizeof(int));
			ret.list[ret.count-1] = hb->id;
		}
        }
	return ret;
}

light_t * findLightByNameRPOINT(const char * name){ //todo write a function that can find ALL entities with name
	return returnLightById(findByNameRINT(name, lighthashtable));
}
int findLightByNameRINT(const char * name){
	return findByNameRINT(name, lighthashtable);
}

int deleteLight(const int id){
	int lightindex = (id & 0xFFFF);
	light_t * ent = &lightlist[lightindex];
	if(ent->myid != id) return FALSE;
	if(!ent->name) return FALSE;
	deleteFromHashTable(ent->name, id, lighthashtable);
	free(ent->name);
	memset(ent, 0, sizeof(light_t));
//	ent->type = 0;
//	ent->model = 0;
//	ent->name = 0;
//	ent->myid = 0;
	if(lightindex < lightArrayFirstOpen) lightArrayFirstOpen = lightindex;
	for(; lightArrayLastTaken > 0 && !lightlist[lightArrayLastTaken].type; lightArrayLastTaken--);
	return TRUE;
}
light_t * returnLightById(const int id){
//	int lightspawncount = (id >> 16);
	int lightindex = (id & 0xFFFF);
	light_t * ent = &lightlist[lightindex];
	if(!ent->type) return FALSE;
	if(ent->myid == id) return ent;
	return FALSE;
}
light_t createLight(const char * name){
	light_t newlight;
	memset(&newlight, 0, sizeof(light_t));
	newlight.type = 1;
	newlight.name = malloc(strlen(name)+1); // todo maybe put this somewhere else...
	strcpy(newlight.name, name);
	newlight.scale = 1.0;
//	Matrix4x4_CreateIdentity(&newlight.view);
	Matrix4x4_CreateIdentity(&newlight.projection);
//	Matrix4x4_CreateIdentity(&newlight.cam);
//	Matrix4x4_CreateIdentity(&newlight.fixproj);
	newlight.needsupdate = 3;
	return newlight;
//todo
}



int light_addRINT(const char * name){
	lightcount++;
	for(; lightArrayFirstOpen < lightArraySize && lightlist[lightArrayFirstOpen].type; lightArrayFirstOpen++);
	if(lightArrayFirstOpen == lightArraySize){	//resize
		lightArraySize++;
		lightlist = realloc(lightlist, lightArraySize * sizeof(light_t));
	}
	lightlist[lightArrayFirstOpen] = createLight(name);
	int returnid = (lightcount << 16) | lightArrayFirstOpen;
	lightlist[lightArrayFirstOpen].myid = returnid;

	addToHashTable(lightlist[lightArrayFirstOpen].name, returnid, lighthashtable);
	if(lightArrayLastTaken < lightArrayFirstOpen) lightArrayLastTaken = lightArrayFirstOpen; //todo redo
	return returnid;
}
light_t * light_addRPOINT(const char * name){
	lightcount++;
	for(; lightArrayFirstOpen < lightArraySize && lightlist[lightArrayFirstOpen].type; lightArrayFirstOpen++);
	if(lightArrayFirstOpen == lightArraySize){	//resize
		lightArraySize++;
		lightlist = realloc(lightlist, lightArraySize * sizeof(light_t));
	}
	lightlist[lightArrayFirstOpen] = createLight(name);
	int returnid = (lightcount << 16) | lightArrayFirstOpen;
	lightlist[lightArrayFirstOpen].myid = returnid;

	addToHashTable(lightlist[lightArrayFirstOpen].name, returnid, lighthashtable);
	//todo maybe have light have a hash variable, so i dont have to calculate it again if i want to delete... maybe
	if(lightArrayLastTaken < lightArrayFirstOpen) lightArrayLastTaken = lightArrayFirstOpen;
//	printf("lightarraysize = %i\n", lightArraySize);
//	printf("lightcount = %i\n", lightcount);

	return &lightlist[lightArrayFirstOpen];

}


void pruneLightList(void){
	if(lightArraySize == lightArrayLastTaken+1) return;
	lightArraySize = lightArrayLastTaken+1;
	lightlist = realloc(lightlist, lightArraySize * sizeof(light_t));
}


typedef struct lightlistdist_s {
	light_t **list;
	unsigned int count;
	vec_t * dist;
	char * infrustum;
} lightlistdist_t;
int lightQuickPartition(lightlistdist_t list, int l, int r){
//adapted from http://www.comp.dit.ie/rlawlor/Alg_DS/sorting/quickSort.c
	int i, j;
	vec_t pivot;
	vec_t td;
	light_t * tl;
	pivot = list.dist[l];
	i = l; j = r+1;
	while(1){
	//todo fix this?
		do ++i; while(list.dist[i] <= pivot && i<= r);
		do --j; while (list.dist[j] > pivot);
		if(i >= j) break;
		//swap em
		td = list.dist[i]; list.dist[i] = list.dist[j]; list.dist[j] = td;
		tl = list.list[i]; list.list[i] = list.list[j]; list.list[j] = tl;
	}
	//swap again?
	td = list.dist[l]; list.dist[l] = list.dist[j]; list.dist[j] = td;
	tl = list.list[l]; list.list[l] = list.list[j]; list.list[j] = tl;
	return j;
}
void lightQuickSort(lightlistdist_t list, int l, int r){
//adapted from http://www.comp.dit.ie/rlawlor/Alg_DS/sorting/quickSort.c
	int j;
	if(l < r){
		//partition myself
		j = lightQuickPartition(list, l, r);
		//now partition the two sides on either side of my "middle"
		lightQuickSort(list, l, j-1);
		lightQuickSort(list, j+1, r);
	}
}



typedef struct lightbucket_s {
	light_t * l;
	vec_t dist;
	struct lightbucket_s *next;
} lightbucket_t;

typedef struct lightbuckethead_s {
	unsigned int count;
	struct lightbucket_s *next;
	struct lightbucket_s *tail;
} lightbuckethead_t;

void lightHashSortPrune(lightlistdist_t * list, const vec_t lmaxdist, const unsigned int max){
	unsigned int count = list->count;
	unsigned int bucketcount = (count / 10) +1;
//	console_printf("bucketcount is %i\n", bucketcount);
	//todo fix this?
	vec_t bucketsize = lmaxdist / (vec_t)(bucketcount-1);

	lightbuckethead_t * table = malloc(bucketcount * sizeof(lightbuckethead_t));
	memset(table, 0 , bucketcount * sizeof(lightbuckethead_t));
	// int lighthash = dist / bucketsize;
	unsigned int i;
	vec_t * dist = list->dist;
	light_t ** tlist = list->list;
	//fill table
	for(i = 0; i < count; i++){
		unsigned int lighthash = (int)(dist[i] / bucketsize);
		if(lighthash > bucketcount) console_printf("u dun goofed now! %i:%i:%i:%i\n", lighthash, dist, bucketsize, bucketcount);
//		printf("lighthash is %i\n", lighthash);
		lightbuckethead_t * h = &table[lighthash];
		lightbucket_t * j = malloc(sizeof(lightbucket_t));
		j->l = tlist[i];
		j->dist = dist[i];
		j->next = 0;
		if(h->tail){
			lightbucket_t *l = h->tail;
			l->next = j;
		} else {
			h->next = j;
		}
		h->tail = j;
		h->count++;
	}

	//dont need to keep that extra data around, so im gonna prune it off now
	list->list = realloc(list->list, max * sizeof(light_t *));
	list->dist = realloc(list->dist, max * sizeof(vec_t));
	list->count = max;


	//go through table, find the "pivot" slot
	unsigned int lcount = 0;
	unsigned int pivot = 0;
	for(pivot = 0; pivot < bucketcount; pivot++){
		lcount+=table[pivot].count;
		if(lcount >= max) break;
	}
	//i should be at the pivot slot
		lightlistdist_t tosort = {0, 0, 0, 0};

	if(lcount != max){
		//organize pivot slot into a new lightlistdist
		tosort.count = table[pivot].count;
		tosort.list = malloc(tosort.count * sizeof(light_t *));
		tosort.dist = malloc(tosort.count * sizeof(vec_t));
		lightbucket_t * b = table[pivot].next;
		for(i = 0; i < tosort.count; i++){
			tosort.list[i] = b->l;
			tosort.dist[i] = b->dist;
			b = b->next;
		}
		//sort my new lightlistdist
		lightQuickSort(tosort, 0 , tosort.count-1);
	} else { // buckets lined up perfectly so that pivot slot can be full

		pivot++;
	}


	// go through buckets, pushing their data into my output list
	unsigned int outarrayloc = 0;
	for(i=0; i < pivot; i++){
		lightbucket_t * b = table[i].next;
		for(; b; b = b->next){
			list->list[outarrayloc] = b->l;
			list->dist[outarrayloc] = b->dist;
			outarrayloc++;
		}
	}

	//now copy over the temporary list
	if(lcount != max){
		unsigned int tlistlength = max - outarrayloc;
		memcpy(&list->list[outarrayloc], tosort.list, tlistlength * sizeof(light_t *));
		memcpy(&list->dist[outarrayloc], tosort.dist, tlistlength * sizeof(vec_t));
	}


	//at this point my input list should be pruned and fixed

	//free temp lightlistdist
	if(tosort.list) free(tosort.list);
	if(tosort.dist) free(tosort.dist);
	//walk and free table
	for(i=0; i < bucketcount; i++){
		lightbucket_t * b = table[i].next;
		lightbucket_t * next;
		for(; b; b = next){
			next = b->next;
			free(b);
		}
	}
	if(table)free(table);
}


//considerations
//if i do happen to do something that "caches" shadowmaps.
// maybe have some sort of feature that there is a max updated shadowmaps per render, and a max total shadowed lights per render.
//that way, shadowmaps can be "streamed in" much like unreal 3's crappy texture streaming
lightrenderout_t readyLightsForRender(viewport_t *v, const unsigned int max, const unsigned int maxshadowed){

	vec_t lmaxdist = 0;
//	vec_t lmaxshadowdist = 0;

	lightlistdist_t llist = {0, 0, 0, 0};
//	lightlistpoint_t lshadowlist = {0, 0};

	int i;
	//todo put shadowed lights in here as well
	//populate pre-sorted list of unshadowed lights
	for( i = 0; i <= lightArrayLastTaken; i++){
		light_t *l = &lightlist[i];
		if(!l->type) continue;
		//get lights test to frustum, if it isnt even in it, move on
		//todo use a sphere if this light is a point light? had some errors, because the sphere model was sliiiightly larger than it needed to be
//		int result;
//		if(l->type == 1) result = testSphereInFrustumNearPlane(v, l->pos, l->scale);
//		else result = testBBoxPInFrustumNearPlane(v, l->bboxp);
		int result = testBBoxPInFrustumNearPlane(v, l->bboxp);
		if(!result) continue;

		//todo decide weather the ^2 thing is needed
		//todo also maybe change it to a dist from viewplane
		vec_t dist = vec3distvec(v->pos, l->pos) / (l->scale*l->scale); // lights that are larger will be perferred
		//check weather the light can pass the "pre dist test"
		if(lmaxdist <= dist){
			if(llist.count < max) lmaxdist = dist;
			else continue;
		}
		//add light to list
		llist.count++;
		llist.list = realloc(llist.list, llist.count * sizeof(light_t *));
		llist.list[llist.count-1] = l;

		//add dist to distlist

		llist.dist = realloc(llist.dist, llist.count * sizeof(vec_t));
		llist.dist[llist.count-1] = dist;
		//also keep track of if its completely in frustum
		llist.infrustum = realloc(llist.infrustum, llist.count * sizeof(char));
		if(result == 1) llist.infrustum[llist.count-1] = 1;
		else llist.infrustum[llist.count-1] = 0;
	}
	if(llist.count > max){
		//sort and prune them
		lightHashSortPrune(&llist, lmaxdist, max);
	}
	unsigned int ltcount = llist.count;

	//todo sort shadowed lights as well

	//set things up for shadowmap updates?

	//TODO cache this so i dont have to free every time!
	lightrenderout_t out = {{0, 0}, 	{0, 0}, 	{0, 0}, 	{0, 0}};
	if(ltcount){
		//first	 making them be able to fit ALL of the possible lights, then later going to realloc them to size
		out.lout.list = malloc(ltcount * sizeof(light_t*));
		out.lin.list = malloc(ltcount * sizeof(light_t*));

		//fill my output buffer with the lights
		for(i = 0; i < ltcount; i++){
			if(llist.infrustum[i]) out.lin.list[out.lin.count++] = llist.list[i];
			else out.lout.list[out.lout.count++] = llist.list[i];
		}
		//resize back down for a perfect fit, no moving of mem needed
		if(out.lin.count != ltcount) out.lin.list = realloc(out.lin.list, out.lin.count * sizeof(light_t *));
		if(out.lout.count != ltcount) out.lout.list = realloc(out.lout.list, out.lout.count * sizeof(light_t *));
	}

	if(llist.list) free(llist.list);

	//todo do this with shadow lights as well

	return out;
}


typedef struct sLightPUBOStruct_s {
	GLfloat mvp[16];
	GLfloat mv[16]; //needed?
	GLfloat size; //needed?
	GLfloat x, y, z; //padding for struct
} sLightUBOStruct_t;
typedef struct renderSLightCallbackData_s {
	//todo?
	GLuint shaderprogram;
	unsigned int shaderid;
	unsigned int shaderperm;
	shaderpermutation_t * perm;
	unsigned char numsamples;
	unsigned int modelid;
	unsigned int ubodataoffset;
//	unsigned int vbodataoffset;//?
	viewport_t *v;
	sLightUBOStruct_t light;
} renderSLightCallbackData_t;


void drawSLightOCallback(renderlistitem_t * ilist, unsigned int count){
//	printf("added!\n");

	renderSLightCallbackData_t *d = ilist->data;
	shaderpermutation_t * perm = d->perm;
	if(shader_bindPerm(perm) == 2){
	//TODODODODODO
		viewport_t *v = d->v;
		//also have to set some basic uniforms?
	//	framebuffer_t *df = returnFramebufferById(v->dfbid);
		framebuffer_t *of = returnFramebufferById(v->outfbid);

		glUniform2f(perm->uniscreensizefix, 1.0/of->width, 1.0/of->height);
//		float far = v->far;
//		float near = v->near;
//		glUniform2f(perm->uniscreentodepth, far/(far-near),far*near/(near-far));
		unsigned char numsamples = d->numsamples;
		if(numsamples) glUniform1i(perm->uniint0, numsamples);
	}

	model_t *m = model_returnById(d->modelid);
	vbo_t *v = returnVBOById(m->vbo);

//	states_bindVertexArray(v->vaoid);
	unsigned int mysize = ((count * sizeof(sLightUBOStruct_t)));
//	states_bindBufferRange(GL_UNIFORM_BUFFER, 0, renderqueueuboid, d->ubodataoffset, mysize);
	glstate_t s = {STATESENABLECULLFACE | STATESENABLEBLEND, GL_ONE, GL_ONE, GL_LESS, GL_FRONT, GL_FALSE, GL_LESS, 0.0, v->vaoid, renderqueueuboid, GL_UNIFORM_BUFFER, 0, d->ubodataoffset, mysize, perm->id};
	states_setState(s);
	//states_cullFace(GL_FRONT);
	CHECKGLERROR
	glDrawElementsInstanced(GL_TRIANGLES, v->numfaces * 3, GL_UNSIGNED_INT, 0, count);

	//todo
}
void drawSLightICallback(renderlistitem_t * ilist, unsigned int count){
//	printf("added!\n");

	renderSLightCallbackData_t *d = ilist->data;
	shaderpermutation_t * perm = d->perm;
	if(shader_bindPerm(perm) == 2){
	//TODODODODODO
		viewport_t *v = d->v;
		//also have to set some basic uniforms?
	//	framebuffer_t *df = returnFramebufferById(v->dfbid);
		framebuffer_t *of = returnFramebufferById(v->outfbid);

		glUniform2f(perm->uniscreensizefix, 1.0/of->width, 1.0/of->height);
//		float far = v->far;
//		float near = v->near;
//		glUniform2f(perm->uniscreentodepth, far/(far-near),far*near/(near-far));
		unsigned char numsamples = d->numsamples;
		if(numsamples) glUniform1i(perm->uniint0, numsamples);
	}

	model_t *m = model_returnById(d->modelid);
	vbo_t *v = returnVBOById(m->vbo);

//	states_bindVertexArray(v->vaoid);
	unsigned int mysize = ((count * sizeof(sLightUBOStruct_t)));
//	states_bindBufferRange(GL_UNIFORM_BUFFER, 0, renderqueueuboid, d->ubodataoffset, mysize);
	glstate_t s = {STATESENABLECULLFACE | STATESENABLEBLEND, GL_ONE, GL_ONE, GL_LESS, GL_BACK, GL_FALSE, GL_LESS, 0.0, v->vaoid, renderqueueuboid, GL_UNIFORM_BUFFER, 0, d->ubodataoffset, mysize, perm->id};
	states_setState(s);
	//states_cullFace(GL_FRONT);
	CHECKGLERROR
	glDrawElementsInstanced(GL_TRIANGLES, v->numfaces * 3, GL_UNSIGNED_INT, 0, count);

	//todo
}

void setupSLightCallback(renderlistitem_t * ilist, unsigned int count){
	if(count > 1){
		sLightUBOStruct_t ubodata[MAXINSTANCESIZE];
		unsigned int i = 0;
		while(i < count){
			renderSLightCallbackData_t *d = ilist[i].data;
			unsigned int counter = 0;
			ubodata[0] = d[0].light;
			unsigned int max = count-i;
			if(max > MAXINSTANCESIZE) max = MAXINSTANCESIZE;
			for(counter = 1; counter < max; counter++){
				ubodata[counter] = d[counter].light;
			}
			int t = pushDataToUBOCache(counter * sizeof(sLightUBOStruct_t), ubodata);
			d->ubodataoffset = t;
			ilist[i].counter = counter; // reset counter, likely wont be needed in this case;
			i+=counter;
		}
	} else if(count == 1){
		renderSLightCallbackData_t *d = ilist->data;
		int t = pushDataToUBOCache(sizeof(sLightUBOStruct_t), &d->light);
		d->ubodataoffset = t;
	} else {
		console_printf("ERROR: SLIGHT SETUP CALLBACK WITH 0 AS COUNT!\n");
	}
}




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
//	printf("added!\n");

	renderPLightCallbackData_t *d = ilist->data;
	shaderpermutation_t * perm = d->perm;
	if(shader_bindPerm(perm) == 2){
	//TODODODODODO
		viewport_t *v = d->v;
		//also have to set some basic uniforms?
	//	framebuffer_t *df = returnFramebufferById(v->dfbid);
		framebuffer_t *of = returnFramebufferById(v->outfbid);

		GLfloat mout[16];	//todo calc MOUT per viewport in viewport update
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

	model_t *m = model_returnById(d->modelid);
	vbo_t *v = returnVBOById(m->vbo);

//	states_bindVertexArray(v->vaoid);
	unsigned int mysize = ((count * sizeof(pLightUBOStruct_t)));
//	states_bindBufferRange(GL_UNIFORM_BUFFER, 0, renderqueueuboid, d->ubodataoffset, mysize);
	glstate_t s = {STATESENABLECULLFACE | STATESENABLEBLEND, GL_ONE, GL_ONE, GL_LESS, GL_FRONT, GL_FALSE, GL_LESS, 0.0, v->vaoid, renderqueueuboid, GL_UNIFORM_BUFFER, 0, d->ubodataoffset, mysize, perm->id};
	states_setState(s);
	//states_cullFace(GL_FRONT);
	CHECKGLERROR
	glDrawElementsInstanced(GL_TRIANGLES, v->numfaces * 3, GL_UNSIGNED_INT, 0, count);

	//todo
}
void drawPLightICallback(renderlistitem_t * ilist, unsigned int count){
	renderPLightCallbackData_t *d = ilist->data;
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
		unsigned char numsamples = d->numsamples;
		if(numsamples) glUniform1i(perm->uniint0, numsamples);
	}

	model_t *m = model_returnById(d->modelid);
	vbo_t *v = returnVBOById(m->vbo);

//	states_bindVertexArray(v->vaoid);
	unsigned int mysize = ((count * sizeof(pLightUBOStruct_t)));
//	states_bindBufferRange(GL_UNIFORM_BUFFER, 0, renderqueueuboid, d->ubodataoffset, mysize);
	glstate_t s = {STATESENABLECULLFACE | STATESENABLEBLEND, GL_ONE, GL_ONE, GL_LESS, GL_BACK, GL_FALSE, GL_LESS, 0.0, v->vaoid, renderqueueuboid, GL_UNIFORM_BUFFER, 0, d->ubodataoffset, mysize, perm->id};
	states_setState(s);
//	states_cullFace(GL_BACK);
	CHECKGLERROR
	glDrawElementsInstanced(GL_TRIANGLES, v->numfaces * 3, GL_UNSIGNED_INT, 0, count);

	//todo

}
void setupPLightCallback(renderlistitem_t * ilist, unsigned int count){
	if(count > 1){
		pLightUBOStruct_t ubodata[MAXINSTANCESIZE];
		unsigned int i = 0;
		while(i < count){
			renderPLightCallbackData_t *d = ilist[i].data;
			unsigned int counter = 0;
			ubodata[0] = d[0].light;
			unsigned int max = count-i;
			if(max > MAXINSTANCESIZE) max = MAXINSTANCESIZE;
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

int lights_addToRenderQueue(viewport_t *v, renderqueue_t * q, unsigned int numsamples){
	shaderprogram_t * shader = shader_returnById(lightshaderid);
	unsigned int permutation = 0;
	shaderpermutation_t * perm, *spotperm;

//	framebuffer_t *df = returnFramebufferById(v->dfbid);
//	framebuffer_t *of = returnFramebufferById(v->outfbid);
//	if(!df || !of) return FALSE;

//	unsigned int numsamples = df->rbflags & FRAMEBUFFERRBFLAGSMSCOUNT;

	if(numsamples){
//		numsamples = 1<<numsamples;
//		resolveMultisampleFramebuffer(df); //only resolves if multisampled
//		resolveMultisampleFramebufferSpecify(df, 4);
		permutation = 2;
	}
	perm = shader_addPermutationToProgram(shader, permutation);
	spotperm = shader_addPermutationToProgram(shader, permutation|4);

	lightrenderout_t out = readyLightsForRender(v, 50, 0);
	if(!out.lin.count && !out.lout.count) return FALSE;
//	out.lout.count = 0;
//	if(out.lin.count) out.lin.count = 1;
	int i;
	renderPLightCallbackData_t pl; //i can do this because the data is copied
	pl.modelid = sphereModel;
	pl.shaderid = lightshaderid;
	pl.shaderperm = permutation;
	pl.perm = perm;
	pl.numsamples = numsamples;
	pl.shaderprogram = perm->id;
	pl.v = v;
	renderSLightCallbackData_t sl; //i can do this because the data is copied
	sl.modelid = lightconeModel;
	sl.shaderid = lightshaderid;
	sl.shaderperm = permutation|4;
	sl.perm = spotperm;
	sl.numsamples = numsamples;
	sl.shaderprogram = spotperm->id;
	sl.v = v;

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
	r.flags = 2 | 4; //copyable, instanceable
	for(i = 0; i < out.lin.count; i++){
		//check if its spot or not
		if(out.lin.list[i]->type ==2){
			matrix4x4_t ct;
			Matrix4x4_Concat(&ct, &v->viewproj, &out.lin.list[i]->camproj);
			Matrix4x4_ToArrayFloatGL(&ct, sl.light.mvp);
			sl.light.size = out.lin.list[i]->scale;
			Matrix4x4_Concat(&ct, &v->view, &out.lin.list[i]->camproj);
			Matrix4x4_ToArrayFloatGL(&ct, sl.light.mv);
			r.draw = drawSLightICallback;
			r.setup = setupSLightCallback;
			r.datasize = sizeof(renderSLightCallbackData_t);
			r.data = &sl;

		} else {
			pl.light.size = out.lin.list[i]->scale;
			pl.light.pos[0] = out.lin.list[i]->pos[0];
			pl.light.pos[1] = out.lin.list[i]->pos[1];
			pl.light.pos[2] = out.lin.list[i]->pos[2];
			r.draw = drawPLightICallback;
			r.setup = setupPLightCallback;
			r.datasize = sizeof(renderPLightCallbackData_t);
			r.data = &pl;
		}
		addRenderlistitem(q, r);
	}
	for(i = 0; i < out.lout.count; i++){
		//check if its spot or not
		if(out.lout.list[i]->type ==2){
			matrix4x4_t ct;
			Matrix4x4_Concat(&ct, &v->viewproj, &out.lout.list[i]->camproj);
			Matrix4x4_ToArrayFloatGL(&ct, sl.light.mvp);
			sl.light.size = out.lout.list[i]->scale;
			Matrix4x4_Concat(&ct, &v->view, &out.lout.list[i]->camproj);
			Matrix4x4_ToArrayFloatGL(&ct, sl.light.mv);
			r.draw = drawSLightOCallback;
			r.setup = setupSLightCallback;
			r.datasize = sizeof(renderSLightCallbackData_t);
			r.data = &sl;
		} else {
			pl.light.size = out.lout.list[i]->scale;
			pl.light.pos[0] = out.lout.list[i]->pos[0];
			pl.light.pos[1] = out.lout.list[i]->pos[1];
			pl.light.pos[2] = out.lout.list[i]->pos[2];
			r.draw = drawPLightOCallback;
			r.setup = setupPLightCallback;
			r.datasize = sizeof(renderPLightCallbackData_t);
			r.data = &pl;
		}
		addRenderlistitem(q, r);
	}

	//todo
	//gotta free the list!
	if(out.lout.list) free(out.lout.list);
	if(out.lin.list) free(out.lin.list);
	return TRUE;
}
