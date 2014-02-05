#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>
#include <sys/stat.h> //todo filesys


#include "globaldefs.h"
#include "texturemanager.h"

#include "SDL_image.h"
#include "SDL.h"
int texturesOK = 0;
int texturegroupnumber = 0; // first one is error one

#define NUMNAMES 5
#define NUMFILES 5
texturegroup_t **texturegrouplist; //todo have a sperate dynamic and static lists
texturegroup_t * defaultTextureGroup;

int initTextureSystem(void){
	if(texturegrouplist) free(texturegrouplist);
	texturegrouplist = malloc(texturegroupnumber * sizeof(texturegroup_t *));
	if(!texturegrouplist) memset(texturegrouplist, 0, texturegroupnumber * sizeof(texturegroup_t *));
	defaultTextureGroup = addTextureGroupToList(createTextureGroup("default", 0));
	//todo error checking
	texturesOK = TRUE;
	return TRUE;
}

texturegroup_t createTextureGroup(char * name, int num){
	texturegroup_t texgroup;
	texgroup.textures = malloc(num*sizeof(texture_t));
	texgroup.num = num;
	texgroup.name = malloc(strlen(name)+1);
	strcpy(texgroup.name, name);
	return texgroup;
}

texturegroup_t * addTextureGroupToList(texturegroup_t texgroup){
	texturegroup_t * pointtexgroup = malloc(sizeof(texturegroup_t));
	*pointtexgroup = texgroup;
	int current = texturegroupnumber;
	texturegroupnumber++;
	texturegrouplist = realloc(texturegrouplist, texturegroupnumber*sizeof(texturegroup_t *));
	texturegrouplist[current] = pointtexgroup;
	return pointtexgroup;
}
texturegroup_t * findTextureGroupByName(char * name){
	int i;
	for(i = 0; i < texturegroupnumber; i++){
		if(!strcmp(name, texturegrouplist[i]->name)) return texturegrouplist[i];
	}
	return texturegrouplist[0];
}
//todo something to load all textures for group *name
texture_t loadTexture(char * filepath, char type){
	texture_t tex;
	SDL_Surface *teximage;
	//todo better errorchecking
//	if(!(teximage = IMG_Load(filepath))) return FALSE; //todo make return a "defualt"
	teximage = IMG_Load(filepath);
	tex.width = teximage->w;
	tex.height = teximage->h;
	tex.type = type;
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

int deleteTexture(texture_t texture){
	glDeleteTextures(1, &texture.id);
	texture.type = 0;
	texture.id = 0;
	texture.width = 0;
	texture.height = 0;
	return TRUE; // todo return something useful
}

int deleteTextureGroup(texturegroup_t * texgroup){
	//todo method for replacing... like particlemanager.
	int i;
	for(i = 0; i < texgroup->num; i++){
		deleteTexture(texgroup->textures[i]);
	}
	texgroup->num = 0;
	free(texgroup->textures);
	texgroup->textures = 0;
	free(texgroup->name);
	texgroup->name = 0;
	free(texgroup);
	//todo return false if some sort of error
	return i;
}
int deleteAllTextureGroups(void){
	int i;
	for(i = 0; i < texturegroupnumber; i++){
		deleteTextureGroup(texturegrouplist[i]); // if texgroup is unused/deleted, the num will be 0 anyway
	}
	free(texturegrouplist);
	texturegrouplist = 0;
	texturegroupnumber = 0;
	return i; //todo useful returns
}

texturegroup_t createAndLoadTextureGroup(char * name){
	char * filetypes[] = {".png",".tga",".bmp",".jpg",".jpeg"}; //todo filesys
	char * nametypes[] = {"_diffuse","_normal","_bump","_spec","_gloss"}; //todo filesys
	texturegroup_t texgroup;
	//todo clean up texturegroup if it already has shit in it.
	texgroup.num = 0;
	texgroup.name = malloc(strlen(name)+1);
	strcpy(texgroup.name, name);
	//todo filesys
	char * filename = malloc(strlen(name)+15); //max size
	int n, f;
	struct stat s;
	for(n = 0; n < NUMNAMES && nametypes[n]; n++){
		for(f = 0; f < NUMFILES && filetypes[f]; f++){
			//do i need to clear the string?
			sprintf(filename, "%s%s%s", name, nametypes[n], filetypes[f]);
			if(!stat(filename, &s)){ //dont actually need it
				//todo implement a filesystem sort of thing
				texture_t temptex = loadTexture(filename, n+1); // n+1 is the type which each in nametypes corresponds to
				if(!temptex.id) break; //if texture loading fails... TODO debug
				texgroup.num++;
				texgroup.textures = realloc(texgroup.textures, texgroup.num*sizeof(texture_t));
				texgroup.textures[texgroup.num-1] = temptex;
				f = sizeof(filetypes)+1; // dont look for any more of name_type files
			}
		}
	}
	free(filename);
	return texgroup;
}

int bindTextureGroup(texturegroup_t * texturegroup){
	int count, i;
	texture_t * texturespointer = texturegroup->textures;
	if(!texturespointer) return FALSE;
	for(i = 0; i < texturegroup->num; i++){
		switch(texturespointer[i].type){
			//atm only model textures todo
			case 0: continue; break;
			case 1: glActiveTexture(GL_TEXTURE0);break;
			case 2: glActiveTexture(GL_TEXTURE1);break;
			case 3: glActiveTexture(GL_TEXTURE2);break;
			case 4: glActiveTexture(GL_TEXTURE3);break;
			case 5: glActiveTexture(GL_TEXTURE4);break;
			case 10: continue; break;
			default: continue; break;
		}
		count++;
		glBindTexture(GL_TEXTURE_2D, texturespointer[i].id);
	}
	glActiveTexture(GL_TEXTURE0);
	return count;
}
