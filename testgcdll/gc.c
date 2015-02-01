//local includes
#include "gc.h"
#include "../dllincludeme.h"
#include <dlfcn.h> //todo move to sys


gcallheader_t gc;
ecallheader_t *ec;
int initgame(void){

	entity_t * entcampointer = ec->entity_addRPOINT("campointer");
		entcampointer->type = 2;
		entcampointer->modelid = ec->model_createAndAddRINT("coil");
		entcampointer->shaderid = ec->shader_createAndAddRINT("deferredmodel");
		entcampointer->texturegroupid = ec->texture_createAndAddGroupRINT("coil");
		entcampointer->shaderperm = 7;
		entcampointer->flags = 1;
		entcampointer->angle[0] = 30;

		entcampointer->scale = 1.0;
		entcampointer->needsmatupdate = TRUE;



	entity_t * enthat = ec->entity_addRPOINT("hat");
		enthat->type = 2;
		enthat->pos[1] = 8.7;
		enthat->pos[0] = -2.5;
		enthat->scale = .5;
		enthat->angle[1] = -45.0;
		enthat->anglevel[0] = 45.0;
		enthat->needsmatupdate = TRUE;
		enthat->modelid = ec->model_createAndAddRINT("teapot");
		enthat->shaderid = ec->shader_createAndAddRINT("deferredmodel");
		enthat->texturegroupid = 0;
	entity_t * entcoil = ec->entity_addRPOINT("coil");
		entcoil->type = 2;
		entcoil->pos[2] = 10.0;
		entcoil->anglevel[2] = 360.0;
		entcoil->needsmatupdate = TRUE;
		entcoil->shaderid = ec->shader_createAndAddRINT("deferredmodel");
		entcoil->modelid = ec->model_createAndAddRINT("bunny");
//		entcoil->scale = 0.1;
		entcoil->modelid = ec->model_createAndAddRINT("coil");
		entcoil->texturegroupid = ec->texture_createAndAddGroupRINT("coil");
//		entcoil->texturegroupid = ec->texture_createAndAddGroupRINT("bunny");
		entcoil->attachmentid = ec->entity_findByNameRINT("hat");
		entcoil->shaderperm = 1;

	int tempid = entcoil->myid;

	entity_t * entlightoffset = ec->entity_addRPOINT("lightoffset");
		entlightoffset->pos[2] = 3.0f;
		entlightoffset->pos[1] = 3.0f;
		entlightoffset->needsmatupdate = TRUE;
		entlightoffset->type = 1;
		entlightoffset->attachmentid = tempid;
	tempid = entlightoffset->myid;

	light_t * splight = ec->light_addRPOINT("spot");
		splight->type = 2;
		splight->attachmentid = tempid;
		splight->needsupdate = 3;
		splight->scale = 20.0f;
		splight->fovx = 60;
		splight->fovy = 60;
		splight->near = 0.1;
		splight->angle[0] = 45.0;
/*

	light_t * light = ec->light_addRPOINT("light");
		light->type = 1;
		light->attachmentid = tempid;
		light->needsupdate = 1;
		light->scale = 20.0f;
*/
	entity_t * enttinydragon = ec->entity_addRPOINT("tinydragon");
		enttinydragon->type = 2;
		enttinydragon->pos[1] = 3.0f;
		enttinydragon->scale = 0.2f;
		enttinydragon->needsmatupdate = TRUE;
		enttinydragon->modelid = ec->model_createAndAddRINT("dragon");
//		enttinydragon->modelid = ec->model_createAndAddRINT("coil");
		enttinydragon->shaderid = ec->shader_createAndAddRINT("deferredmodel");
		enttinydragon->texturegroupid = 0;//findtexturegroupidByName("coil");
		enttinydragon->attachmentid = ec->entity_findByNameRINT("coil");

	enttinydragon = ec->entity_addRPOINT("tinydragon");
		enttinydragon->type = 2;
		enttinydragon->pos[1] = -3.0f;
		enttinydragon->angle[2] = 180.0f;
		enttinydragon->scale = 0.2f;
		enttinydragon->needsmatupdate = TRUE;
		enttinydragon->shaderid = ec->shader_createAndAddRINT("deferredmodel");
		enttinydragon->modelid = ec->model_createAndAddRINT("dragon");
//		enttinydragon->modelid = model_createAndAddRINT("coil");
		enttinydragon->texturegroupid = 0;//findtexturegroupidByName("coil");
		enttinydragon->attachmentid = ec->entity_findByNameRINT("coil");
		enttinydragon->texturegroupid = 1;

	enttinydragon = ec->entity_addRPOINT("tinydragon");
		enttinydragon->type = 2;
		enttinydragon->pos[2] = 3.0f;
		enttinydragon->angle[2] = 90.0f;
		enttinydragon->scale = 0.2f;
		enttinydragon->needsmatupdate = TRUE;
		enttinydragon->shaderid = ec->shader_createAndAddRINT("deferredmodel");
//		enttinydragon->modelid = model_createAndAddRINT("coil");
		enttinydragon->modelid = ec->model_createAndAddRINT("dragon");
		enttinydragon->texturegroupid = 0;//findtexturegroupidByName("coil");
		enttinydragon->attachmentid = ec->entity_findByNameRINT("coil");
		enttinydragon->shaderperm = 0;

	enttinydragon = ec->entity_addRPOINT("tinydragon");
		enttinydragon->type = 2;
		enttinydragon->pos[2] = -3.0f;
		enttinydragon->angle[2] = -90.0f;
		enttinydragon->scale = 0.2f;
		enttinydragon->needsmatupdate = TRUE;
		enttinydragon->modelid = ec->model_createAndAddRINT("dragon");
//		enttinydragon->modelid = model_createAndAddRINT("coil");
		enttinydragon->shaderid = ec->shader_createAndAddRINT("deferredmodel");
		enttinydragon->texturegroupid = 0;//findtexturegroupidByName("coil");
		enttinydragon->attachmentid = ec->entity_findByNameRINT("coil");
		enttinydragon->shaderperm = 0;

	int i;
	tempid = 0;
	for(i = 0; i < 100; i++){
		enttinydragon = ec->entity_addRPOINT("bunny");
		enttinydragon->type = 2;
		enttinydragon->pos[0] = 5.0f;
		enttinydragon->pos[2] = -2.0f;
		enttinydragon->anglevel[1] = 20.0f;
		enttinydragon->scale = 1.0f;
		enttinydragon->needsmatupdate = TRUE;
		enttinydragon->modelid = ec->model_createAndAddRINT("cube2");
//		enttinydragon->modelid = model_createAndAddRINT("bunny2");
//		enttinydragon->modelid = model_createAndAddRINT("coil");
		enttinydragon->shaderid = ec->shader_createAndAddRINT("deferredmodel");
		enttinydragon->texturegroupid = ec->texture_createAndAddGroupRINT("bunny");
		enttinydragon->shaderperm = 1;
		enttinydragon->attachmentid = tempid;
		tempid = enttinydragon->myid;


	light_t * light = ec->light_addRPOINT("light");
		light->type = 1;
		light->attachmentid = enttinydragon->myid;
		light->needsupdate = 1;
		light->scale = 5.0f;

	}
	light_t * slight = ec->light_addRPOINT("spot");
		slight->type = 2;
		slight->needsupdate = 3;
		slight->scale = 10.0f;
		slight->fovx = 90;
		slight->fovy = 90;
		slight->near = 0.1;
		slight->far = 10.0;



#ifdef EXTRALIGHTS
	srand(103010);
	for(i = 0; i < 2000; i++){
		light = ec->light_addRPOINT("light");
		light->type = 1;
		light->needsupdate = 1;
		light->scale = 50.0f;

		light->pos[0] = (rand()/(double)RAND_MAX -0.5) * 1000.0;
		light->pos[1] = 5.0;
		light->pos[2] = (rand()/(double)RAND_MAX -0.5) * 1000.0;
	}
#endif


	return TRUE; // todo error check
}


gcallheader_t* setupGameCodeCallbacks(ecallheader_t *myec){
	ec = myec;
	if(!ec) return FALSE;
	gc.apiver = GAMECODEINCLUDEVERSION;
	gc.initgame = initgame;
	return &gc;
}

