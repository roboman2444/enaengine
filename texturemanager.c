#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>

#include "globaldefs.h"
#include "texturemanager.h"

int texturegroupnumber = 0; // first one is error one
texturegroup_t *texturegrouplist; //todo have a sperate dynamic and static lists

int initTextureSystem(void){
	if(texturegrouplist) free(texturegrouplist);
	texturegrouplist = malloc(texturegroupnumber * sizeof(texturegroup_t));
	if(!texturegrouplist) memset(texturegrouplist, 0, texturegroupnumber * sizeof(texturegroup_t));
	addTextureGroupToList(createTextureGroup("default", 0));
	//todo error checking
	return TRUE;
}
texturegroup_t createTextureGroup(char * name, int num){
	texturegroup_t texgroup;
	texgroup.textures = malloc(num*sizeof(texture_t));
	texgroup.num = num;
	texgroup.name = name;
	return texgroup;
}

int addTextureGroupToList(texturegroup_t texgroup){
	int current = texturegroupnumber;
	texturegroupnumber++;
	texturegrouplist = realloc(texturegrouplist, texturegroupnumber*sizeof(texturegroup_t));
	texturegrouplist[current] = texgroup;
	return current;
}
texturegroup_t findTextureGroupByName(char * name){
	int i;
	for(i = 0; i < texturegroupnumber; i++){
		if(!strcmp(name, texturegrouplist[i].name)) return texturegrouplist[i];
	}
	return texturegrouplist[0];
}
