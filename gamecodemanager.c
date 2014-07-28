//global includes
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
#include "viewportmanager.h"
#include "lightmanager.h"

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

//#define RESAVEWORLD

#ifdef RESAVEWORLD
	entity_t * entdragon = addEntityRPOINT("dragon");
		entdragon->type = 2;
		entdragon->modelid = createAndAddModelRINT("dragon");
		entdragon->texturegroupid = 0;
		entdragon->shaderid = createAndAddShaderRINT("deferredmodel");
		entdragon->shaderperm = 0;
		entdragon->flags = 1;

		calcEntAttachMat(entdragon); // needed because i add it to the world, and the mat needs to be updated beforehand
		recalcEntBBox(entdragon); // needed because this is added to the world before the gamecode runs

		addEntityToWorld(entdragon->myid);
		deleteEntity(entdragon->myid);
	entity_t *entteapot = addEntityRPOINT("teapot");
		entteapot->type = 2;
		entteapot->pos[0] = 10.0f;
		entteapot->needsmatupdate = TRUE;
		entteapot->modelid = createAndAddModelRINT("teapot");
		entteapot->shaderid = createAndAddShaderRINT("deferredmodel");
		entteapot->texturegroupid = 0;
		entteapot->flags = 1;

		calcEntAttachMat(entteapot); // needed because i add it to the world, and the mat needs to be updated beforehand
		recalcEntBBox(entteapot); // needed because this is added to the world before the gamecode runs

		addEntityToWorld(entteapot->myid);
		deleteEntity(entteapot->myid);


	entity_t * entfloor = addEntityRPOINT("floor");
		entfloor->type = 2;
		entfloor->modelid = createAndAddModelRINT("cube2");
		entteapot->needsmatupdate = TRUE;
		entfloor->texturegroupid = 0;
		entfloor->shaderid = createAndAddShaderRINT("deferredmodel");
		entfloor->flags = 1;

		entfloor->pos[1] = -100.0f;
		entfloor->scale = 100.0f;

		calcEntAttachMat(entfloor); // needed because i add it to the world, and the mat needs to be updated beforehand
		recalcEntBBox(entfloor); // needed because this is added to the world before the gamecode runs

		addEntityToWorld(entfloor->myid);
		deleteEntity(entfloor->myid);



	saveWorld("world");
	deleteWorld();
#endif




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
		enthat->shaderid = createAndAddShaderRINT("deferredmodel");
		enthat->texturegroupid = 0;
	entity_t * entcoil = addEntityRPOINT("coil");
		entcoil->type = 2;
		entcoil->pos[2] = 10.0;
		entcoil->anglevel[2] = 360.0;
		entcoil->needsmatupdate = TRUE;
		entcoil->shaderid = createAndAddShaderRINT("deferredmodel");
		entcoil->modelid = createAndAddModelRINT("bunny");
//		entcoil->scale = 0.1;
		entcoil->modelid = createAndAddModelRINT("coil");
		entcoil->texturegroupid = createAndAddTexturegroupRINT("coil");
//		entcoil->texturegroupid = createAndAddTexturegroupRINT("bunny");
		entcoil->attachmentid = findEntityByNameRINT("hat");
		entcoil->shaderperm = 1;

	int tempid = entcoil->myid;

	entity_t * entlightoffset = addEntityRPOINT("lightoffset");
		entlightoffset->pos[2] = 3.0f;
		entlightoffset->pos[1] = 3.0f;
		entlightoffset->needsmatupdate = TRUE;
		entlightoffset->type = 1;
		entlightoffset->attachmentid = tempid;
	tempid = entlightoffset->myid;

	light_t * light = addLightRPOINT("light");
		light->type = 1;
		light->attachmentid = tempid;
		light->needsupdate = 1;
		light->scale = 20.0f;

	entity_t * enttinydragon = addEntityRPOINT("tinydragon");
		enttinydragon->type = 2;
		enttinydragon->pos[1] = 3.0f;
		enttinydragon->scale = 0.2f;
		enttinydragon->needsmatupdate = TRUE;
		enttinydragon->modelid = createAndAddModelRINT("dragon");
//		enttinydragon->modelid = createAndAddModelRINT("coil");
		enttinydragon->shaderid = createAndAddShaderRINT("deferredmodel");
		enttinydragon->texturegroupid = 0;//findtexturegroupidByName("coil");
		enttinydragon->attachmentid = findEntityByNameRINT("coil");

	enttinydragon = addEntityRPOINT("tinydragon");
		enttinydragon->type = 2;
		enttinydragon->pos[1] = -3.0f;
		enttinydragon->angle[2] = 180.0f;
		enttinydragon->scale = 0.2f;
		enttinydragon->needsmatupdate = TRUE;
		enttinydragon->shaderid = createAndAddShaderRINT("deferredmodel");
		enttinydragon->modelid = createAndAddModelRINT("dragon");
//		enttinydragon->modelid = createAndAddModelRINT("coil");
		enttinydragon->texturegroupid = 0;//findtexturegroupidByName("coil");
		enttinydragon->attachmentid = findEntityByNameRINT("coil");
		enttinydragon->texturegroupid = 1;
	enttinydragon = addEntityRPOINT("tinydragon");
		enttinydragon->type = 2;
		enttinydragon->pos[2] = 3.0f;
		enttinydragon->angle[2] = 90.0f;
		enttinydragon->scale = 0.2f;
		enttinydragon->needsmatupdate = TRUE;
		enttinydragon->shaderid = createAndAddShaderRINT("deferredmodel");
//		enttinydragon->modelid = createAndAddModelRINT("coil");
		enttinydragon->modelid = createAndAddModelRINT("dragon");
		enttinydragon->texturegroupid = 0;//findtexturegroupidByName("coil");
		enttinydragon->attachmentid = findEntityByNameRINT("coil");
		enttinydragon->shaderperm = 0;
	enttinydragon = addEntityRPOINT("tinydragon");
		enttinydragon->type = 2;
		enttinydragon->pos[2] = -3.0f;
		enttinydragon->angle[2] = -90.0f;
		enttinydragon->scale = 0.2f;
		enttinydragon->needsmatupdate = TRUE;
		enttinydragon->modelid = createAndAddModelRINT("dragon");
//		enttinydragon->modelid = createAndAddModelRINT("coil");
		enttinydragon->shaderid = createAndAddShaderRINT("deferredmodel");
		enttinydragon->texturegroupid = 0;//findtexturegroupidByName("coil");
		enttinydragon->attachmentid = findEntityByNameRINT("coil");
		enttinydragon->shaderperm = 0;
	int i;


	tempid = 0;
	for(i = 0; i < 100; i++){
		enttinydragon = addEntityRPOINT("bunny");
		enttinydragon->type = 2;
		enttinydragon->pos[0] = 5.0f;
		enttinydragon->pos[2] = -2.0f;
		enttinydragon->anglevel[1] = 20.0f;
		enttinydragon->scale = 1.0f;
		enttinydragon->needsmatupdate = TRUE;
		enttinydragon->modelid = createAndAddModelRINT("cube2");
//		enttinydragon->modelid = createAndAddModelRINT("bunny2");
//		enttinydragon->modelid = createAndAddModelRINT("coil");
		enttinydragon->shaderid = createAndAddShaderRINT("deferredmodel");
		enttinydragon->texturegroupid = createAndAddTexturegroupRINT("bunny");
		enttinydragon->shaderperm = 1;
		enttinydragon->attachmentid = tempid;
		tempid = enttinydragon->myid;


	light_t * light = addLightRPOINT("light");
		light->type = 1;
		light->attachmentid = enttinydragon->myid;
		light->needsupdate = 1;
		light->scale = 5.0f;

	}


#ifdef RESAVEWORLD
	srand(103010);
	for(i = 0; i < 200000; i++){
		enttinydragon = addEntityRPOINT("cube");
		enttinydragon->type = 2;
		enttinydragon->pos[0] = (rand()/(double)RAND_MAX -0.5) * 16384.0;
		enttinydragon->pos[2] = (rand()/(double)RAND_MAX -0.5) * 16384.0;
		enttinydragon->angle[1] = 90.0f;

		enttinydragon->scale = (rand()/(double)RAND_MAX) * 2.0;
//		enttinydragon->scale = 20.0f;
//		enttinydragon->scale = (rand()/(double)RAND_MAX) * vec4length(enttinydragon->pos);
		enttinydragon->needsmatupdate = TRUE;
		enttinydragon->modelid = createAndAddModelRINT("coil");
		enttinydragon->shaderid = createAndAddShaderRINT("deferredmodel");
		enttinydragon->texturegroupid = createAndAddTexturegroupRINT("coil");
		enttinydragon->shaderperm = 7;
		enttinydragon->flags = 1;


		calcEntAttachMat(enttinydragon); // needed because i add it to the world, and the mat needs to be updated beforehand
		recalcEntBBox(enttinydragon); // needed because this is added to the world before the gamecode runs

		addEntityToWorld(enttinydragon->myid);
		deleteEntity(enttinydragon->myid);

	}
	for(i = 0; i < 200000; i++){
		enttinydragon = addEntityRPOINT("cube2");
		enttinydragon->type = 2;
		enttinydragon->pos[0] = (rand()/(double)RAND_MAX -0.5) * 16384.0;
		enttinydragon->pos[2] = (rand()/(double)RAND_MAX -0.5) * 16384.0;

		enttinydragon->scale = (rand()/(double)RAND_MAX) * 4.0;
//		enttinydragon->scale =  20.0f;
		enttinydragon->needsmatupdate = TRUE;
		enttinydragon->modelid = createAndAddModelRINT("cube");
		enttinydragon->shaderid = createAndAddShaderRINT("deferredmodel");
		enttinydragon->flags = 1;


		calcEntAttachMat(enttinydragon); // needed because i add it to the world, and the mat needs to be updated beforehand
		recalcEntBBox(enttinydragon); // needed because this is added to the world before the gamecode runs

		addEntityToWorld(enttinydragon->myid);
		deleteEntity(enttinydragon->myid);

	}

	saveWorld("world2");
	deleteWorld();
#endif
	loadWorld("world");
	loadWorld("world2");


	pruneEntityList();


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
		if(!e->type) continue;
		calcEntAttachMat(e);
		if(e->needsmatupdate || e->needsbboxupdate) recalcEntBBox(e);
	}
	//todo maybe convert to an entity "carry" system instead of a light attach system
	lightLoop();
	for(i = 0; i <= entityArrayLastTaken; i++){// make sure they dont update again
		entitylist[i].needsmatupdate = FALSE;
		entitylist[i].needsbboxupdate = FALSE;
	}


//		if(e->think && e->nextthink <= tGameTime){
//			e->think();
//		}


	//ent gamecode

}
