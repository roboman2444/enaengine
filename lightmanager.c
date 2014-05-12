//global includes
#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>
//local includes
#include "globaldefs.h"
#include "hashtables.h"
#include "matrixlib.h"
#include "entitymanager.h"
#include "mathlib.h"
#include "lightmanager.h"

int lightcount = 0;
int lightArrayFirstOpen = 0;
int lightArrayLastTaken = -1;
int lightArraySize = 0;
int lightsOK = 0;
light_t *lightlist;
hashbucket_t lighthashtable[MAXHASHBUCKETS];

int recalcLightBBox(light_t *l){
	//todo calculate some special thing for non-point lights...
	float scale = l->scale;
	l->bbox[0] = l->pos[0] - scale;
	l->bbox[1] = l->pos[0] + scale;
	l->bbox[2] = l->pos[1] - scale;
	l->bbox[3] = l->pos[1] + scale;
	l->bbox[4] = l->pos[2] - scale;
	l->bbox[5] = l->pos[2] + scale;
	getBBoxpFromBBox(l->bbox, l->bboxp);
	return TRUE;
}
int recheckLightLeaf(light_t *l){
	return TRUE;
}
int lightLoop(void){
	int count = 0, i;
	for(i = 0; i <= lightArrayLastTaken; i++){
		light_t *l = &lightlist[i];
		entity_t *e = returnEntityById(l->attachmentid);
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
				recalcLightBBox(l);
				recheckLightLeaf(l);
				count++;
				l->needsupdate = 0;
//	consolePrintf("updated light\n");
			}
		} else if(l->needsupdate){
				recalcLightBBox(l);
				recheckLightLeaf(l);
				count++;
				l->needsupdate = 0;
		}
	}
	return count;
}


int initLightSystem(void){
	memset(lighthashtable, 0, MAXHASHBUCKETS * sizeof(hashbucket_t));
	if(lightlist) free(lightlist);
	lightlist = 0;
//	lightlist = malloc(lightcount * sizeof(light_t));
//	if(!lightlist) memset(lightlist, 0 , lightcount * sizeof(light_t));
//	addLightRINT("default");
	//todo make this useful, gonna need a large list to put the lights in viewport into, then sort front to back and remove back ones
	maxVisLights = 50;
	maxShadowLights = 20;
	lightsOK = TRUE;
	return TRUE; // todo error check
}
lightlistpoint_t findLightsByNameRPOINT(char * name){
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
lightlistint_t findLightsByNameRINT(char * name){
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

light_t * findLightByNameRPOINT(char * name){ //todo write a function that can find ALL entities with name
	return returnLightById(findByNameRINT(name, lighthashtable));
}
int findLightByNameRINT(char * name){
	return findByNameRINT(name, lighthashtable);
}

int deleteLight(int id){
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
light_t * returnLightById(int id){
//	int lightspawncount = (id >> 16);
	int lightindex = (id & 0xFFFF);
	light_t * ent = &lightlist[lightindex];
	if(!ent->type) return FALSE;
	if(ent->myid == id) return ent;
	return FALSE;
}
light_t createLight(char * name){
	light_t newlight;
	memset(&newlight, 0, sizeof(light_t));
	newlight.type = 1;
	newlight.name = malloc(strlen(name)+1); // todo maybe put this somewhere else...
	strcpy(newlight.name, name);
	newlight.scale = 1.0;
//	Matrix4x4_CreateFromQuakeEntity(&newlight.mat, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
	return newlight;
//todo
}



int addLightRINT(char * name){
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
light_t * addLightRPOINT(char * name){
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
