//gloabl includes
#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>
//local includes
#include "globaldefs.h"
#include "matrixlib.h"
#include "texturemanager.h"
#include "vbomanager.h"
#include "modelmanager.h"
#include "entitymanager.h"

int gamecodeOK;
int tGameTime = 0;

int setupGameCodeCallbacks(void){
	//todo
	return TRUE;
}

int initGameCodeSystem(void){
	initEntitySystem();
	if(!entitiesOK){
		gamecodeOK = FALSE;
		return FALSE;
	}
	setupGameCodeCallbacks();


	entity_t entdragon = createEntity("dragon");
		entdragon.type = 1;
		entdragon.model = findModelByName("dragon");
		entdragon.texturegroup = 0;

	addEntityToList(entdragon);
	entity_t entteapot = createEntity("teapot");
		entteapot.type = 1;
		entteapot.pos[0] = 10.0;
		entteapot.needsmatupdate = TRUE;
		entteapot.model = findModelByName("teapot");
		entteapot.texturegroup = 0;
	addEntityToList(entteapot);
	entity_t entcoil = createEntity("coil");
		entcoil.type = 1;
		entcoil.pos[2] = 10.0;
		entcoil.needsmatupdate = TRUE;
		entcoil.model = findModelByName("coil");
		entcoil.texturegroup = findTextureGroupByName("coil");
	addEntityToList(entcoil);

	entity_t enthat = createEntity("hat");
		enthat.type = 1;
		enthat.pos[1] = 8.7;
		enthat.pos[0] = -2.5;
		enthat.anglevel[0] = 360.0;
		enthat.needsmatupdate = TRUE;
		enthat.model = findModelByName("teapot");
		enthat.texturegroup = 0;
	addEntityToList(enthat);



	gamecodeOK = TRUE;
	return TRUE; // todo error check
}
void gameCodeTick(void){ //todo maybe change to float in seconds
	tGameTime+=GCTIMESTEP;
	int i;
	for(i = 0; i < entitynumber; i++){
		entity_t * e = entitylist[i];
		if(!e->type) continue;
		if(e->vel[0] || e->vel[1] || e->vel[2]){
			e->pos[0] += e->vel[0] * GCTIMESTEPSECONDS;
			e->pos[1] += e->vel[1] * GCTIMESTEPSECONDS;
			e->pos[2] += e->vel[2] * GCTIMESTEPSECONDS;
			e->needsmatupdate = TRUE;
		}
		if(e->anglevel[0] || e->anglevel[1] || e->anglevel[2]){
			e->angle[0] += e->anglevel[0] * GCTIMESTEPSECONDS;
			e->angle[1] += e->anglevel[1] * GCTIMESTEPSECONDS;
			e->angle[2] += e->anglevel[2] * GCTIMESTEPSECONDS;
			e->needsmatupdate = TRUE;
		}
		if(e->think && e->nextthink <= tGameTime){
			e->think();
		}
		//todo check if ents are touching!
		if(e->needsmatupdate){
			Matrix4x4_CreateFromQuakeEntity(&e->mat, e->pos[0], e->pos[1], e->pos[2], e->angle[0], e->angle[1], e->angle[2], e->scale);
			e->needsmatupdate = FALSE;
		}
	}
}
