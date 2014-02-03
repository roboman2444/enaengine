//gloabl includes
#include <GL/glew.h>
#include <GL/gl.h>

//local includes
#include "globaldefs.h"
#include "matrixlib.h"
#include "texturemanager.h"
#include "vbomanager.h"
#include "modelmanager.h"
#include "entitymanager.h"

int entitynumber = 0; //the first is an error one/screen
int entitiesOK = 0;
entity_t **entitylist;
entity_t *defaultEntity;

int initEntitySystem(void){
	//todo have it figure out screen aspect for the default
			//	name	id	width	height	aspect	fov	texid
	entity_t none = {"default"};
	if(entitylist) free(entitylist);
	entitylist = malloc(entitynumber * sizeof(entity_t *));
	if(!entitylist) memset(entitylist, 0 , entitynumber * sizeof(entity_t *));
	defaultEntity = addEntityToList(none);
	entitiesOK = TRUE;
	return TRUE; // todo error check
}
entity_t * addEntityToList(entity_t entity){ //todo have this return a entity pointa
	entity_t * pointentity = malloc(sizeof(entity_t));
	*pointentity = entity;
	int current = entitynumber;
	entitynumber++;
	entitylist = realloc(entitylist, entitynumber * sizeof(entity_t *));
	entitylist[current] = pointentity;
	//entitylist[current].name = malloc(sizeof(*entity.name));
	//strcpy(entitylist[current].name, entity.name);
	return pointentity;
}

entity_t createEntity(char * name){
	entity_t newent;
	newent.type = 0;
	newent.name = malloc(strlen(name)+1);
	strcpy(newent.name, name);
//	newent.pos = {0.0, 0.0, 0.0};
//	newent.angle = {0.0, 0.0, 0.0};
	newent.model = defaultModel;
	newent.texturegroup = defaultTextureGroup;
	newent.scale = 0;
	Matrix4x4_CreateFromQuakeEntity(&newent.mat, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
	return newent;
//todo
}

entity_t * returnEntity(int id){
	if(id >= entitynumber) return entitylist[0];
	return entitylist[id];
}
entity_t * findEntityByName(char * name){
	int i;
	for(i = 0; i<entitynumber; i++){
		if(!strcmp(name, entitylist[i]->name)) return entitylist[i];
	}
	return entitylist[0];
}
