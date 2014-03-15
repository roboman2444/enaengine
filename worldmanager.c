#include <GL/glew.h>
#include <GL/gl.h>

#include "globaldefs.h"
#include "matrixlib.h"
#include "worldmanager.h"
#include "modelmanager.h"
#include "mathlib.h"
#include "entitymanager.h"
#include "console.h"


int worldOK = 0;
worldleaf_t * root;

typedef struct worldTextureQueue_s {
	int texid;
	worldobject_t * list;
} worldTextureQueue_t;

worldleaf_t * createWorldLeaf(int depth, vec2_t center){
	worldleaf_t * leaf = malloc(sizeof(worldleaf_t));
	memset(leaf, 0 , sizeof(worldleaf_t));
	leaf->treedepth = depth;
	leaf->center[0] = center[0];
	leaf->center[1] = center[1];
	leaf->size = (float)WORLDMAXSIZE / (float)(depth * depth);
	leaf->bbox[0] = center[0] + leaf->size;
	leaf->bbox[1] = center[0] + leaf->size;
	leaf->bbox[2] = 0.0;
	leaf->bbox[3] = 0.0;
	leaf->bbox[4] = center[1] + leaf->size;
	leaf->bbox[5] = center[1] + leaf->size;
	getBBoxpFromBBox(leaf->bbox, leaf->bboxp);
	//todo
	return leaf;
}

int initWorldSystem(void){
//todo
	vec2_t center = {0.0, 0.0};
	root = createWorldLeaf(0, center);
	if(!root) return FALSE;
	worldOK = TRUE;
	return TRUE;
}
int addObjectToLeaf(worldobject_t * o, worldleaf_t *l){
	//todo
	//if first object in, set anyway
	if(!l->numObjects){
		l->bbox[2] = o->bbox[2];
		l->bbox[3] = o->bbox[3];
	} else {
		if(o->bbox[2] > l->bbox[2]){
			l->bbox[2] = o->bbox[2];
		}
		if(o->bbox[3] < l->bbox[3]){
			l->bbox[3] = o->bbox[3];
		}
	}
	//todo recalcbboxp
	l->numObjects++;
	l->list = realloc(l->list, l->numObjects * sizeof(worldobject_t));
	l->list[l->numObjects-1] = *o;
	free(o);
	return TRUE;
}

int walkAndAddObject(worldobject_t * o, worldleaf_t * l){
	char xspace = 0;
	char yspace = 0;
	char nofits = 0;
	if(l->treedepth >= WORLDTREEDEPTH) nofits = TRUE;
	//find possible space
	if(o->bbox[0] > l->center[0]) xspace = TRUE;
	if(o->bbox[4] > l->center[1]) yspace = TRUE;

	//now check the other bounds, make sure its the same as the origional
	if((o->bbox[1] > l->center[0]) != xspace) nofits = TRUE;
	if((o->bbox[5] > l->center[1]) != yspace) nofits = TRUE;
	if(nofits){
		//todo
		addObjectToLeaf(o, l);
		return TRUE;
	} else {
		int intspace = xspace + 2*yspace;
		if(!l->children[intspace]){
			vec2_t newcenter;
			newcenter[0] = l->center[0] + ((float)xspace-0.5)*l->size;
			newcenter[1] = l->center[1] + ((float)yspace-0.5)*l->size;
			l->children[intspace] = createWorldLeaf(l->treedepth + 1, newcenter);
			walkAndAddObject(o, l->children[intspace]);
		} else {
			walkAndAddObject(o, l->children[intspace]);
		}
		//should figure out top and bottom bounds now...
		if(l->bbox[2] < l->children[intspace]->bbox[2]) l->bbox[2] = l->children[intspace]->bbox[2];
		if(l->bbox[3] > l->children[intspace]->bbox[3]) l->bbox[3] = l->children[intspace]->bbox[3];
		//should recalc bboxp now
		getBBoxpFromBBox(l->bbox, l->bboxp);
		//todo i can make a more efficient way of doing this, only updating the y values of the points

		return 2;
	}
	return FALSE; // should never hit
}
int addObjectToWorld(worldobject_t * o){
	//transform points
	model_t * m = returnModelById(o->modelid);
	if(!m) return FALSE;
	int vertcount = m->numverts;
	if(!vertcount) return FALSE;
	//walk tree and add
	return walkAndAddObject(o, root);
}
int addEntityToWorld(int entityid){
	entity_t *e = returnEntityById(entityid);
	if(!e) return FALSE;

	model_t *m = returnModelById(e->modelid);
	if(!m) return FALSE;
	if(!m->interleaveddata) return FALSE;
	if(!m->numverts) return FALSE;

	worldobject_t * obj = malloc(sizeof(worldobject_t));
	memset(obj, 0 , sizeof(worldobject_t));
	obj->mat = e->mat;
	obj->modelid = e->modelid;
	obj->textureid = e->texturegroupid;
	obj->shaderid = e->shaderid;
	obj->shaderperm = e->shaderperm;
	memcpy(obj->bbox, e->bbox, 6 * sizeof(vec_t));
	memcpy(obj->bboxp, e->bboxp, 24 * sizeof(vec_t));
	obj->status = 1;

	return addObjectToWorld(obj);
//	return TRUE;
}

