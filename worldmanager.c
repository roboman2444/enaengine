#include <GL/glew.h>
#include <GL/gl.h>

#include "globaldefs.h"
#include "matrixlib.h"
#include "worldmanager.h"
#include "modelmanager.h"
#include "mathlib.h"
#include "entitymanager.h"
#include "console.h"

extern int createAndAddTexturegroupRINT(char * name);
extern int createAndAddShaderRINT(char * name);

int worldOK = 0;
worldleaf_t * root;
/* file structure description
	header
	model list
		just a huge string pretty much. Each name of is seperated by a \0.
	texture list
		just a huge string pretty much. Each name of is seperated by a \0.
	shader list
		just a huge string pretty much. Each name of is seperated by a \0.
	object list
		array of the worldFileObject_t struct
*/
typedef struct worldFileHeader_s {
	unsigned int filesize;
	unsigned int modellistlength;  // bytes
	unsigned int modellistcount;   // count
	unsigned int texturelistlength;// bytes
	unsigned int texturelistcount; // count
	unsigned int shaderlistlength; // bytes
	unsigned int shaderlistcount;  // count
	unsigned int objectlistcount; // count
}worldFileHeader_t;
typedef struct worldFileObject_s {
	matrix4x4_t mat;
	unsigned int modelindice;
	unsigned int textureindice;
	unsigned int shaderindice;
	int shaderperm;
}worldFileObject_t;
/*
typedef struct worldTextureQueue_s {
	int texid;
	worldobject_t * list;
} worldTextureQueue_t;
*/
char **loadWorldNameList(char *buf, unsigned int bc, unsigned int nc){
	char  **namelist = malloc(nc * sizeof(char *));
	unsigned int namei, bytei = 0;
	for(namei = 0; namei < nc; namei++){
		namelist[namei] = buf+bytei;
		while(namelist[bytei]){
			bytei++; // look for next /0
			if(bytei > bc){
				free(namelist);
				return FALSE;
			}
		}
		bytei++; //bump forward once
	}
	return namelist;
}
int recalcObjBBox(worldobject_t *o){
	model_t * m = returnModelById(o->modelid);
	if(!m) return FALSE;
	int i;
	o->bbox[0] = -3.4028e+38;
	o->bbox[1] = 3.4028e+38;
	o->bbox[2] = -3.4028e+38;
	o->bbox[3] = 3.4028e+38;
	o->bbox[4] = -3.4028e+38;
	o->bbox[5] = 3.4028e+38;

	for(i = 0; i < 8; i++){
		int oneplace = i*3;
		Matrix4x4_Transform(&o->mat, &m->bboxp[oneplace], &o->bboxp[oneplace]);

		if(o->bboxp[oneplace] > o->bbox[0]) o->bbox[0] = o->bboxp[oneplace];
		else if(o->bboxp[oneplace] < o->bbox[1]) o->bbox[1] = o->bboxp[oneplace];
		if(o->bboxp[oneplace+1] > o->bbox[2]) o->bbox[2] = o->bboxp[oneplace+1];
		else if(o->bboxp[oneplace+1] < o->bbox[3]) o->bbox[3] = o->bboxp[oneplace+1];
		if(o->bboxp[oneplace+2] > o->bbox[4]) o->bbox[4] = o->bboxp[oneplace+2];
		else if(o->bboxp[oneplace+2] < o->bbox[5]) o->bbox[5] = o->bboxp[oneplace+2];
	}
	return TRUE;
}
int loadWorld(char * filename){
	FILE *f = fopen(filename, "rb");
	if(!f) return FALSE;
	worldFileHeader_t header;

	char **modelnamelist = 0;
	char **texturenamelist = 0;
	char **shadernamelist = 0;
	char *buf = 0;

	if(fread(&header, 1, sizeof(header), f) != sizeof(header))goto error;
	if(header.shaderlistlength + header.modellistlength + header.texturelistlength + (header.objectlistcount * sizeof(worldFileObject_t)) != header.filesize) goto error;

	buf = malloc(header.filesize);
	if(fread(buf, 1, header.filesize, f) != header.filesize) goto error;
	char * newbuf = buf;

	modelnamelist = loadWorldNameList(newbuf, header.modellistlength, header.modellistcount);
	if(!modelnamelist) goto error;
	newbuf += header.modellistlength;

	texturenamelist = loadWorldNameList(newbuf, header.texturelistlength, header.texturelistcount);
	if(!texturenamelist) goto error;
	newbuf += header.texturelistlength;


	shadernamelist = loadWorldNameList(newbuf, header.shaderlistlength, header.shaderlistcount);
	if(!shadernamelist) goto error;
	newbuf += header.shaderlistlength;

	//newbuf is now the start of the actual object list
	//todo parse it out

	worldFileObject_t * objbuf = (worldFileObject_t *)newbuf;
	worldobject_t * obj = malloc(sizeof(worldobject_t));

	int i;
	for(i = 0; i < header.objectlistcount; i++){
		obj->mat = objbuf[i].mat;
		Matrix4x4_OriginFromMatrix(&obj->mat, obj->pos);

		int modelindice = objbuf[i].modelindice;
		if(modelindice > header.modellistcount||!modelindice)continue;

		int textureindice = objbuf[i].textureindice;
		if(textureindice > header.texturelistcount)continue;

		int shaderindice = objbuf[i].shaderindice;
		if(shaderindice > header.shaderlistcount)continue;

		else obj->modelid = createAndAddModelRINT(modelnamelist[modelindice-1]);
		if(!textureindice) obj->textureid = 0;
		else obj->textureid = createAndAddTexturegroupRINT(texturenamelist[textureindice-1]);
		if(!shaderindice) obj->shaderid = 0;
		else obj->shaderid = createAndAddShaderRINT(modelnamelist[shaderindice-1]);
		obj->shaderperm = objbuf[i].shaderperm;
		recalcObjBBox(obj);
		obj->status = 1;
		addObjectToWorld(obj);
	}
	if(obj) free(obj);

	free(shadernamelist);
	free(texturenamelist);
	free(modelnamelist);
	free(buf);
	fclose(f);
	return TRUE;

	error:
		fclose(f);
		if(buf) free(buf);
		if(modelnamelist) free(modelnamelist);
		if(texturenamelist) free(texturenamelist);
		if(shadernamelist) free(shadernamelist);
		consolePrintf("%s: error while loading world\n", filename);
	return FALSE;
}
worldleaf_t * createWorldLeaf(int depth, vec2_t center){
	worldleaf_t * leaf = malloc(sizeof(worldleaf_t));
	memset(leaf, 0 , sizeof(worldleaf_t));
	leaf->treedepth = depth;
	leaf->center[0] = center[0];
	leaf->center[1] = center[1];
	leaf->size = (float)WORLDMAXSIZE / (float)(2<<depth);
	leaf->bbox[0] = center[0] + leaf->size;
	leaf->bbox[1] = center[0] - leaf->size;
	leaf->bbox[2] = 0.0;
	leaf->bbox[3] = 0.0;
	leaf->bbox[4] = center[1] + leaf->size;
	leaf->bbox[5] = center[1] - leaf->size;
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
	if(!l->numobjects){
//		l->bbox[0] = o->bbox[0];
//		l->bbox[1] = o->bbox[1];
		l->bbox[2] = o->bbox[2];
		l->bbox[3] = o->bbox[3];
//		l->bbox[4] = o->bbox[4];
//		l->bbox[5] = o->bbox[5];
	} else {
		if(o->bbox[2] > l->bbox[2]){
			l->bbox[2] = o->bbox[2];
		}
		if(o->bbox[3] < l->bbox[3]){
			l->bbox[3] = o->bbox[3];
		}
	}
	getBBoxpFromBBox(l->bbox, l->bboxp);
	l->numobjects++;
	l->list = realloc(l->list, l->numobjects * sizeof(worldobject_t));
	l->list[l->numobjects-1] = *o;
//	free(o);
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
//		if(l->bbox[0] < l->children[intspace]->bbox[0]) consolePrintf("bboxbig\n");
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
	Matrix4x4_OriginFromMatrix(&obj->mat, obj->pos);
	obj->modelid = e->modelid;
	obj->textureid = e->texturegroupid;
	obj->shaderid = e->shaderid;
	obj->shaderperm = e->shaderperm;
	memcpy(obj->bbox, e->bbox, 6 * sizeof(vec_t));
	memcpy(obj->bboxp, e->bboxp, 24 * sizeof(vec_t));
	obj->status = 1;

	int returnval = addObjectToWorld(obj);
	free(obj);
	return returnval;
//	return TRUE;
}

