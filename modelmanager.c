#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>

#include "globaldefs.h"
#include "texturemanager.h"
#include "modelmanager.h"

int modelnumber = 0;
model_t *modellist;

int initModelSystem(void){
	model_t none = {"default", findTextureGroupByName("default"), 0};
	if(modellist) free(modellist);
	modellist = malloc(modelnumber * sizeof(model_t));
	if(!modellist) memset(modellist, 0 , modelnumber * sizeof(model_t));
	addModelToList(none);
	return TRUE;
}
int addModelToList(model_t model){
	int current = modelnumber;
	modelnumber++;
	modellist = realloc(modellist, modelnumber*sizeof(model_t));
	modellist[current] = model;
	return current;
}
model_t * findModelByName(char * name){
	int i;
	for (i=0; i<modelnumber; i++){
		if(!strcmp(name, modellist[i].name)) return &modellist[i];
	}
	return &modellist[0];
}
