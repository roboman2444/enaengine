//gloabl includes
#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>
//local includes
#include "globaldefs.h"
#include "matrixlib.h"
#include "texturemanager.h"
#include "vbomanager.h"
#include "modelmanager.h"
#include "shadermanager.h"
#include "entitymanager.h"
#include "worldmanager.h"

int gamecodeOK;
int tGameTime = 0;

int setupGameCodeCallbacks(void){
	//todo
	return TRUE;
}
extern int calcEntAttachMat(entity_t *e);
extern int recalcEntBBox(entity_t *e);

int initGameCodeSystem(void){
	initEntitySystem();
	if(!entitiesOK){
		gamecodeOK = FALSE;
		return FALSE;
	}
	setupGameCodeCallbacks();


	entity_t * entdragon = addEntityRPOINT("dragon");
		entdragon->type = 2;
		entdragon->modelid = createAndAddModelRINT("dragon");
//		entdragon->modelid = createAndAddModelRINT("coil");
		entdragon->texturegroupid = 0;
		entdragon->shaderid = createAndAddShaderRINT("staticmodel");
		entdragon->shaderperm = 1;

		recalcEntBBox(entdragon); // needed because this is added to the world before the gamecode runs

		addEntityToWorld(entdragon->myid);
		deleteEntity(entdragon->myid);
	entity_t *entteapot = addEntityRPOINT("teapot");
		entteapot->type = 2;
		entteapot->pos[0] = 10.0;
		entteapot->needsmatupdate = TRUE;
		entteapot->modelid = createAndAddModelRINT("teapot");
//		entteapot->modelid = createAndAddModelRINT("coil");
		entteapot->shaderid = createAndAddShaderRINT("staticmodel");
		entteapot->texturegroupid = 0;

		calcEntAttachMat(entteapot); // needed because i add it to the world, and the mat needs to be updated beforehand
		recalcEntBBox(entteapot); // needed because this is added to the world before the gamecode runs

		addEntityToWorld(entteapot->myid);
		deleteEntity(entteapot->myid);
	entity_t * enthat = addEntityRPOINT("hat");
		enthat->type = 2;
		enthat->pos[1] = 8.7;
		enthat->pos[0] = -2.5;
		enthat->scale = .5;
		enthat->angle[1] = -45.0;
		enthat->anglevel[0] = 45.0;
		enthat->needsmatupdate = TRUE;
		enthat->modelid = createAndAddModelRINT("teapot");
//		enthat->modelid = createAndAddModelRINT("coil");
		enthat->shaderid = createAndAddShaderRINT("staticmodel");
		enthat->texturegroupid = 0;
	entity_t * entcoil = addEntityRPOINT("coil");
		entcoil->type = 2;
		entcoil->pos[2] = 10.0;
		entcoil->anglevel[2] = 360.0;
		entcoil->needsmatupdate = TRUE;
		entcoil->shaderid = createAndAddShaderRINT("staticmodel");
		entcoil->modelid = createAndAddModelRINT("coil");
		entcoil->texturegroupid = createAndAddTexturegroupRINT("coil");
		entcoil->attachmentid = findEntityByNameRINT("hat");
		entcoil->shaderperm = 4;
	entity_t * enttinydragon = addEntityRPOINT("tinydragon");
		enttinydragon->type = 2;
		enttinydragon->pos[1] = 3.0;
		enttinydragon->scale = 0.2;
		enttinydragon->needsmatupdate = TRUE;
		enttinydragon->modelid = createAndAddModelRINT("dragon");
//		enttinydragon->modelid = createAndAddModelRINT("coil");
		enttinydragon->shaderid = createAndAddShaderRINT("staticmodel");
		enttinydragon->texturegroupid = 0;//findtexturegroupidByName("coil");
		enttinydragon->attachmentid = findEntityByNameRINT("coil");
		enttinydragon->shaderperm = 1;

	enttinydragon = addEntityRPOINT("tinydragon");
		enttinydragon->type = 2;
		enttinydragon->pos[1] = -3.0;
		enttinydragon->angle[2] = 180.0;
		enttinydragon->scale = 0.2;
		enttinydragon->needsmatupdate = TRUE;
		enttinydragon->shaderid = createAndAddShaderRINT("staticmodel");
		enttinydragon->modelid = createAndAddModelRINT("dragon");
//		enttinydragon->modelid = createAndAddModelRINT("coil");
		enttinydragon->texturegroupid = 0;//findtexturegroupidByName("coil");
		enttinydragon->attachmentid = findEntityByNameRINT("coil");
		enttinydragon->texturegroupid = 1;
		enttinydragon->shaderperm = 1;
	enttinydragon = addEntityRPOINT("tinydragon");
		enttinydragon->type = 2;
		enttinydragon->pos[2] = 3.0;
		enttinydragon->angle[2] = 90.0;
		enttinydragon->scale = 0.2;
		enttinydragon->needsmatupdate = TRUE;
		enttinydragon->shaderid = createAndAddShaderRINT("staticmodel");
//		enttinydragon->modelid = createAndAddModelRINT("coil");
		enttinydragon->modelid = createAndAddModelRINT("dragon");
		enttinydragon->texturegroupid = 0;//findtexturegroupidByName("coil");
		enttinydragon->attachmentid = findEntityByNameRINT("coil");
		enttinydragon->shaderperm = 2;
	enttinydragon = addEntityRPOINT("tinydragon");
		enttinydragon->type = 2;
		enttinydragon->pos[2] = -3.0;
		enttinydragon->angle[2] = -90.0;
		enttinydragon->scale = 0.2;
		enttinydragon->needsmatupdate = TRUE;
		enttinydragon->modelid = createAndAddModelRINT("dragon");
//		enttinydragon->modelid = createAndAddModelRINT("coil");
		enttinydragon->shaderid = createAndAddShaderRINT("staticmodel");
		enttinydragon->texturegroupid = 0;//findtexturegroupidByName("coil");
		enttinydragon->attachmentid = findEntityByNameRINT("coil");
		enttinydragon->shaderperm = 2;
	int i;

	int prevatid = 0;
	for(i = 0; i < 100; i++){
		enttinydragon = addEntityRPOINT("bunny");
		enttinydragon->type = 2;
		enttinydragon->pos[0] = 5.0;
		enttinydragon->pos[2] = -2.0;
		enttinydragon->anglevel[1] = 20.0;
		enttinydragon->scale = 1.0;
		enttinydragon->needsmatupdate = TRUE;
		enttinydragon->modelid = createAndAddModelRINT("cube");
//		enttinydragon->modelid = createAndAddModelRINT("coil");
		enttinydragon->shaderid = createAndAddShaderRINT("staticmodel");
		enttinydragon->texturegroupid = createAndAddTexturegroupRINT("bunny");
		enttinydragon->shaderperm = 4;
		enttinydragon->attachmentid = prevatid;
		prevatid = enttinydragon->myid;
	}


	srand(103010);
	for(i = 0; i < 100000; i++){
		enttinydragon = addEntityRPOINT("cube");
		enttinydragon->type = 2;
		enttinydragon->pos[0] = (rand()/(double)RAND_MAX -0.5) * 10000.0;
		enttinydragon->pos[2] = (rand()/(double)RAND_MAX -0.5) * 10000.0;
		enttinydragon->angle[1] = 90.0;

		enttinydragon->scale = (rand()/(double)RAND_MAX) * 1.0;
		enttinydragon->needsmatupdate = TRUE;
		enttinydragon->modelid = createAndAddModelRINT("coil");
		enttinydragon->shaderid = createAndAddShaderRINT("staticmodel");
		enttinydragon->texturegroupid = createAndAddTexturegroupRINT("coil");
		enttinydragon->shaderperm = 4;


		calcEntAttachMat(enttinydragon); // needed because i add it to the world, and the mat needs to be updated beforehand
		recalcEntBBox(enttinydragon); // needed because this is added to the world before the gamecode runs

		addEntityToWorld(enttinydragon->myid);
		deleteEntity(enttinydragon->myid);

	}
	for(i = 0; i < 100000; i++){
		enttinydragon = addEntityRPOINT("cube");
		enttinydragon->type = 2;
		enttinydragon->pos[0] = (rand()/(double)RAND_MAX -0.5) * 10000.0;
		enttinydragon->pos[2] = (rand()/(double)RAND_MAX -0.5) * 10000.0;

		enttinydragon->scale = (rand()/(double)RAND_MAX) * 1.0;
		enttinydragon->needsmatupdate = TRUE;
		enttinydragon->modelid = createAndAddModelRINT("cube");
		enttinydragon->shaderid = createAndAddShaderRINT("staticmodel");


		calcEntAttachMat(enttinydragon); // needed because i add it to the world, and the mat needs to be updated beforehand
		recalcEntBBox(enttinydragon); // needed because this is added to the world before the gamecode runs

		addEntityToWorld(enttinydragon->myid);
		deleteEntity(enttinydragon->myid);

	}



	gamecodeOK = TRUE;
	return TRUE; // todo error check
}
int recalcEntBBox(entity_t * e){
	model_t * m = returnModelById(e->modelid);
	if(!m) return FALSE;
	int i;
	e->bbox[0] = -3.4028e+38;
	e->bbox[1] = 3.4028e+38;
	e->bbox[2] = -3.4028e+38;
	e->bbox[3] = 3.4028e+38;
	e->bbox[4] = -3.4028e+38;
	e->bbox[5] = 3.4028e+38;

	for(i = 0; i < 8; i++){
		int oneplace = i*3;
		Matrix4x4_Transform(&e->mat, &m->bboxp[oneplace], &e->bboxp[oneplace]);

		if(e->bboxp[oneplace] > e->bbox[0]) e->bbox[0] = e->bboxp[oneplace];
		else if(e->bboxp[oneplace] < e->bbox[1]) e->bbox[1] = e->bboxp[oneplace];
		if(e->bboxp[oneplace+1] > e->bbox[2]) e->bbox[2] = e->bboxp[oneplace+1];
		else if(e->bboxp[oneplace+1] < e->bbox[3]) e->bbox[3] = e->bboxp[oneplace+1];
		if(e->bboxp[oneplace+2] > e->bbox[4]) e->bbox[4] = e->bboxp[oneplace+2];
		else if(e->bboxp[oneplace+2] < e->bbox[5]) e->bbox[5] = e->bboxp[oneplace+2];
	}
	e->needsbboxupdate = FALSE;
	return TRUE;
}

int calcEntAttachMat(entity_t * e){ //return value is weather e->mat got changed
	if(!e->type) return FALSE;
	if(e->attachmentid){
		entity_t * attacher = returnEntityById(e->attachmentid);
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

	//ent phys
	for(i = 0; i <= entityArrayLastTaken; i++){
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
	}
	for(i = 0; i <= entityArrayLastTaken; i++){
		entity_t *e = &entitylist[i];
		calcEntAttachMat(e);
		if(e->needsmatupdate || e->needsbboxupdate) recalcEntBBox(e);
	}
	for(i = 0; i <= entityArrayLastTaken; i++){// make sure they dont update again
		entitylist[i].needsmatupdate = FALSE;
	}


//		if(e->think && e->nextthink <= tGameTime){
//			e->think();
//		}


	//ent gamecode

}
