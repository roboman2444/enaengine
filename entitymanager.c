//global includes
#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>
//local includes
#include "globaldefs.h"
#include "matrixlib.h"
#include "texturemanager.h"
#include "vbomanager.h"
#include "modelmanager.h"
#include "entitymanager.h"

typedef struct hashbucket_s {
	char * name;
	int id;
	struct hashbucket_s * next;
} hashbucket_t;

int entitycount = 0;
int entityArrayFirstOpen = 0;
int entityArrayLastTaken = 0; // may not be used
int entityArraySize = 0;
int entitiesOK = 0;
entity_t *entitylist;
hashbucket_t hashtable[MAXHASHBUCKETS];

int initEntitySystem(void){
	bzero(hashtable, MAXHASHBUCKETS * sizeof(hashbucket_t));
	if(entitylist) free(entitylist);
	entitylist = malloc(entitycount * sizeof(entity_t));
	if(!entitylist) memset(entitylist, 0 , entitycount * sizeof(entity_t));
	addEntityRINT("default");
	entitiesOK = TRUE;
	return TRUE; // todo error check
}
int getHash(char * string){
	unsigned long rethash=0;
	while(*string){
		rethash= rethash * 31 + *string;
		string++;
	}
	return rethash%MAXHASHBUCKETS;
}
int addEntityToHashTable(char * name, int id){
	int hash = getHash(name);
	hashbucket_t * hb = &hashtable[hash];
        for(; hb->next; hb = hb->next);
	hb->next = malloc(sizeof(hashbucket_t));
	hb->next->name = name;
	hb->next->id = id;
	return hash;
}
int deleteEntityFromHashTable(char * name, int id){
	int hash = getHash(name);
	hashbucket_t * hb = &hashtable[hash];
	hashbucket_t * oldb;
        for(oldb = hb; hb; oldb = hb, hb = hb->next){
		if(hb->id == id){
			oldb->next = hb->next;
			free(hb);
			return TRUE;
		}
        }
	return FALSE;
}

entity_t * findEntityByNameRPOINT(char * name){ //todo write a function that can find ALL entities with name
	int hash = getHash(name);
	hashbucket_t * hb = &hashtable[hash];
	if(!hb->name) return 0;
        for(; hb; hb = hb->next){
		if(strcmp(hb->name, name)){
			return returnById(hb->id);
		}
        }
	//not found :(
	return 0;
}
int findEntityByNameRINT(char * name){

	int hash = getHash(name);
	hashbucket_t * hb = &hashtable[hash];
	if(!hb->name) return 0;
        for(; hb; hb = hb->next){
		if(strcmp(hb->name, name)){
			return hb->id;
		}
        }
	//not found :(
	return 0;
}


int deleteEntity(int id){
	int entityindex = (id & 0xFFFF);
	entity_t * ent = &entitylist[entityindex];
	if(ent->myid != id) return FALSE;
	if(!ent->name) return FALSE;
	deleteEntityFromHashTable(ent->name, id);
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
entity_t * returnById(int id){
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
	newent.name = malloc(strlen(name)+1);
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
*/
	newent.model = defaultModel;
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

	addEntityToHashTable(entitylist[entityArrayFirstOpen].name, returnid);
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

	addEntityToHashTable(entitylist[entityArrayFirstOpen].name, returnid);
	//todo maybe have entity have a hash variable, so i dont have to calculate it again if i want to delete... maybe
	if(entityArrayLastTaken < entityArrayFirstOpen) entityArrayLastTaken = entityArrayFirstOpen;
//	printf("entityarraysize = %i\n", entityArraySize);
//	printf("entitycount = %i\n", entitycount);

	return &entitylist[entityArrayFirstOpen];

}
