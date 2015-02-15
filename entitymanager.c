//global includes
#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>
//local includes
#include "globaldefs.h"
#include "hashtables.h"
#include "matrixlib.h"
#include "entitymanager.h"
#include "stringlib.h"

int entity_count = 0;
int entity_arrayfirstopen = 0;
int entity_arraylasttaken = -1;
int entity_arraysize = 0;
int entity_ok = 0;
entity_t *entity_list;
hashbucket_t entityhashtable[MAXHASHBUCKETS];


int entity_init(void){
	memset(entityhashtable, 0, MAXHASHBUCKETS * sizeof(hashbucket_t));
	if(entity_list) free(entity_list);
	entity_list = 0;
//	entity_list = malloc(entity_count * sizeof(entity_t));
//	if(!entity_list) memset(entity_list, 0 , entity_count * sizeof(entity_t));
	entity_addRINT("default");
	entity_ok = TRUE;
	return TRUE; // todo error check
}
entitylistpoint_t entity_findAllByNameRPOINT(const char * name){
	entitylistpoint_t ret;
	int hash = getHash(name);
	hashbucket_t * hb = &entityhashtable[hash];
	if(!hb->name) return ret;
        for(; hb; hb = hb->next){
		if(string_testEqual(hb->name, name)){
//			return entity_returnById(hb->id);
			ret.count++;
			ret.list = realloc(ret.list, ret.count * sizeof(entity_t *));
			ret.list[ret.count-1] = entity_returnById(hb->id);
		}
        }
	return ret;
}
entitylistint_t entity_findAllByNameRINT(const char * name){
	entitylistint_t ret;
	int hash = getHash(name);
	hashbucket_t * hb = &entityhashtable[hash];
	if(!hb->name) return ret;
        for(; hb; hb = hb->next){
		if(string_testEqual(hb->name, name)){
//			return entity_returnById(hb->id);
			ret.count++;
			ret.list = realloc(ret.list, ret.count * sizeof(int));
			ret.list[ret.count-1] = hb->id;
		}
        }
	return ret;
}

entity_t * entity_findByNameRPOINT(const char * name){ //todo write a function that can find ALL entities with name
	return entity_returnById(findByNameRINT(name, entityhashtable));
}
int entity_findByNameRINT(const char * name){
	return findByNameRINT(name, entityhashtable);
}

int entity_delete(const int id){
	int entityindex = (id & 0xFFFF);
	entity_t * ent = &entity_list[entityindex];
	if(ent->myid != id) return FALSE;
	if(!ent->name) return FALSE;
	deleteFromHashTable(ent->name, id, entityhashtable);
	free(ent->name);
	memset(ent, 0, sizeof(entity_t));
//	ent->type = 0;
//	ent->model = 0;
//	ent->name = 0;
//	ent->myid = 0;
	if(entityindex < entity_arrayfirstopen) entity_arrayfirstopen = entityindex;
	for(; entity_arraylasttaken > 0 && !entity_list[entity_arraylasttaken].type; entity_arraylasttaken--);
	return TRUE;
}
entity_t * entity_returnById(const int id){
//	int entityspawncount = (id >> 16);
	int entityindex = (id & 0xFFFF);
	entity_t * ent = &entity_list[entityindex];
	if(!ent->type) return FALSE;
	if(ent->myid == id) return ent;
	return FALSE;
}
entity_t createEntity(const char * name){
	entity_t newent;
	memset(&newent, 0, sizeof(entity_t));
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
	newent.texturegroup = 0;
*/
	newent.scale = 1.0;
	Matrix4x4_CreateFromQuakeEntity(&newent.mat, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
//	entity_count++;
//	newent.spawncount = entity_count;
	return newent;
//todo
}



int entity_addRINT(const char * name){
	entity_count++;
	for(; entity_arrayfirstopen < entity_arraysize && entity_list[entity_arrayfirstopen].type; entity_arrayfirstopen++);
	if(entity_arrayfirstopen == entity_arraysize){	//resize
		entity_arraysize++;
		entity_list = realloc(entity_list, entity_arraysize * sizeof(entity_t));
	}
	entity_list[entity_arrayfirstopen] = createEntity(name);
	int returnid = (entity_count << 16) | entity_arrayfirstopen;
	entity_list[entity_arrayfirstopen].myid = returnid;

	addToHashTable(entity_list[entity_arrayfirstopen].name, returnid, entityhashtable);
	if(entity_arraylasttaken < entity_arrayfirstopen) entity_arraylasttaken = entity_arrayfirstopen; //todo redo
	if(returnid == 0) printf("yes, i can\n");
	return returnid;
}
entity_t * entity_addRPOINT(const char * name){
	entity_count++;
	for(; entity_arrayfirstopen < entity_arraysize && entity_list[entity_arrayfirstopen].type; entity_arrayfirstopen++);
	if(entity_arrayfirstopen == entity_arraysize){	//resize
		entity_arraysize++;
		entity_list = realloc(entity_list, entity_arraysize * sizeof(entity_t));
	}
	entity_list[entity_arrayfirstopen] = createEntity(name);
	int returnid = (entity_count << 16) | entity_arrayfirstopen;
	entity_list[entity_arrayfirstopen].myid = returnid;

	addToHashTable(entity_list[entity_arrayfirstopen].name, returnid, entityhashtable);
	//todo maybe have entity have a hash variable, so i dont have to calculate it again if i want to delete... maybe
	if(entity_arraylasttaken < entity_arrayfirstopen) entity_arraylasttaken = entity_arrayfirstopen;
//	printf("entity_arraysize = %i\n", entity_arraysize);
//	printf("entity_count = %i\n", entity_count);
	if(entity_list[entity_arrayfirstopen].myid == 0) printf("yes, i can\n");

	return &entity_list[entity_arrayfirstopen];

}
void entity_pruneList(void){
	if(entity_arraysize == entity_arraylasttaken+1) return;
	entity_arraysize = entity_arraylasttaken+1;
	entity_list = realloc(entity_list, entity_arraysize * sizeof(entity_t));
}
