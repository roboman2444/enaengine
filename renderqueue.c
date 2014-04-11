#include <GL/glew.h>
#include <GL/gl.h>

#include "globaldefs.h"
#include "matrixlib.h"
#include "entitymanager.h"
#include "worldmanager.h"
#include "lightmanager.h"
#include "renderqueue.h"
#include "glmanager.h"

int addEntityToModelbatche(entity_t * ent, modelbatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	if(!batch->matlist) count = 0;
	//doesnt have texture in it
	batch->count++;
	batch->matlist = realloc(batch->matlist, batch->count * sizeof(matrix4x4_t));
	batch->matlist[count] = ent->mat;
//	batch->matlist = realloc(batch->cammatlist, batch->count * sizeof(matrix4x4_t));
//	Matrix4x4_Concat(&cammatlist[count], viewport mat, ent->mat);
//	batch->cammatlist[count] = ent->mat;
	return count+1;
}
int addEntityToTexturebatche(entity_t * ent, texturebatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	int modelid = ent->modelid;
//	modelbatche_t * b = batch->modelbatch;
	if(!batch->modelbatch) count = 0;
//	if(!b) count = 0;
	int i;
	for(i = 0; i < count; i++){
		if(batch->modelbatch[i].modelid == modelid){
//		if(b[i].modelid == modelid){
			return addEntityToModelbatche(ent, &batch->modelbatch[i]);
	//		return addEntityToModelbatche(ent, &b[i]);
		}
	}
	//doesnt have texture in it
	batch->count++;
	batch->modelbatch = realloc(batch->modelbatch, batch->count * sizeof(modelbatche_t));
//	b = batch->modelbatch = realloc(b, batch->count * sizeof(modelbatche_t));
	batch->modelbatch[count].count = 0;
	batch->modelbatch[count].modelid = modelid;
	batch->modelbatch[count].matlist = 0;
//	b->count = 0;
//	b->modelid = modelid;
//	b->matlist = 0;

	return addEntityToModelbatche(ent, &batch->modelbatch[count]);
//	return addEntityToModelbatche(ent, &b[count]);
}
int addEntityToShaderbatche(entity_t * ent, shaderbatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	int textureid = ent->texturegroupid;
//	texturebatche_t * b = batch->texturebatch;
	if(!batch->texturebatch) count = 0;
//	if(!b) count = 0;
	int i;
	for(i = 0; i < count; i++){
		if(batch->texturebatch[i].textureid == textureid){
//		if(b[i].textureid == textureid){
			return addEntityToTexturebatche(ent, &batch->texturebatch[i]);
//			return addEntityToTexturebatche(ent, &b[i]);
		}
	}
	//doesnt have shader in it
	batch->count++;
	batch->texturebatch = realloc(batch->texturebatch, batch->count * sizeof(texturebatche_t));
//	batch->texturebatch = realloc(b, batch->count * sizeof(texturebatche_t));
	batch->texturebatch[count].count = 0;
	batch->texturebatch[count].textureid = textureid;
	batch->texturebatch[count].modelbatch = 0;

	return addEntityToTexturebatche(ent, &batch->texturebatch[count]);
//	return addEntityToTexturebatche(ent, &b[count]);
}

int addEntityToRenderbatche(entity_t * ent, renderbatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	int shaderid = ent->shaderid;
	int shaderperm = ent->shaderperm;
//	shaderbatche_t *b = batch->shaderbatch;
	if(!batch->shaderbatch) count = 0;
//	if(!b) count = 0;
	int i;
	for(i = 0; i < count; i++){
		if(batch->shaderbatch[i].shaderid == shaderid && batch->shaderbatch[i].shaderperm == shaderperm){
//		if(b[i].shaderid == shaderid && b[i].shaderperm == shaderperm){
			return addEntityToShaderbatche(ent, &batch->shaderbatch[i]);
//			return addEntityToShaderbatche(ent, &b[i]);
		}
	}
	//doesnt have shader in it
	batch->count++;
	batch->shaderbatch = realloc(batch->shaderbatch, batch->count * sizeof(shaderbatche_t));
	batch->shaderbatch[count].count = 0;
	batch->shaderbatch[count].shaderid = shaderid;
	batch->shaderbatch[count].shaderperm = shaderperm;
	batch->shaderbatch[count].texturebatch = 0;

	return addEntityToShaderbatche(ent, &batch->shaderbatch[count]);
//	return addEntityToShaderbatche(ent, &b[count]);
}




//todo more streamlined for objects
int addObjectToModelbatche(worldobject_t * obj, modelbatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	if(!batch->matlist) count = 0;
	//doesnt have texture in it
	batch->count++;
	batch->matlist = realloc(batch->matlist, batch->count * sizeof(matrix4x4_t));
	batch->matlist[count] = obj->mat;
//	batch->matlist = realloc(batch->cammatlist, batch->count * sizeof(matrix4x4_t));
//	Matrix4x4_Concat(&cammatlist[count], viewport mat, ent->mat);
//	batch->cammatlist[count] = ent->mat;
	return count+1;
}
int addObjectToTexturebatche(worldobject_t * obj, texturebatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	int modelid = obj->modelid;
//	modelbatche_t * b = batch->modelbatch;
	if(!batch->modelbatch) count = 0;
//	if(!b) count = 0;
	int i;
	for(i = 0; i < count; i++){
		if(batch->modelbatch[i].modelid == modelid){
//		if(b[i].modelid == modelid){
			return addObjectToModelbatche(obj, &batch->modelbatch[i]);
//			return addObjectToModelbatche(obj, &b[i]);
		}
	}
	//doesnt have texture in it
	batch->count++;
	batch->modelbatch = realloc(batch->modelbatch, batch->count * sizeof(modelbatche_t));
//	batch->modelbatch = realloc(b, batch->count * sizeof(modelbatche_t));
//	b = batch->modelbatch = realloc(b, batch->count * sizeof(modelbatche_t));
	batch->modelbatch[count].count = 0;
	batch->modelbatch[count].modelid = modelid;
	batch->modelbatch[count].matlist = 0;
//	b[count].count = 0;
//	b[count].modelid = modelid;
//	b[count].matlist = 0;

	return addObjectToModelbatche(obj, &batch->modelbatch[count]);
//	return addObjectToModelbatche(obj, &b[count]);
}



int addObjectToShaderbatche(worldobject_t * obj, shaderbatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	int textureid = obj->textureid;
	if(!batch->texturebatch) count = 0;
	int i;
	for(i = 0; i < count; i++){
		if(batch->texturebatch[i].textureid == textureid){
			return addObjectToTexturebatche(obj, &batch->texturebatch[i]);
		}
	}
	//doesnt have shader in it
	batch->count++;
	batch->texturebatch = realloc(batch->texturebatch, batch->count * sizeof(texturebatche_t));
	batch->texturebatch[count].count = 0;
	batch->texturebatch[count].textureid = textureid;
	batch->texturebatch[count].modelbatch = 0;

	return addObjectToTexturebatche(obj, &batch->texturebatch[count]);
}



int addObjectToRenderbatche(worldobject_t * obj, renderbatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	int shaderid = obj->shaderid;
	int shaderperm = obj->shaderperm;
	if(!batch->shaderbatch) count = 0;
	int i;
	for(i = 0; i < count; i++){
		if(batch->shaderbatch[i].shaderid == shaderid && batch->shaderbatch[i].shaderperm == shaderperm){
			return addObjectToShaderbatche(obj, &batch->shaderbatch[i]);
		}
	}
	//doesnt have shader in it
	batch->count++;
	batch->shaderbatch = realloc(batch->shaderbatch, batch->count * sizeof(shaderbatche_t));
	batch->shaderbatch[count].count = 0;
	batch->shaderbatch[count].shaderid = shaderid;
	batch->shaderbatch[count].shaderperm = shaderperm;
	batch->shaderbatch[count].texturebatch = 0;

	return addObjectToShaderbatche(obj, &batch->shaderbatch[count]);
}


int cleanupModelbatche(modelbatche_t * batch){
	if(!batch) return FALSE;
	if(batch->matlist)free(batch->matlist);
	batch->count = 0;
	batch->matlist = 0;
	return TRUE;
}
int cleanupTexturebatche(texturebatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	if(!batch->modelbatch) count = 0;
	int i;
	for(i = 0; i < count; i++){
		cleanupModelbatche(&batch->modelbatch[i]);
	}
	if(batch->modelbatch)free(batch->modelbatch);
	batch->count = 0;
	batch->modelbatch = 0;
	return TRUE;
}
int cleanupShaderbatche(shaderbatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	if(!batch->texturebatch) count = 0;
	int i;
	for(i = 0; i < count; i++){
		cleanupTexturebatche(&batch->texturebatch[i]);
	}
	if(batch->texturebatch)free(batch->texturebatch);
	batch->count = 0;
	batch->texturebatch = 0;
	return TRUE;
}
int cleanupRenderbatche(renderbatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	if(!batch->shaderbatch) count = 0;
	int i;
	for(i = 0; i < count; i++){
		cleanupShaderbatche(&batch->shaderbatch[i]);
	}
	if(batch->shaderbatch)free(batch->shaderbatch);
	batch->count = 0;
	batch->shaderbatch = 0;
	return TRUE;
}
//deprecated?
void addLightToLightbatche(int lightid, lightbatche_t * batch){
	batch->count++;
	batch->lightlist = realloc(batch->lightlist, batch->count * sizeof(int));
	batch->lightlist[batch->count] = lightid;
}
//deprecated?
void cleanupLightbatche(lightbatche_t * batch){
	if(!batch) return;
	if(batch->lightlist) free(batch->lightlist);
	batch->count = 0;
}
