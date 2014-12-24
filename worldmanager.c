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

#define WORLDFILEVERSION 3

int worldOK = 0;
worldleaf_t * worldroot;
unsigned int world_numObjects = 0;
unsigned int world_numEnts = 0;
unsigned int world_numLights = 0;
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
	unsigned int filesize;		//bytes
	unsigned int modellistlength;	// bytes
	unsigned int modellistcount;	// count
	unsigned int texturelistlength;	// bytes
	unsigned int texturelistcount;	// count
	unsigned int shaderlistlength;	// bytes
	unsigned int shaderlistcount;	// count
	unsigned int objectlistcount;	// count
	unsigned int entitylistcount;	// count
	unsigned int lightlistcount;	// count
}worldFileHeader_t;
typedef struct worldFileHeaderv2_s {
	unsigned int version;
	unsigned int filesize;		//bytes
	unsigned int modellistlength;	// bytes
	unsigned int modellistcount;	// count
	unsigned int texturelistlength;	// bytes
	unsigned int texturelistcount;	// count
	unsigned int shaderlistlength;	// bytes
	unsigned int shaderlistcount;	// count
	unsigned int objectlistcount;	// count
}worldFileHeaderv2_t;
typedef struct worldFileObject_s {
	matrix4x4_t mat;
	unsigned int modelindice;
	unsigned int textureindice;
	unsigned int shaderindice;
	int shaderperm;
	char flags;
}worldFileObject_t;
typedef struct worldFileEntity_s {
	matrix4x4_t mat;
	unsigned int modelindice;
	unsigned int textureindice;
	unsigned int shaderindice;
	int shaderperm;
	char flags;
	//todo
}worldFileEntity_t;
typedef struct worldFileLight_s {
	matrix4x4_t mat;
	unsigned int modelindice;
	unsigned int textureindice;
	unsigned int shaderindice;
	int shaderperm;
	char flags;
	//todo
}worldFileLight_t;

//todo gotta fix the list gettin!
int saveWorldPopList(int * count, worldobject_t ** outlist, worldleaf_t * leaf){

	if(!leaf) return FALSE;
//	console_printf("count: %i\n", *count);


	//WARNING: revisions untested!
	int i;
	if(leaf->numobjects){
		int myarraysize = leaf->objectarraysize;
		worldobject_t * list = leaf->list;
		for(i = 0; i < myarraysize && *count < world_numObjects; i++){
//			if(!list[i].modelid)continue; //had to remove because of "invisible" objects
			if(!list[i].leaf)continue; //had to remove because of "invisible" objects
			outlist[*count] = &list[i];
			*count = *count+1;
		}
	}
	for(i = 0; i < 4; i++){
		saveWorldPopList(count, outlist, leaf->children[i]);
	}
	return TRUE;
}

int saveWorld(char * filename){
	console_printf("Saving %i objects to %s\n", world_numObjects, filename);
	if(!worldroot){
		console_printf("ERROR: No root world to save!\n");
		return FALSE;
	}
	if(!world_numObjects/* && !entity_count && !light_count*/){
		console_printf("ERROR: No world things to save!\n");
		return FALSE;
	}
	FILE *f = fopen(filename,"wb");
	if(!f){
		console_printf("ERROR: could not open file %s for writing\n", filename);
		return FALSE;
	}
	worldFileHeader_t header;
	header.version = 2;
	header.objectlistcount = world_numObjects;
//	header.entitylistcount = entity_count;
//	header.lightlistcount = light_count;
	worldobject_t ** worldlist = malloc(world_numObjects * sizeof(worldobject_t *));
	//we already have a list of entities and lights, no need to allocate a new one
	if(!worldlist){
		return FALSE;
	}
	//populate the list
	int count = 0;
	saveWorldPopList(&count, worldlist, worldroot);
	if(count != world_numObjects){
		free(worldlist);
		console_printf("ERROR: leaflist prop failed\n");
		return FALSE;
	}
	worldFileObject_t * objlist = malloc(count*sizeof(worldFileObject_t));
//	worldFileEntity_t * entlist = malloc(entity_count*sizeof(worldFileEntity_t));
//	worldFileLight_t * lightlist = malloc(light_count*sizeof(worldFileLight_t));
	memset(objlist, 0, count * sizeof(worldFileObject_t));
//	memset(entlist, 0, entity_count * sizeof(worldFileEntity_t));
//	memset(lightlist, 0, light_count * sizeof(worldFileLight_t));

	int modelcount = 0;
	int texturecount = 0;
	int shadercount = 0;
	int *modellist = 0;
	int *shaderlist = 0;
	int *texturelist = 0;
	int i = 0;
	//todo copy this for lights and entities as well
	for(i = 0; i < count; i++){
//		console_printf("count: %i i: %i\n", count, i);

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
		model_t * m = model_returnById(modellist[i]);
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
		shaderprogram_t * s = shader_returnById(shaderlist[i]);
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

	header.filesize = header.shaderlistlength + header.modellistlength + header.texturelistlength + (header.objectlistcount * sizeof(worldFileObject_t))/*+(header.entitylistcount * sizeof(worldFileEntity_t))+(header.lightlistcount * sizeof(worldFileLight_t))*/;

//	now i can start to write
	fwrite(&header, 1, sizeof(worldFileHeader_t), f);
	fwrite(modelliststring, 1, header.modellistlength, f);
	if(modelliststring)free(modelliststring);
	fwrite(textureliststring, 1, header.texturelistlength, f);
	if(textureliststring)free(textureliststring);
	fwrite(shaderliststring, 1, header.shaderlistlength, f);
	if(shaderliststring)free(shaderliststring);
	fwrite(objlist, 1, header.objectlistcount * sizeof(worldFileObject_t), f);
	if(objlist)free(objlist);
//	fwrite(entlist, 1, header.entitylistcount * sizeof(worldFileEntity_t), f);
//	if(entlistlist)free(objlist);
//	fwrite(lightlist, 1, header.lightlistcount * sizeof(worldFileLight_t), f);
//	if(lightlist)free(objlist);


	fclose(f);
	return header.filesize;
}
int recalcObjBBox(worldobject_t *o){
	model_t * m = model_returnById(o->modelid);
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
		console_printf("opening file:%s failed\n", filename);
		return FALSE;
	}
	worldFileHeader_t header;

	char **modelnamelist = 0;
	char **texturenamelist = 0;
	char **shadernamelist = 0;
	char *buf = 0;


	unsigned int version = 0;
	if(fread(&version, 1, sizeof(version), f)!= sizeof(int)) goto error;
	rewind(f);
	switch(version){
		default: goto error;
		case 2:
			if(fread(&header, 1, sizeof(worldFileHeaderv2_t), f) != sizeof(worldFileHeaderv2_t))goto error;
			header.entitylistcount = 0;
			header.lightlistcount = 0;
			break;
		case 3:
			if(fread(&header, 1, sizeof(worldFileHeader_t), f) != sizeof(worldFileHeader_t))goto error;
			break;
	}

//	if(header.version != WORLDFILEVERSION)goto error; //todo different version handles
	if(header.shaderlistlength + header.modellistlength + header.texturelistlength + (header.objectlistcount * sizeof(worldFileObject_t)) + (header.entitylistcount * sizeof(worldFileEntity_t))+ (header.lightlistcount * sizeof(worldFileLight_t)) != header.filesize) goto error;



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



//todo copy this for lights and entities
	worldFileObject_t * objbuf = (worldFileObject_t *)newbuf;
	worldobject_t * obj = malloc(sizeof(worldobject_t));
	//todo different version handling here

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
		else obj->modelid = model_createAndAddRINT(modelnamelist[modelindice-1]);
		if(!textureindice) obj->texturegroupid = 0;
		else obj->texturegroupid = texture_createAndAddGroupRINT(texturenamelist[textureindice-1]);
		if(!shaderindice) obj->shaderid = 0;
		else obj->shaderid = shader_createAndAddRINT(shadernamelist[shaderindice-1]);
		obj->shaderperm = objbuf[i].shaderperm;
		recalcObjBBox(obj);
		obj->flags = objbuf[i].flags;
		obj->status = 1;
		addObjectToWorld(obj);
	}
	if(obj) free(obj);

	if(shadernamelist)free(shadernamelist);
	if(texturenamelist)free(texturenamelist);
	if(modelnamelist)free(modelnamelist);
	if(buf)free(buf);
	fclose(f);
	//maybe check if world_numObjects = header.objectlistcount...
	console_printf("%i objects loaded from %s\n", header.objectlistcount, filename);
//	console_printf("%i entities loaded from %s\n", header.entitylistcount, filename);
//	console_printf("%i lights loaded from %s\n", header.lightlistcount, filename);
	return TRUE;

	error:
		fclose(f);
		if(buf) free(buf);
		if(modelnamelist) free(modelnamelist);
		if(texturenamelist) free(texturenamelist);
		if(shadernamelist) free(shadernamelist);
		console_printf("%s: error while loading world\n", filename);
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
	leaf->bbox[4] = center[1] + leaf->size;
	leaf->bbox[5] = center[1] - leaf->size;
	leaf->bbox[2] = 0.0; // redundant because of the memset
	leaf->bbox[3] = 0.0;
	leaf->objectarraylasttaken = -1;
	leaf->entityarraylasttaken = -1;
	leaf->lightarraylasttaken = -1;

	getBBoxPFromBBox(leaf->bbox, leaf->bboxp);
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
/* gonna redo
int walkAndDeleteObject(worldleaf_t * l, worldobject_t * o){
	if(o->treedepth > l->treedepth){
		int xspace = FALSE;
		int yspace = FALSE;
		if(o->pos[0] > l->center[0]) xspace = TRUE;
		if(o->pos[2] > l->center[1]) yspace = TRUE;
//		worldleaf_t * children = &l->children[0];
		worldleaf_t * child = l->children[xspace + 2*yspace];
		if(!child) return FALSE;
		int r = walkAndDeleteObject(child, o);
		char tflags = l->myincludes;
		//have to check all 4 chillun here
		if(l->children[0])tflags |= l->children[0]->includes;
		if(l->children[1])tflags |= l->children[1]->includes;
		if(l->children[2])tflags |= l->children[2]->includes;
		if(l->children[3])tflags |= l->children[3]->includes;
		l->includes = tflags;
		if(!child->numobjects){ //todo make sure to only have this if the ents and lights are good
			int c;
			for(c = 0; c < 4 && child->children[c]; c++); // make sure all are null
			if(c == 4) free(child); //easier than doing the normal delete
			//todo possible segfault cause here
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
		//todo check include flags here
		*listobj = l->list[l->numobjects]; // replace it with the one at the end
		l->list = realloc(l->list, l->numobjects * sizeof(worldobject_t)); //resize array
		if(r){
			//todo
			// recalc bbox
		}
		world_numObjects--;
		return r;

	//todo
	}
	return FALSE;
}
*/ //going to redo
/*
int deleteObject(worldobject_t * o){
	if(!o) return FALSE;
	return walkAndDeleteObject(worldroot, o);
}
*/ //going to redo
//going to redo as well
int deleteLeaf(worldleaf_t *l){
	if(!l) return FALSE;
	int i, count = 1;
	for(i = 0; i < 4; i++){
		count += deleteLeaf(l->children[i]);
	}
//	if(l->numobjects && l->list)free(l->list);
//	if(l->numents && l->entlist) free(l->entlist);
//	if(l->numlights && l->lightlist) free(l->lightlist);
	if(l->list)free(l->list);
	if(l->entlist) free(l->entlist);
	if(l->lightlist) free(l->lightlist);
	world_numObjects -= l->numobjects;
	world_numEnts -= l->numents;
	world_numLights -= l->numlights;
	free(l);
	return(count);
}

int deleteWorld(void){
	int leafcount = deleteLeaf(worldroot);
	worldroot = 0;
	world_numObjects = 0;
	if(!initWorldSystem())return FALSE;
	return leafcount;
}
//maybe going to redo
int addObjectToLeaf(worldobject_t * o, worldleaf_t *l){
	//todo
	//if first object in, set anyway
	if(!l->numobjects && !l->numents){
		l->bbox[2] = o->bbox[2];
		l->bbox[3] = o->bbox[3];
	} else {
		if(o->bbox[2] > l->bbox[2])l->bbox[2] = o->bbox[2];
		if(o->bbox[3] < l->bbox[3])l->bbox[3] = o->bbox[3];
	}
	getBBoxPFromBBox(l->bbox, l->bboxp);	//todo i can optimize this maybe
	o->treedepth = l->treedepth;
	o->leafpos = l->numobjects;
	o->leaf = l;
	l->numobjects++;

	worldobject_t * list = l->list;
	int myarrayfo = l->objectarrayfirstopen;
	int myarraysize = l->objectarraysize;
	for(; myarrayfo < myarraysize  && list[myarrayfo].leaf; myarrayfo++);
	l->objectarrayfirstopen = myarrayfo;
	if(myarrayfo == myarraysize){ //resize
		l->objectarraysize = ++myarraysize;
		l->list = list = realloc(list, myarraysize * sizeof(worldobject_t));
	}
	list[myarrayfo] = *o;
	if(l->objectarraylasttaken < myarrayfo) l->objectarraylasttaken = myarrayfo;


//	l->list = realloc(l->list, l->numobjects * sizeof(worldobject_t));
//	l->list[l->numobjects-1] = *o;
	l->myincludes = l->myincludes | WORLDTREEOBJECT;
	world_numObjects++;
	return TRUE;
}
//maybegoing to redo
int walkAndAddObject(worldobject_t * o, worldleaf_t * l){
	//add obj to flags
	l->includes = l->includes | WORLDTREEOBJECT;
	char xspace = 0;
	char yspace = 0;
	char nofits = 0;
	if(l->treedepth >= WORLDTREEDEPTH) nofits = TRUE;
	if(!nofits){ //possible optimization? might remove
		//find possible space
		if(o->bbox[0] > l->center[0]) xspace = TRUE;
		if(o->bbox[4] > l->center[1]) yspace = TRUE;

		//now check the other bounds, make sure its the same as the origional
		if((o->bbox[1] > l->center[0]) != xspace) nofits = TRUE;
		if((o->bbox[5] > l->center[1]) != yspace) nofits = TRUE;
	}
	if(nofits){
		//todo?
		return addObjectToLeaf(o, l);
	} else {
		int intspace = xspace + 2*yspace;
		if(!l->children[intspace]){
			vec2_t newcenter;
			newcenter[0] = l->center[0] + ((float)xspace-0.5)*l->size;
			newcenter[1] = l->center[1] + ((float)yspace-0.5)*l->size;
			l->children[intspace] = createWorldLeaf(l->treedepth + 1, newcenter);
			l->children[intspace]->parent = l;
			walkAndAddObject(o, l->children[intspace]);
		} else {
			walkAndAddObject(o, l->children[intspace]);
		}
		//should figure out top and bottom bounds now...
		if(l->bbox[2] < l->children[intspace]->bbox[2]) l->bbox[2] = l->children[intspace]->bbox[2];
		if(l->bbox[3] > l->children[intspace]->bbox[3]) l->bbox[3] = l->children[intspace]->bbox[3];
		//should recalc bboxp now
		getBBoxPFromBBox(l->bbox, l->bboxp);
		//todo i can make a more efficient way of doing this, only updating the y values of the points
		return 2;
	}
	return FALSE; // should never hit
}
//maybe going to redo
int addObjectToWorld(worldobject_t * o){
	model_t * m = model_returnById(o->modelid); //todo maybe i should remove the need for VBOs so i can add "empty" objects
	if(!m) return FALSE;
	if(!m->vbo) return FALSE;
	//walk tree and add
	return walkAndAddObject(o, worldroot);
}
//maybe going to redo
int addEntityToLeaf(entity_t * e, worldleaf_t *l){
	//todo
	//if first object in, set anyway
	if(!l->numobjects && !l->numents){
		l->bbox[2] = e->bbox[2];
		l->bbox[3] = e->bbox[3];
	} else {
		if(e->bbox[2] > l->bbox[2])l->bbox[2] = e->bbox[2];
		if(e->bbox[3] < l->bbox[3])l->bbox[3] = e->bbox[3];
	}
	getBBoxPFromBBox(l->bbox, l->bboxp);
	e->treedepth = l->treedepth;
	e->leafpos = l->numents;
	e->leaf = l;
	l->numents++;



	int * list = l->entlist;
	int myarrayfo = l->entityarrayfirstopen;
	int myarraysize = l->entityarraysize;
	for(; myarrayfo < myarraysize  && list[myarrayfo]; myarrayfo++);
	l->entityarrayfirstopen = myarrayfo;
	if(myarrayfo == myarraysize){ //resize
		l->entityarraysize = ++myarraysize;
		l->entlist = list = realloc(list, myarraysize * sizeof(int));
	}
	list[myarrayfo] = e->myid;
	if(l->entityarraylasttaken < myarrayfo) l->entityarraylasttaken = myarrayfo;




//	l->entlist = realloc(l->list, l->numents * sizeof(void *));
//	l->entlist[l->numents-1] = e;
	l->myincludes = l->myincludes | WORLDTREEENTITY;
//	l->includes = l->includes | WORLDTREEENTITY;
	world_numEnts++;
	return TRUE;
}
//probably not gonna use this func... here anyway
worldleaf_t * walkDown(vec_t b[6], worldleaf_t *l){
	char xspace = 0;
	char yspace = 0;
	char nofits = 0;
	if(l->treedepth >= WORLDTREEDEPTH) nofits = TRUE;
	if(!nofits){ //possible optimization? might remove
		//find possible space
		if(b[0] > l->center[0]) xspace = TRUE;
		if(b[4] > l->center[1]) yspace = TRUE;

		//now check the other bounds, make sure its the same as the origional
		if((b[1] > l->center[0]) != xspace) nofits = TRUE;
		if((b[5] > l->center[1]) != yspace) nofits = TRUE;
	}
	if(!nofits){
		int intspace = xspace + 2*yspace;
		if(!l->children[intspace]){
			vec2_t newcenter;
			newcenter[0] = l->center[0] + ((float)xspace-0.5)*l->size;
			newcenter[1] = l->center[1] + ((float)yspace-0.5)*l->size;
			l->children[intspace] = createWorldLeaf(l->treedepth + 1, newcenter);
			l->children[intspace]->parent = l;
		}
		return walkDown(b, l->children[intspace]);
	}
	return l;
}
//maybe going to redo
int walkAndAddEntity(entity_t * e, worldleaf_t * l){
	l->includes = l->includes | WORLDTREEENTITY;
	char xspace = 0;
	char yspace = 0;
	char nofits = 0;
	if(l->treedepth >= WORLDTREEDEPTH) nofits = TRUE;
	if(!nofits){ //possible optimization? might remove
		//find possible space
		if(e->bbox[0] > l->center[0]) xspace = TRUE;
		if(e->bbox[4] > l->center[1]) yspace = TRUE;

		//now check the other bounds, make sure its the same as the origional
		if((e->bbox[1] > l->center[0]) != xspace) nofits = TRUE;
		if((e->bbox[5] > l->center[1]) != yspace) nofits = TRUE;
	}
	if(nofits){
		//todo?
		return addEntityToLeaf(e, l);
	} else {
		int intspace = xspace + 2*yspace;
		if(!l->children[intspace]){
			vec2_t newcenter;
			newcenter[0] = l->center[0] + ((float)xspace-0.5)*l->size;
			newcenter[1] = l->center[1] + ((float)yspace-0.5)*l->size;
			l->children[intspace] = createWorldLeaf(l->treedepth + 1, newcenter);
			l->children[intspace]->parent = l;
			walkAndAddEntity(e, l->children[intspace]);
		} else {
			walkAndAddEntity(e, l->children[intspace]);
		}
		//should figure out top and bottom bounds now...
		if(l->bbox[2] < l->children[intspace]->bbox[2]) l->bbox[2] = l->children[intspace]->bbox[2];
		if(l->bbox[3] > l->children[intspace]->bbox[3]) l->bbox[3] = l->children[intspace]->bbox[3];
		//should recalc bboxp now
		getBBoxPFromBBox(l->bbox, l->bboxp);
		//todo i can make a more efficient way of doing this, only updating the y values of the points
		return 2;
	}
	return FALSE; // should never hit
}
int addEntityToWorld(void * e){
	return walkAndAddEntity((entity_t *)e, worldroot);
}
//todo i really should rename this to avoid confusion...
int addEntityToWorldOBJ(const int entityid){
	entity_t *e = entity_returnById(entityid);
	if(!e) return FALSE;

	model_t *m = model_returnById(e->modelid);
	if(!m) return FALSE;
	if(!m->vbo) return FALSE;

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



//will returns 0 if not moved, currently doesn
int moveEntInWorld(void * ep){
	entity_t * e = (entity_t *) ep;
	worldleaf_t *l1 = e->leaf;
	worldleaf_t *l = l1;
	char REMOVEENTMASK = ~WORLDTREEENTITY;


	if(e->bbox[0] < l->bbox[0] && e->bbox[1] > l->bbox[1] && e->bbox[4] < l->bbox[4] && e->bbox[5] > l->bbox[5]){
		char nofits = FALSE;
		if(l->treedepth >= WORLDTREEDEPTH) nofits = TRUE; //cant move any lower anyway...
		//check if i can move any lower
		//find possible space
		if(!nofits){
			char xspace = FALSE;
			char yspace = FALSE;
			if(e->bbox[0] > l->center[0]) xspace = TRUE;
			if(e->bbox[4] > l->center[1]) yspace = TRUE;
			//now check the other bounds, make sure its the same as the origional
			if((e->bbox[1] > l->center[0]) != xspace) nofits = TRUE;
			if((e->bbox[5] > l->center[1]) != yspace) nofits = TRUE;
		}
		if(!nofits){
			//the entity can further move down in the tree
			unsigned int leafpos = e->leafpos;
			//check to make sure entity is the same one
			int * list = l->entlist;
			if(list[leafpos] != e->myid) return -1; //DAMN WE FUKED UP
			//remove entity from list
			list[leafpos] = 0;
			//fix up bound info for list
			if(leafpos < l->entityarrayfirstopen) l->entityarrayfirstopen = leafpos;
			int entityarraylasttaken = l->entityarraylasttaken;
			for(; entityarraylasttaken > 0 && !list[entityarraylasttaken]; entityarraylasttaken--);
			l->entityarraylasttaken = entityarraylasttaken;
			l->numents--;
			//walk and add ent
			walkAndAddEntity(e, l);
			if(l->numents < 1) l->myincludes = l->myincludes & REMOVEENTMASK;
		} else {
			// i need to manually fix the bbox for this leaf
			//todo
		}
	} else {
		//entity does not fit within bbox anymore, delete it
		unsigned int leafpos = e->leafpos;
		//check to make sure entity is the same one
		int * list = l->entlist;
		if(list[leafpos] != e->myid) return 0; //DAMN WE FUKED UP
		//remove entity from list
		list[leafpos] = 0;
		//fix up bound info for list
		if(leafpos < l->entityarrayfirstopen) l->entityarrayfirstopen = leafpos;
		int entityarraylasttaken = l->entityarraylasttaken;
		for(; entityarraylasttaken > 0 && !list[entityarraylasttaken]; entityarraylasttaken--);
		l->entityarraylasttaken = entityarraylasttaken;
		l->numents--;
		//update my flags

		if(l->numents < 1){
			l->myincludes = l->myincludes & REMOVEENTMASK;
			l->includes = l->myincludes;
			if(l->children[0])l->includes |= l->children[0]->includes;
			if(l->children[1])l->includes |= l->children[1]->includes;
			if(l->children[2])l->includes |= l->children[2]->includes;
			if(l->children[3])l->includes |= l->children[3]->includes;
		}

		//TODO CHECK IF I NEED TO DELETE THIS LEAF

		//pop up till ent fits
		for(l = l->parent; l; l = l->parent) if(e->bbox[0] < l->bbox[0] && e->bbox[1] > l->bbox[1] && e->bbox[4] < l->bbox[4] && e->bbox[5] > l->bbox[5]) break;
		if(!l){
			e->leaf = 0;
			console_printf("out of bounds ent!\n");
			return 0;
		}
		//recursively add to new leaf
		walkAndAddEntity(e, l);
		//walk up and fix flags
		//TODO DELETE HERE AS WELL

		if(l1->numents < 1 && !(l->includes & WORLDTREEENTITY)){
			for(l = l1->parent; l; l = l->parent){
				if(l->myincludes & WORLDTREEENTITY) break;
				l->includes = l->myincludes;
				if(l->children[0])l->includes |= l->children[0]->includes;
				if(l->children[1])l->includes |= l->children[1]->includes;
				if(l->children[2])l->includes |= l->children[2]->includes;
				if(l->children[3])l->includes |= l->children[3]->includes;
			}
		}

	}
	//TODO RECALC BBOXES, WALK UP FROM L AND L1


	//TODO WALK UP, WALK DOWN, CHECK IF RESULTING LEAF IS DIFFERENT THAN STARTING LEAF.
	//IF IT IS, MOVE ENT
		// RECALC BBOX IN NEW ONE
		// THEN RECALC BBOX IN OLD ONE
//	ELSE
	// recalc bbox in old one

	//store current leaf in 1
	//walk up till entity fits in leaf
	//walk down till entity only fits in leaf and not children, or reach lowest branch.
		//add leafs if needed
		//store leaf  in 2
	//move entity to new leaf
	//update flags if needed, from 1 up
	//update flags if needed, from 2 up
		//delete old leaf(1) if needed, walk up and continue deleting
	return TRUE;
}
