#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>

#include "globaldefs.h"
#include "texturemanager.h"

#include "SDL_image.h"
#include "SDL.h"

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
//todo something to load all textures for group *name
texture_t loadTexture(char * filepath){
	texture_t tex;
	SDL_Surface *teximage;
	//todo better errorchecking
//	if(!(teximage = IMG_Load(filepath))) return FALSE; //todo make return a "defualt"
	teximage = IMG_Load(filepath);
	tex.width = teximage->w;
	tex.height = teximage->h;
//	int size = teximage->format->BytesPerPixel * tex.width * tex.height;

	GLint texformat = GL_RGB;
	switch(teximage->format->BytesPerPixel){//todo different precisions
		case(1):
			texformat = GL_RED;
		break;
		case(2):
			texformat = GL_RG;
		break;
		case(3):
			texformat = GL_RGB;
		break;
		case(4):
			texformat = GL_RGBA;
		break;
		default:
			//todo make return a default
		break;
	}
	glGenTextures(1, &tex.id);
	glBindTexture(GL_TEXTURE_2D, tex.id); //todo set filters, etc
	glTexImage2D(GL_TEXTURE_2D, 0, texformat, tex.width, tex.height, 0, texformat, GL_UNSIGNED_BYTE, teximage->pixels); //todo different formats
	SDL_FreeSurface(teximage);
	glBindTexture(GL_TEXTURE_2D, 0);
	return tex;
}
