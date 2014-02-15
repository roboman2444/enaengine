#include <GL/glew.h>
#include <GL/gl.h>

#include "globaldefs.h"
#include "matrixlib.h"
#include "entitymanager.h"
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
	return count+1;
}
int addEntityToTexturebatche(entity_t * ent, texturebatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	int modelid = ent->modelid;
	if(!batch->modelbatch) count = 0;
	int i;
	for(i = 0; i < count; i++){
		if(batch->modelbatch[i].modelid == modelid){
			return addEntityToModelbatche(ent, &batch->modelbatch[i]);
		}
	}
	//doesnt have texture in it
	batch->count++;
	batch->modelbatch = realloc(batch->modelbatch, batch->count * sizeof(modelbatche_t));
	batch->modelbatch[count].count = 0;
	batch->modelbatch[count].modelid = modelid;
	batch->modelbatch[count].matlist = 0;

	return addEntityToModelbatche(ent, &batch->modelbatch[count]);
}
int addEntityToShaderbatche(entity_t * ent, shaderbatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	int textureid = ent->texturegroupid;
	if(!batch->texturebatch) count = 0;
	int i;
	for(i = 0; i < count; i++){
		if(batch->texturebatch[i].textureid == textureid){
			return addEntityToTexturebatche(ent, &batch->texturebatch[i]);
		}
	}
	//doesnt have shader in it
	batch->count++;
	batch->texturebatch = realloc(batch->texturebatch, batch->count * sizeof(texturebatche_t));
	batch->texturebatch[count].count = 0;
	batch->texturebatch[count].textureid = textureid;
	batch->texturebatch[count].modelbatch = 0;

	return addEntityToTexturebatche(ent, &batch->texturebatch[count]);
}

int addEntityToRenderbatche(entity_t * ent, renderbatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	int programid = ent->programid;
	if(!batch->shaderbatch) count = 0;
	int i;
	for(i = 0; i < count; i++){
		if(batch->shaderbatch[i].programid == programid){
			return addEntityToShaderbatche(ent, &batch->shaderbatch[i]);
		}
	}
	//doesnt have shader in it
	batch->count++;
	batch->shaderbatch = realloc(batch->shaderbatch, batch->count * sizeof(shaderbatche_t));
	batch->shaderbatch[count].count = 0;
	batch->shaderbatch[count].programid = programid;
	batch->shaderbatch[count].texturebatch = 0;

	return addEntityToShaderbatche(ent, &batch->shaderbatch[count]);
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
