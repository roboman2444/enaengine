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
#include "gamecodemanager.h"

#include "mathlib.h"
#include "console.h"

#include "cvarmanager.h"
#include "filemanager.h"
#include "stringlib.h"
#include "gamecodeincludes.h"

#include <dlfcn.h> //todo move to sys

int gamecodeOK;
int tGameTime = 0;

gcallheader_t *gc = 0;
static void * game_lib;
ecallheader_t ec;

void sys_unloadGameAPI(void){ // todo move to sys
	if(game_lib)dlclose(game_lib);
	game_lib = 0;
}

gcallheader_t * sys_getGameAPI(ecallheader_t *ec){ //todo move to sys
	if(game_lib){ console_printf("Error: game DLL still open, cant reload\n"); return FALSE;}


	//todo search for it, cvar it, etc
	game_lib = dlopen("./gamecode.so", RTLD_NOW);

	if(!game_lib){ console_printf("Error opening gamecode.so, file not found\n"); return FALSE;}
	void *(*dllSetupCallbacks) (void*);
	dllSetupCallbacks = (void *) dlsym (game_lib, "setupGameCodeCallbacks");
	if(!dllSetupCallbacks){
		console_printf("Error opening gamecode.so\n");
		sys_unloadGameAPI();
		return FALSE;
	}
	return dllSetupCallbacks(ec);
}



int setupGameCodeCallbacks(void){
	ec.console_printf = console_printf;
	ec.console_nprintf = console_nprintf;

	ec.cvar_register = cvar_register;
	ec.cvar_unregister = cvar_unregister;
	ec.cvar_nameset = cvar_nameset;
	ec.cvar_pset = cvar_pset;
	ec.cvar_idset = cvar_idset;
	ec.cvar_returnById = cvar_returnById;
	ec.cvar_findByNameRPOINT = cvar_findByNameRPOINT;
	ec.cvar_findByNameRINT = cvar_findByNameRINT;

	ec.entity_findByNameRPOINT = entity_findByNameRPOINT;
	ec.entity_findByNameRINT = entity_findByNameRINT;
	ec.entity_findAllByNameRPOINT = entity_findAllByNameRPOINT;
	ec.entity_findAllByNameRINT = entity_findAllByNameRINT;
	ec.entity_returnById = entity_returnById;
	ec.entity_addRPOINT = entity_addRPOINT;
	ec.entity_addRINT = entity_addRINT;
	ec.entity_delete = entity_delete;

	ec.file_loadString = file_loadString;
	ec.file_loadStringNoLength = file_loadStringNoLength;

	ec.light_addRINT = light_addRINT;
	ec.light_addRPOINT = light_addRPOINT;

	ec.shader_createAndAddRINT = shader_createAndAddRINT;

	ec.string_toVec = string_toVec;

	ec.texture_createAndAddGroupRINT = texture_createAndAddGroupRINT;

	ec.model_createAndAddRINT = model_createAndAddRINT;


	gc = sys_getGameAPI(&ec);
	if(!gc){
		console_printf("Error: could not load game code\n");
		return FALSE;
	}
	//todo implement a new api checking formula, maybe ranges of compatible
	if(gc->apiver != GAMECODEINCLUDEVERSION){
		console_printf("Error: Gamecode version is %i, engine version is %i, not compatible\n", gc->apiver, GAMECODEINCLUDEVERSION);
		return FALSE;
	}
	if(!gc->initgame){
		console_printf("Error: Gamecode does not have an initgame function\n");
		return FALSE;
	}
	//todo
	return TRUE;
}
extern int calcEntAttachMat(entity_t *e);
extern int recalcEntBBox(entity_t *e);

int initGameCodeSystem(void){
	entity_init();
	if(!entity_ok){
		gamecodeOK = FALSE;
		return FALSE;
	}
	if(!setupGameCodeCallbacks()){
		gamecodeOK = FALSE;
		return FALSE; //todo something
	}

	loadWorld("world");
	loadWorld("world2");

	gc->initgame();

	entity_pruneList();
	gamecodeOK = TRUE;
	return TRUE; // todo error check
}
int recalcEntBBox(entity_t * e){
	model_t * m = model_returnById(e->modelid);
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
		entity_t * attacher = entity_returnById(e->attachmentid);
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
	for(i = 0; i <= entity_arraylasttaken; i++){
		entity_t * e = &entity_list[i];
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
	for(i = 0; i <= entity_arraylasttaken; i++){
		entity_t *e = &entity_list[i];
		if(!e->type) continue;
		calcEntAttachMat(e);
		if(e->needsmatupdate || e->needsbboxupdate) recalcEntBBox(e);
	}
	//todo maybe convert to an entity "carry" system instead of a light attach system
	lightLoop();
	for(i = 0; i <= entity_arraylasttaken; i++){// make sure they dont update again
		entity_list[i].needsmatupdate = FALSE;
		entity_list[i].needsbboxupdate = FALSE;
	}


//		if(e->think && e->nextthink <= tGameTime){
//			e->think();
//		}


	//ent gamecode

}
