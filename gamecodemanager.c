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


	entity_t * entdragon = addEntityRPOINT("dragon");
		entdragon->type = 2;
		entdragon->model = createAndAddModel("dragon");
		entdragon->texturegroup = 0;
	entity_t *entteapot = addEntityRPOINT("teapot");
		entteapot->type = 2;
		entteapot->pos[0] = 10.0;
		entteapot->needsmatupdate = TRUE;
		entteapot->model = createAndAddModel("teapot");
		entteapot->texturegroup = 0;
	entity_t * enthat = addEntityRPOINT("hat");
		enthat->type = 2;
		enthat->pos[1] = 8.7;
		enthat->pos[0] = -2.5;
		enthat->scale = .5;
		enthat->angle[1] = -45.0;
		enthat->anglevel[0] = 360.0;
		enthat->needsmatupdate = TRUE;
		enthat->model = createAndAddModel("teapot");
		enthat->texturegroup = 0;
	entity_t * entcoil = addEntityRPOINT("coil");
		entcoil->type = 2;
		entcoil->pos[2] = 10.0;
		entcoil->anglevel[2] = 1080.0;
		entcoil->needsmatupdate = TRUE;
		entcoil->model = createAndAddModel("coil");
		entcoil->texturegroup = 0;//findTextureGroupByName("coil");
//		entcoil->attachmentid = tidhat;
		entcoil->attachmentid = findEntityByNameRINT("hat");
	entity_t * enttinydragon = addEntityRPOINT("tinydragon");
		enttinydragon->type = 2;
		enttinydragon->pos[1] = 3.0;
		enttinydragon->scale = 0.2;
		enttinydragon->needsmatupdate = TRUE;
		enttinydragon->model = createAndAddModel("dragon");
		enttinydragon->texturegroup = 0;//findTextureGroupByName("coil");
		enttinydragon->attachmentid = findEntityByNameRINT("coil");

	deleteEntity(findEntityByNameRINT("hat"));
//	deleteEntity(tidhat);


	gamecodeOK = TRUE;
	return TRUE; // todo error check
}

int calcEntAttachMat(entity_t * e){ //return value is weather e->mat got changed
	if(!e->type) return FALSE;
	if(e->attachmentid){
		entity_t * attacher = returnById(e->attachmentid);
		if(!attacher){
			 e->attachmentid = 0;
		}
		else if (calcEntAttachMat(attacher)){ //dat recursion
			matrix4x4_t tempmat;
//			Matrix4x4_CreateFromQuakeEntity(&tempmat, e->pos[0], e->pos[1], e->pos[2], e->angle[0], e->angle[1], e->angle[2], e->scale);
//			Matrix4x4_CreateFromQuakeEntity(&tempmat, e->pos[0], e->pos[1], e->pos[2], e->angle[0], e->angle[1], e->angle[2], e->scale/attacher->scale);
			Matrix4x4_CreateFromQuakeEntity(&tempmat, e->pos[0]/attacher->scale, e->pos[1]/attacher->scale, e->pos[2]/attacher->scale, e->angle[0], e->angle[1], e->angle[2], e->scale/attacher->scale);

			Matrix4x4_Concat(&e->mat, &attacher->mat, &tempmat);
			e->needsmatupdate = 2;
			return TRUE;
		}
		//todo figure this out...
		else if (e->needsmatupdate & 1){
//		else if (TRUE){
			e->needsmatupdate = 2;
			if(attacher){
				matrix4x4_t tempmat;
	//			Matrix4x4_CreateFromQuakeEntity(&tempmat, e->pos[0], e->pos[1], e->pos[2], e->angle[0], e->angle[1], e->angle[2], e->scale);
//				Matrix4x4_CreateFromQuakeEntity(&tempmat, e->pos[0], e->pos[1], e->pos[2], e->angle[0], e->angle[1], e->angle[2], e->scale/attacher->scale);
				Matrix4x4_CreateFromQuakeEntity(&tempmat, e->pos[0]/attacher->scale, e->pos[1]/attacher->scale, e->pos[2]/attacher->scale, e->angle[0], e->angle[1], e->angle[2], e->scale/attacher->scale);
				//may need to swap order
				Matrix4x4_Concat(&e->mat, &attacher->mat, &tempmat);

				return TRUE;
			} else {
				Matrix4x4_CreateFromQuakeEntity(&e->mat, e->pos[0], e->pos[1], e->pos[2], e->angle[0], e->angle[1], e->angle[2], e->scale);
				return TRUE;
			}
		}
		else if (e->needsmatupdate) return TRUE;
		//else implied
		return FALSE;
	} else if (e->needsmatupdate & 1) {
		Matrix4x4_CreateFromQuakeEntity(&e->mat, e->pos[0], e->pos[1], e->pos[2], e->angle[0], e->angle[1], e->angle[2], e->scale);
		e->needsmatupdate = 2;
		return TRUE;
	} else if(e->needsmatupdate) return TRUE;
	return FALSE;
}
void gameCodeTick(void){ //todo maybe change to float in seconds
	tGameTime+=GCTIMESTEP;
	int i;
	for(i = 0; i <= entityArrayLastTaken; i++){ //todo turn into entityArrayLastTaken
		entity_t * e = &entitylist[i];
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
/*
		if(e->needsmatupdate){
			Matrix4x4_CreateFromQuakeEntity(&e->mat, e->pos[0], e->pos[1], e->pos[2], e->angle[0], e->angle[1], e->angle[2], e->scale);
			e->needsmatupdate = FALSE;
		}
	*/	calcEntAttachMat(e); //does the checking and updating mat anyway...
	}
	for(i = 0; i <= entityArrayLastTaken; i++){// make sure they dont update again
		entitylist[i].needsmatupdate = FALSE;
	}

}
