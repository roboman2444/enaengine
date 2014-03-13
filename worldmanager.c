#include <GL/glew.h>
#include <GL/gl.h>

#include "globaldefs.h"
#include "matrixlib.h"
#include "worldmanager.h"
#include "modelmanager.h"
#include "mathlib.h"
#include "entitymanager.h"


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
	//todo make bboxp
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
		} else {
			walkAndAddObject(o, l->children[intspace]);
		}
		//should figure out top and bottom bounds now...
		if(l->bbox[2] < l->children[intspace]->bbox[2]) l->bbox[2] = l->children[intspace]->bbox[2];
		if(l->bbox[3] > l->children[intspace]->bbox[3]) l->bbox[3] = l->children[intspace]->bbox[3];
		//should recalc bboxp now

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
	return walkAndAddObject(o, root);
/*
	o->interleaveddata = malloc(vertcount * 8 * sizeof(GLfloat));
	int mstride = m->stride;
	int i;
	//i need a rotation matrix for surface normals
	matrix4x4_t rotmat;
	Matrix4x4_CopyRotateOnly(&rotmat, &o->mat);

	o->bbox[0] = -3.4028e+38;
	o->bbox[1] = 3.4028e+38;
	o->bbox[2] = -3.4028e+38;
	o->bbox[3] = 3.4028e+38;
	o->bbox[4] = -3.4028e+38;
	o->bbox[5] = 3.4028e+38;
	for(i = 0; i < vertcount; i++){
		int oneplace = i*8;
		int twoplace = i*mstride;
		//transform points
		Matrix4x4_Transform(&o->mat,&m->interleaveddata[twoplace], &o->interleaveddata[oneplace]);
		//rotate normals
		Matrix4x4_Transform(&rotmat,&m->interleaveddata[twoplace+3], &o->interleaveddata[oneplace+3]);
		//copy texcoords
		if(mstride>5){
			o->interleaveddata[oneplace+6] = m->interleaveddata[twoplace+6];
			o->interleaveddata[oneplace+7] = m->interleaveddata[twoplace+7];
		} else {
			o->interleaveddata[oneplace+6] = 0.0;
			o->interleaveddata[oneplace+7] = 0.0;

		}

		//find bbox
		if(o->interleaveddata[oneplace] > o->bbox[0]) o->bbox[0] = o->interleaveddata[oneplace];
		else if(o->interleaveddata[oneplace] < o->bbox[1]) o->bbox[1] = o->interleaveddata[oneplace];
		if(o->interleaveddata[oneplace+1] > o->bbox[2]) o->bbox[2] = o->interleaveddata[oneplace+1];
		else if(o->interleaveddata[oneplace+1] < o->bbox[3]) o->bbox[3] = o->interleaveddata[oneplace+1];
		if(o->interleaveddata[oneplace+2] > o->bbox[4]) o->bbox[4] = o->interleaveddata[oneplace+2];
		else if(o->interleaveddata[oneplace+2] < o->bbox[5]) o->bbox[5] = o->interleaveddata[oneplace+2];

	}
	//construct bpoints
	for(i = 0; i < 8; i++){
		o->bboxp[(i*3)+0] = o->bbox[(i&1)+0];
		o->bboxp[(i*3)+1] = o->bbox[((i&2)>>1)+2];
		o->bboxp[(i*3)+2] = o->bbox[((i&4)>>2)+4];
	}
	o->status = 2;
*/
	//walk tree and add
	return TRUE;
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
	obj->status = 1;

	return addObjectToWorld(obj);
//	return TRUE;
}

