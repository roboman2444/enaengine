#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>
#include <sys/stat.h> //todo filesys

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
model_t createAndLoadModel(char * name){
	char *statictypes[] = {".obj"}; //todo filesys
	char *animtypes[] = {".dpm"}; //todo filesys //todo

	model_t m;
	m.type = 0; // error
	char * filename = malloc(200);
	struct stat s;
	int n;
	for(n = 0; n < sizeof(statictypes) &&  statictypes[n]; n++){
		sprintf(filename, "%s%s", name, statictypes[n]);
		if(!stat(filename, &s)){ //if file exists... i guess
			//TODO CALL SOME SORTA LOADING FUNCTION
			m.type = 1;
			m.name = malloc(sizeof(name));
			strcpy(m.name, name);
			free(filename);
			return m;
		}
	}
	for(n = 0; n < sizeof(animtypes) &&  animtypes[n]; n++){
		sprintf(filename, "%s%s", name, animtypes[n]);
		if(!stat(filename, &s)){ //if file exists... i guess
			//TODO CALL SOME SORTA LOADING FUNCTION... ANIMATED
			m.type = 2;
			m.name = malloc(sizeof(name));
			strcpy(m.name, name);
			free(filename);
			return m;
		}
	}
	if(!m.type)/*some sorta error*/;

	free(filename);
	return m;
}
