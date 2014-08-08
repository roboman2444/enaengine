#include <GL/glew.h>
#include <GL/gl.h>

#include "globaldefs.h"
#include "matrixlib.h"
#include "worldmanager.h"
#include "modelmanager.h"
#include "mathlib.h"
#include "entitymanager.h"
#include "console.h"

#include "texturemanager.h"
#include "shadermanager.h"

int worldOK = 0;
worldleaf_t * worldroot;
unsigned int worldNumObjects = 0;
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
	unsigned int version;
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
	char flags;
}worldFileObject_t;

int saveWorldPopList(int * count, worldobject_t ** list, worldleaf_t * leaf){

	if(!leaf) return FALSE;
//	consolePrintf("count: %i\n", *count);

	int i;
	for(i = 0; i < leaf->numobjects && *count < worldNumObjects; i++){
		if(!leaf->list[i].modelid)continue;
		list[*count] = &leaf->list[i];
		*count = *count+1;
	}
	for(i = 0; i < 4; i++){
		saveWorldPopList(count, list, leaf->children[i]);
	}
	return TRUE;
}

int saveWorld(char * filename){
	consolePrintf("Saving %i objects to %s\n", worldNumObjects, filename);
	if(!worldroot){
		consolePrintf("ERROR: No root world to save!\n");
		return FALSE;
	}
	if(!worldNumObjects){
		consolePrintf("ERROR: No world objects to save!\n");
		return FALSE;
	}
	FILE *f = fopen(filename,"wb");
	if(!f){
		consolePrintf("ERROR: could not open file %s for writing\n", filename);
		return FALSE;
	}
	worldFileHeader_t header;
	header.version = 2;
	header.objectlistcount = worldNumObjects;
	worldobject_t ** worldlist = malloc(worldNumObjects * sizeof(worldobject_t *));
	if(!worldlist){
		return FALSE;
	}
	//populate the list
	int count = 0;
	saveWorldPopList(&count, worldlist, worldroot);
	if(count != worldNumObjects){
		free(worldlist);
		consolePrintf("ERROR: leaflist prop failed\n");
		return FALSE;
	}
	worldFileObject_t * objlist = malloc(count*sizeof(worldFileObject_t));
	memset(objlist, 0, sizeof(worldFileObject_t));

	int modelcount = 0;
	int texturecount = 0;
	int shadercount = 0;
	int *modellist = 0;
	int *shaderlist = 0;
	int *texturelist = 0;
	int i = 0;
	for(i = 0; i < count; i++){
//		consolePrintf("count: %i i: %i\n", count, i);

		worldobject_t * testobj = worldlist[i];
		objlist[i].mat = testobj->mat;
		//do model texture indice stuff here
		if(testobj->modelid){
			int j;
			for(j = 0; j < modelcount; j++){
				if(testobj->modelid == modellist[j]){
					objlist[i].modelindice = j+1;
					break;
				}
			}
			if(j == modelcount){
				modelcount++;
				modellist = realloc(modellist, modelcount * sizeof(int));
				modellist[j] = testobj->modelid;
				objlist[i].modelindice = modelcount;
			}
		} else {
			objlist[i].modelindice = 0;
		}
		if(testobj->texturegroupid){
			int j;
			for(j = 0; j < texturecount; j++){
				if(testobj->texturegroupid == texturelist[j]){
					objlist[i].textureindice = j+1;
					break;
				}
			}
			if(j == texturecount){
				texturecount++;
				texturelist = realloc(texturelist, texturecount * sizeof(int));
				texturelist[j] = testobj->texturegroupid;
				objlist[i].textureindice = texturecount;
			}
		} else {
			objlist[i].textureindice = 0;
		}
		if(testobj->shaderid){
			int j;
			for(j = 0; j < shadercount; j++){
				if(testobj->shaderid == shaderlist[j]){
					objlist[i].shaderindice = j+1;
					break;
				}
			}
			if(j == shadercount){
				shadercount++;
				shaderlist = realloc(shaderlist, shadercount * sizeof(int));
				shaderlist[j] = testobj->shaderid;
				objlist[i].shaderindice = shadercount;
			}
			objlist[i].shaderperm = testobj->shaderperm;
		} else {
			objlist[i].shaderindice = 0;
			objlist[i].shaderperm = 0;
		}
		objlist[i].flags = testobj->flags;
	}
	free(worldlist); // dont need anymore
	header.objectlistcount = count;
	header.modellistcount = modelcount;
	header.texturelistcount = texturecount;
	header.shaderlistcount = shadercount;
	header.modellistlength = 0;
	header.shaderlistlength = 0;
	header.texturelistlength = 0;

	char *modelliststring = 0;
	char *shaderliststring = 0;
	char *textureliststring = 0;

	for(i = 0; i < header.modellistcount; i++){
		model_t * m = returnModelById(modellist[i]);
		if(!m) continue;
		int jlen, tlen = header.modellistlength;
		header.modellistlength += strlen(m->name)+1;
		modelliststring = realloc(modelliststring, header.modellistlength);
		if(i){
			//manual concat ftw
			for(jlen = 0; tlen < header.modellistlength; tlen++, jlen++){
				modelliststring[tlen] = m->name[jlen];
			}
//			sprintf(modelliststring, "%s\0%s", modelliststring, m->name);
//			strcat(modelliststring, m->name);
		} else {
			strcpy(modelliststring, m->name);
		}
	}
	for(i = 0; i < header.texturelistcount; i++){
		texturegroup_t * t = returnTexturegroupById(texturelist[i]);
		if(!t) continue;
		int jlen, tlen = header.texturelistlength;
		header.texturelistlength += strlen(t->name)+1;
		textureliststring = realloc(textureliststring, header.texturelistlength);
		if(i){
			for(jlen = 0; tlen < header.texturelistlength; tlen++, jlen++){
				textureliststring[tlen] = t->name[jlen];
			}

//			sprintf(textureliststring, "%s\0%s", textureliststring, t->name);
//			strcat(textureliststring, t->name);
		} else {
			strcpy(textureliststring, t->name);
		}

	}
	for(i = 0; i < header.shaderlistcount; i++){
		shaderprogram_t * s = returnShaderById(shaderlist[i]);
		if(!s) continue;
		int jlen, tlen = header.shaderlistlength;
		header.shaderlistlength += strlen(s->name)+1;
		shaderliststring = realloc(shaderliststring, header.shaderlistlength);
		if(i){
			for(jlen = 0; tlen < header.shaderlistlength; tlen++, jlen++){
				shaderliststring[tlen] = s->name[jlen];
			}

//			sprintf(shaderliststring, "%s\0%s", shaderliststring, s->name);
//			strcat(shaderliststring, s->name);
		} else {
			strcpy(shaderliststring, s->name);
		}


	}

	header.filesize = header.shaderlistlength + header.modellistlength + header.texturelistlength + (header.objectlistcount * sizeof(worldFileObject_t));

//	now i can start to write
	fwrite(&header, 1, sizeof(worldFileHeader_t), f);
	fwrite(modelliststring, 1, header.modellistlength, f);
	fwrite(textureliststring, 1, header.texturelistlength, f);
	fwrite(shaderliststring, 1, header.shaderlistlength, f);
	fwrite(objlist, 1, header.objectlistcount * sizeof(worldFileObject_t), f);


	if(modelliststring)free(modelliststring);
	if(textureliststring)free(textureliststring);
	if(shaderliststring)free(shaderliststring);
	free(objlist);
	fclose(f);
	return header.filesize;
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



char **loadWorldNameList(char *buf, unsigned int bc, unsigned int nc){
	char  **namelist = malloc(nc * sizeof(char *));
	unsigned int namei, bytei = 0;
	for(namei = 0; namei < nc; namei++){
		namelist[namei] = buf+bytei;
/*		while(namelist[bytei]){
			bytei++; // look for next /0
			if(bytei > bc){
				free(namelist);
				return FALSE;
			}
		}
*/
		while(buf[bytei]){
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
int loadWorld(char * filename){
	FILE *f = fopen(filename, "rb");
	if(!f){
		consolePrintf("opening file:%s failed\n", filename);
		return FALSE;
	}
	worldFileHeader_t header;

	char **modelnamelist = 0;
	char **texturenamelist = 0;
	char **shadernamelist = 0;
	char *buf = 0;

	if(fread(&header, 1, sizeof(header), f) != sizeof(header))goto error;
	if(header.version != 2)goto error; //todo different version handles
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

		if(!modelindice) obj->modelid = 0;
		else obj->modelid = createAndAddModelRINT(modelnamelist[modelindice-1]);
		if(!textureindice) obj->texturegroupid = 0;
		else obj->texturegroupid = createAndAddTexturegroupRINT(texturenamelist[textureindice-1]);
		if(!shaderindice) obj->shaderid = 0;
		else obj->shaderid = createAndAddShaderRINT(shadernamelist[shaderindice-1]);
		obj->shaderperm = objbuf[i].shaderperm;
		recalcObjBBox(obj);
		obj->flags = objbuf[i].flags;
		obj->status = 1;
		addObjectToWorld(obj);
	}
	if(obj) free(obj);

	free(shadernamelist);
	free(texturenamelist);
	free(modelnamelist);
	free(buf);
	fclose(f);
	//maybe check if worldNumObjects = header.objectlistcount...
	consolePrintf("%i objects loaded from %s\n", header.objectlistcount, filename);
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
//	leaf->bbox[0] = center[0];
//	leaf->bbox[1] = center[0];
//	leaf->bbox[4] = center[1];
//	leaf->bbox[5] = center[1];
	leaf->bbox[0] = center[0] + leaf->size;
	leaf->bbox[1] = center[0] - leaf->size;
	leaf->bbox[4] = center[1] + leaf->size;
	leaf->bbox[5] = center[1] - leaf->size;
	leaf->bbox[2] = 0.0;
	leaf->bbox[3] = 0.0;
	getBBoxpFromBBox(leaf->bbox, leaf->bboxp);
	//todo
	return leaf;
}

int initWorldSystem(void){
//todo
	vec2_t center = {0.0, 0.0};
	worldroot = createWorldLeaf(0, center);
	if(!worldroot) return FALSE;
	worldOK = TRUE;
	return TRUE;
}
int walkAndDeleteObject(worldleaf_t * l, worldobject_t * o){
	if(o->treedepth > l->treedepth){
		int xspace = FALSE;
		int yspace = FALSE;
		if(o->pos[0] > l->center[0]) xspace = TRUE;
		if(o->pos[2] > l->center[1]) yspace = TRUE;
		worldleaf_t * child = l->children[xspace + 2*yspace];
		if(!child) return FALSE;
		int r = walkAndDeleteObject(child, o);
		if(!child->numobjects){
			int c;
			for(c = 0; c < 4 && child->children[c]; c++); // make sure all are null
			if(c == 4) free(child); //easier than doing the normal delete
		}
		if(r>1){
			r = 0;
			int i;
			for(i = 0; i < 6; i++){
				if(child->bbox[i] == l->bbox[i]) r |= 2<<i;
			}
			if(r){
				//todo
				//recalc bbox
				return r;
			} else {
				return TRUE;
			}
		}
		return r;
	} else if(o->treedepth == l->treedepth){
		int arraypos = o->leafpos;
		if(arraypos >= l->numobjects) return FALSE;
		worldobject_t * listobj = &l->list[arraypos];
		//check to make sure they match, either if the pointers are the same, or if the data match
		if(o != listobj && memcmp(o, listobj, sizeof(worldobject_t))){
			return FALSE;
		}
		// in leaf containing object
		int i, r = 0;
		//check if this object is the edge of the bbox
		for(i = 0; i < 6; i++){
			if(o->bbox[i] == l->bbox[i]) r |= 2<<i;
		}
		l->numobjects--;
		*listobj = l->list[l->numobjects]; // replace it with the one at the end
		l->list = realloc(l->list, l->numobjects * sizeof(worldobject_t)); //resize array
		if(r){
			//todo
			// recalc bbox
		}
		worldNumObjects--;
		return r;

	//todo
	}
	return FALSE;
}
int deleteObject(worldobject_t * o){
	if(!o) return FALSE;
	return walkAndDeleteObject(worldroot, o);
}

worldleaf_t * walkAndFindObject(worldleaf_t * l, worldobject_t * o){
	if(o->treedepth > l->treedepth){
		int xspace = FALSE;
		int yspace = FALSE;
		if(o->pos[0] > l->center[0]) xspace = TRUE;
		if(o->pos[2] > l->center[1]) yspace = TRUE;
		int intspace = xspace + 2*yspace;
		if(l->children[intspace]){
			return walkAndFindObject(l->children[intspace], o);
		}
	} else if(o->treedepth == l->treedepth){
		//i guess i can just return the leaf now... it SHOULD be the leaf containing the obj
		return l;
	}
	return FALSE;
}
worldleaf_t * findObject(worldobject_t * o){
	if(!o) return FALSE;
	return walkAndFindObject(worldroot, o);
}
int deleteLeaf(worldleaf_t *l){
	if(!l) return FALSE;
	int i, count = 1;
	for(i = 0; i < 4; i++){
		count += deleteLeaf(l->children[i]);
	}
	if(l->list)free(l->list);
	worldNumObjects -= l->numobjects;
	free(l);
	return(count);
}
int deleteWorld(void){
	int leafcount = deleteLeaf(worldroot);
	worldroot = 0;
	worldNumObjects = 0;
	if(!initWorldSystem())return FALSE;
	return leafcount;
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
//		if(o->bbox[0] > l->bbox[0])l->bbox[0] = o->bbox[0];
//		if(o->bbox[1] < l->bbox[1])l->bbox[1] = o->bbox[1];
		if(o->bbox[2] > l->bbox[2])l->bbox[2] = o->bbox[2];
		if(o->bbox[3] < l->bbox[3])l->bbox[3] = o->bbox[3];
//		if(o->bbox[4] > l->bbox[4])l->bbox[4] = o->bbox[4];
//		if(o->bbox[5] < l->bbox[5])l->bbox[5] = o->bbox[5];
	}
	getBBoxpFromBBox(l->bbox, l->bboxp);
	o->treedepth = l->treedepth;
	o->leafpos = l->numobjects;
	l->numobjects++;
	l->list = realloc(l->list, l->numobjects * sizeof(worldobject_t));
	l->list[l->numobjects-1] = *o;
	worldNumObjects++;
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
//		if(l->bbox[0] < l->children[intspace]->bbox[0]) l->bbox[0] = l->children[intspace]->bbox[0];
//		if(l->bbox[1] > l->children[intspace]->bbox[1]) l->bbox[1] = l->children[intspace]->bbox[1];
		if(l->bbox[2] < l->children[intspace]->bbox[2]) l->bbox[2] = l->children[intspace]->bbox[2];
		if(l->bbox[3] > l->children[intspace]->bbox[3]) l->bbox[3] = l->children[intspace]->bbox[3];
//		if(l->bbox[4] < l->children[intspace]->bbox[4]) l->bbox[4] = l->children[intspace]->bbox[4];
//		if(l->bbox[5] > l->children[intspace]->bbox[5]) l->bbox[5] = l->children[intspace]->bbox[5];
		//should recalc bboxp now
		getBBoxpFromBBox(l->bbox, l->bboxp);
		//todo i can make a more efficient way of doing this, only updating the y values of the points

		return 2;
	}
	return FALSE; // should never hit
}
int addObjectToWorld(worldobject_t * o){
	model_t * m = returnModelById(o->modelid);
	if(!m) return FALSE;
	if(!m->vbo) return FALSE;
//	int vertcount = m->numverts;
//	if(!vertcount)return FALSE;
	//walk tree and add
	return walkAndAddObject(o, worldroot);
}
int addEntityToWorld(int entityid){
	entity_t *e = returnEntityById(entityid);
	if(!e) return FALSE;

	model_t *m = returnModelById(e->modelid);
	if(!m) return FALSE;
	if(!m->vbo) return FALSE;
//	if(!m->interleaveddata) return FALSE;
//	if(!m->numverts) return FALSE;

	worldobject_t * obj = malloc(sizeof(worldobject_t));
	memset(obj, 0 , sizeof(worldobject_t));
	obj->mat = e->mat;
//	Matrix4x4_OriginFromMatrix(&obj->mat, obj->pos);
	Matrix4x4_OriginFromMatrix(&e->mat, obj->pos); // uh... this should fix SOMETHING
	obj->modelid = e->modelid;
	obj->texturegroupid = e->texturegroupid;
	obj->shaderid = e->shaderid;
	obj->shaderperm = e->shaderperm;
	memcpy(obj->bbox, e->bbox, 6 * sizeof(vec_t));
	memcpy(obj->bboxp, e->bboxp, 24 * sizeof(vec_t));
	obj->status = 1;

	int returnval = walkAndAddObject(obj, worldroot);
	free(obj);
	return returnval;
//	return TRUE;
}

