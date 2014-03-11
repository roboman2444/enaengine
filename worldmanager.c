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

int initWorldSystem(void){
	root = malloc(sizeof(worldleaf_t));
	memset(root, 0 , sizeof(worldleaf_t));
	root->height = 3.4028e+38;
	root->negheight = -3.4028e+38;
//todo
	return TRUE;
}
int addObjectToWorld(worldobject_t * o){
	//transform points
	model_t * m = returnModelById(o->modelid);
	if(!m) return FALSE;
	int vertcount = m->numverts;
	if(!vertcount) return FALSE;
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
		o->bboxp[(i*3)+1] = o->bbox[(i&2)+2];
		o->bboxp[(i*3)+2] = o->bbox[(i&4)+4];
	}
	o->status = 2;

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
	Matrix4x4_OriginFromMatrix(&e->mat, obj->pos);
	obj->status = 1;

	//transform all the models verts
	//generate model bbox



	return addObjectToWorld(obj);
	return TRUE;
}

