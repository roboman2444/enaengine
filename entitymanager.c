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

int entitycount = 0;
int entityArrayFirstOpen = 0;
int entityArrayLastTaken = 0; // may not be used
int entityArraySize = 0;
int entitiesOK = 0;
entity_t *entitylist;

int initEntitySystem(void){
	//todo have it figure out screen aspect for the default
			//	name	id	width	height	aspect	fov	texid
	if(entitylist) free(entitylist);
	entitylist = malloc(entitycount * sizeof(entity_t));
	if(!entitylist) memset(entitylist, 0 , entitycount * sizeof(entity_t));
	addEntityRINT("default");
	entitiesOK = TRUE;
	return TRUE; // todo error check
}
int deleteEntity(int id){
	int entityindex = (id & 0xFFFF);
	entity_t * ent = &entitylist[entityindex];
	if(ent->myid != id) return FALSE;
	ent->type = 0;
	if(ent->name) free(ent->name);
	ent->name = 0;
	if(entityindex < entityArrayFirstOpen) entityArrayFirstOpen = entityindex;
	for(; entityArrayLastTaken > 0 && !entitylist[entityArrayFirstOpen].type; entityArrayLastTaken--);
	return TRUE;
}
entity_t * returnById(int id){
//	int entityspawncount = (id >> 16);
	int entityindex = (id & 0xFFFF);
	entity_t * ent = &entitylist[entityindex];
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
	if(entityArrayLastTaken < entityArrayFirstOpen) entityArrayLastTaken = entityArrayFirstOpen;
//	printf("entityarraysize = %i\n", entityArraySize);
//	printf("entitycount = %i\n", entitycount);

	return &entitylist[entityArrayFirstOpen];

}

entity_t * findEntityByName(char * name){
	//todo
	return &entitylist[0];
}
