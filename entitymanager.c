//global includes
#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>
//local includes
#include "globaldefs.h"
#include "hashtables.h"
#include "matrixlib.h"
#include "texturemanager.h"
#include "entitymanager.h"

int entitycount = 0;
int entityArrayFirstOpen = 0;
int entityArrayLastTaken = 0; // may not be used
int entityArraySize = 0;
int entitiesOK = 0;
entity_t *entitylist;
hashbucket_t entityhashtable[MAXHASHBUCKETS];


int initEntitySystem(void){
	bzero(entityhashtable, MAXHASHBUCKETS * sizeof(hashbucket_t));
	if(entitylist) free(entitylist);
	entitylist = malloc(entitycount * sizeof(entity_t));
	if(!entitylist) memset(entitylist, 0 , entitycount * sizeof(entity_t));
	addEntityRINT("default");
	entitiesOK = TRUE;
	return TRUE; // todo error check
}
entitylistpoint_t findEntitiesByNameRPOINT(char * name){
	entitylistpoint_t ret;
	int hash = getHash(name);
	hashbucket_t * hb = &entityhashtable[hash];
	if(!hb->name) return ret;
        for(; hb; hb = hb->next){
		if(strcmp(hb->name, name)==0){
//			return returnEntityById(hb->id);
			ret.count++;
			ret.list = realloc(ret.list, ret.count * sizeof(entity_t *));
			ret.list[ret.count-1] = returnEntityById(hb->id);
		}
        }
	return ret;
}
entitylistint_t findEntitiesByNameRINT(char * name){
	entitylistint_t ret;
	int hash = getHash(name);
	hashbucket_t * hb = &entityhashtable[hash];
	if(!hb->name) return ret;
        for(; hb; hb = hb->next){
		if(strcmp(hb->name, name)==0){
//			return returnEntityById(hb->id);
			ret.count++;
			ret.list = realloc(ret.list, ret.count * sizeof(int));
			ret.list[ret.count-1] = hb->id;
		}
        }
	return ret;
}

entity_t * findEntityByNameRPOINT(char * name){ //todo write a function that can find ALL entities with name
	return returnEntityById(findByNameRINT(name, entityhashtable));
}
int findEntityByNameRINT(char * name){
	return findByNameRINT(name, entityhashtable);
}

int deleteEntity(int id){
	int entityindex = (id & 0xFFFF);
	entity_t * ent = &entitylist[entityindex];
	if(ent->myid != id) return FALSE;
	if(!ent->name) return FALSE;
	deleteFromHashTable(ent->name, id, entityhashtable);
	free(ent->name);
	bzero(ent, sizeof(entity_t));
//	ent->type = 0;
//	ent->model = 0;
//	ent->name = 0;
//	ent->myid = 0;
	if(entityindex < entityArrayFirstOpen) entityArrayFirstOpen = entityindex;
	for(; entityArrayLastTaken > 0 && !entitylist[entityArrayLastTaken].type; entityArrayLastTaken--);
	return TRUE;
}
entity_t * returnEntityById(int id){
//	int entityspawncount = (id >> 16);
	int entityindex = (id & 0xFFFF);
	entity_t * ent = &entitylist[entityindex];
	if(!ent->type) return FALSE;
	if(ent->myid == id) return ent;
	return FALSE;
}
entity_t createEntity(char * name){
	entity_t newent;
	bzero(&newent, sizeof(entity_t));
	newent.type = 1;
	newent.name = malloc(strlen(name)+1); // todo maybe put this somewhere else...
	strcpy(newent.name, name);
/*
	int i;
	for(i = 0; i<3; i++){
		newent.pos[i]=0.0;
		newent.vel[i]=0.0;
		newent.angle[i]=0.0;
		newent.anglevel[i]=0.0;
	}

	newent.touch = 0;
	newent.think = 0;
	newent.spawn = 0;
	newent.nextthink = 0;
	newent.needsmatupdate = FALSE;
	newent.model = 0;

*/
	newent.texturegroup = defaultTextureGroup;
	newent.scale = 1.0;
	Matrix4x4_CreateFromQuakeEntity(&newent.mat, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
//	entitycount++;
//	newent.spawncount = entitycount;
	return newent;
//todo
}



int addEntityRINT(char * name){
	entitycount++;
	for(; entityArrayFirstOpen < entityArraySize && entitylist[entityArrayFirstOpen].type; entityArrayFirstOpen++);
	if(entityArrayFirstOpen == entityArraySize){	//resize
		entityArraySize++;
		entitylist = realloc(entitylist, entityArraySize * sizeof(entity_t));
	}
	entitylist[entityArrayFirstOpen] = createEntity(name);
	int returnid = (entitycount << 16) | entityArrayFirstOpen;
	entitylist[entityArrayFirstOpen].myid = returnid;

	addToHashTable(entitylist[entityArrayFirstOpen].name, returnid, entityhashtable);
	if(entityArrayLastTaken < entityArrayFirstOpen) entityArrayLastTaken = entityArrayFirstOpen; //todo redo
	return returnid;
}
entity_t * addEntityRPOINT(char * name){
	entitycount++;
	for(; entityArrayFirstOpen < entityArraySize && entitylist[entityArrayFirstOpen].type; entityArrayFirstOpen++);
	if(entityArrayFirstOpen == entityArraySize){	//resize
		entityArraySize++;
		entitylist = realloc(entitylist, entityArraySize * sizeof(entity_t));
	}
	entitylist[entityArrayFirstOpen] = createEntity(name);
	int returnid = (entitycount << 16) | entityArrayFirstOpen;
	entitylist[entityArrayFirstOpen].myid = returnid;

	addToHashTable(entitylist[entityArrayFirstOpen].name, returnid, entityhashtable);
	//todo maybe have entity have a hash variable, so i dont have to calculate it again if i want to delete... maybe
	if(entityArrayLastTaken < entityArrayFirstOpen) entityArrayLastTaken = entityArrayFirstOpen;
//	printf("entityarraysize = %i\n", entityArraySize);
//	printf("entitycount = %i\n", entitycount);

	return &entitylist[entityArrayFirstOpen];

}
