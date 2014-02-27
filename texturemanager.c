#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>
#include <sys/stat.h> //todo filesys


#include "globaldefs.h"
#include "texturemanager.h"

#include "SDL_image.h"
#include "SDL.h"
#include "hashtables.h"
#include "console.h"
int texturesOK = 0;
int texturegroupcount = 0;
int texturegroupArrayFirstOpen = 0;
int texturegroupArrayLastTaken = 0;
int texturegroupArraySize = 0;

#define NUMNAMES 5
#define NUMFILES 5
char * filetypes[] = {".png",".tga",".bmp",".jpg",".jpeg"}; //todo filesys
char * nametypes[] = {"_diffuse","_normal","_bump","_spec","_gloss"}; //todo filesys

hashbucket_t texturegrouphashtable[MAXHASHBUCKETS];

texturegroup_t *texturegrouplist;

int initTextureSystem(void){
	memset(texturegrouphashtable, 0, MAXHASHBUCKETS * sizeof(hashbucket_t));
	if(texturegrouplist) free(texturegrouplist);
	texturegrouplist = 0;
//	texturegrouplist = malloc(texturegroupnumber * sizeof(texturegroup_t));
//	if(!texturegrouplist) memset(texturegrouplist, 0, texturegroupnumber * sizeof(texturegroup_t));
//	defaultTextureGroup = addTextureGroupToList(createTextureGroup("default", 0));
	//todo error checking
	texturesOK = TRUE;
	return TRUE;
}

texturegroup_t * findTexturegroupByNameRPOINT(char * name){
	return returnTexturegroupById(findByNameRINT(name, texturegrouphashtable));
}
int findTexturegroupByNameRINT(char * name){
	return findByNameRINT(name, texturegrouphashtable);
}
int deleteTexturegroup(int id){
	int texturegroupindex = (id & 0xFFFF);
	texturegroup_t * tex = &texturegrouplist[texturegroupindex];
	if(tex->myid != id) return FALSE;
	if(!tex->name) return FALSE;
	int i = 0;
	if(tex->textures){
		for(i = 0; i < tex->num; i++){
			deleteTexture(tex->textures[i]);
		}
		free(tex->textures);
	}
	free(tex->name);
	memset(tex,0, sizeof(texturegroup_t));
	if(texturegroupindex < texturegroupArrayFirstOpen) texturegroupArrayFirstOpen = texturegroupindex;
	for(; texturegroupArrayLastTaken > 0 && !texturegrouplist[texturegroupArrayLastTaken].type; texturegroupArrayLastTaken--);
	return i;
}
texturegroup_t * returnTexturegroupById(int id){
	int texturegroupindex = (id & 0xFFFF);
	texturegroup_t * tex = &texturegrouplist[texturegroupindex];
	if(!tex->type) return FALSE;
	if(tex->myid == id) return tex;
	return FALSE;
}

texturegroup_t createTextureGroup(char * name, int num){
	texturegroup_t texgroup;
	texgroup.textures = malloc(num*sizeof(texture_t));
	texgroup.num = num;
	texgroup.name = malloc(strlen(name)+1);
	strcpy(texgroup.name, name);
	return texgroup;
}

//todo something to load all textures for group *name
texture_t loadTexture(char * filepath, char type){
	texture_t tex;
	SDL_Surface *teximage;
	//todo better errorchecking
//	if(!(teximage = IMG_Load(filepath))) return FALSE; //todo make return a "defualt"
	teximage = IMG_Load(filepath);
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
			return tex;
		break;
	}
	glGenTextures(1, &tex.id);
	glBindTexture(GL_TEXTURE_2D, tex.id); //todo set filters, etc
	if(!tex.id) return tex;
/*
	int x,y;
	unsigned char * i;
	i = teximage->pixels;
	for(y = 0; y < tex.height; y++){
		for(x = 0; x < tex.width; x++){
			consolePrintf("%i %i %i %i\t", (int)*i, (int)*i+1, (int)*i+2, (int) *i+3);
			i+=4;
		}
		printf("\n");
	}
*/
	glTexImage2D(GL_TEXTURE_2D, 0, texformat, tex.width, tex.height, 0, texformat, GL_UNSIGNED_BYTE, teximage->pixels); //todo different formats
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
//	glGenerateMipmap(GL_TEXTURE_2D);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
/*
	unsigned char * data = malloc(300 * sizeof(GLfloat));
	int x;
	for(x = 0; x < 300; x++){
		data[x] = rand() / (RAND_MAX);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 10, 10, 0, GL_RGB, GL_FLOAT, data); //todo different formats
	free(data);
*/
	tex.width = teximage->w;
	tex.height = teximage->h;
	tex.type = type;
	consolePrintf("loaded texture %s with dimensions %ix%i format %i and type %i\n", filepath, tex.width, tex.height, teximage->format->BytesPerPixel, tex.type);
	SDL_FreeSurface(teximage);
	glBindTexture(GL_TEXTURE_2D, 0);
//	if(glIsTexture(tex.id))consolePrintf("yes, its a texture!\n");


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

int deleteAllTexturegroups(void){
	int i;
	for(i = 0; i < texturegroupnumber; i++){
		deleteTexturegroup(texturegrouplist[i].myid); // if texgroup is unused/deleted, the num will be 0 anyway
	}
	free(texturegrouplist);
	texturegrouplist = 0;
	texturegroupnumber = 0;
	return i; //todo useful returns
}

texturegroup_t createAndLoadTexturegroup(char * name){
	texturegroup_t texgroup;
	memset(&texgroup, 0, sizeof(texturegroup_t));
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
	texgroup.type = 2;
	return texgroup;
}

void unbindTexturegroup(void){

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
}

int bindTexturegroup(texturegroup_t * texturegroup){
	if(!texturegroup) return -1;
	int count = 0, i;
	texture_t * texturespointer = texturegroup->textures;
	if(!texturespointer) return -2;
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
//			consolePrintf("texture %i at pos %i\n", texturespointer[i].id, texturespointer[i].type);
			count++;

			glBindTexture(GL_TEXTURE_2D, texturespointer[i].id);
//			consolePrintf("error:%i\n",glGetError());
//			consolePrintf("texture bound with id %i\n", texturespointer[i].id);
		}
//	glActiveTexture(GL_TEXTURE0);
	return count;
}

int addTexturegroupRINT(texturegroup_t tex){
	texturegroupcount++;
	for(; texturegroupArrayFirstOpen < texturegroupArraySize && texturegrouplist[texturegroupArrayFirstOpen].type; texturegroupArrayFirstOpen++);
	if(texturegroupArrayFirstOpen == texturegroupArraySize){	//resize
		texturegroupArraySize++;
		texturegrouplist = realloc(texturegrouplist, texturegroupArraySize * sizeof(texturegroup_t));
	}
	texturegrouplist[texturegroupArrayFirstOpen] = tex;
	int returnid = (texturegroupcount << 16) | texturegroupArrayFirstOpen;
	texturegrouplist[texturegroupArrayFirstOpen].myid = returnid;

	addToHashTable(texturegrouplist[texturegroupArrayFirstOpen].name, returnid, texturegrouphashtable);
	if(texturegroupArrayLastTaken < texturegroupArrayFirstOpen) texturegroupArrayLastTaken = texturegroupArrayFirstOpen; //todo redo
	return returnid;
}
texturegroup_t * addTexturegroupRPOINT(texturegroup_t tex){
	texturegroupcount++;
	for(; texturegroupArrayFirstOpen < texturegroupArraySize && texturegrouplist[texturegroupArrayFirstOpen].type; texturegroupArrayFirstOpen++);
	if(texturegroupArrayFirstOpen == texturegroupArraySize){	//resize
		texturegroupArraySize++;
		texturegrouplist = realloc(texturegrouplist, texturegroupArraySize * sizeof(texturegroup_t));
	}
	texturegrouplist[texturegroupArrayFirstOpen] = tex;
	int returnid = (texturegroupcount << 16) | texturegroupArrayFirstOpen;
	texturegrouplist[texturegroupArrayFirstOpen].myid = returnid;

	addToHashTable(texturegrouplist[texturegroupArrayFirstOpen].name, returnid, texturegrouphashtable);
	//todo maybe have texturegroup have a hash variable, so i dont have to calculate it again if i want to delete... maybe
	if(texturegroupArrayLastTaken < texturegroupArrayFirstOpen) texturegroupArrayLastTaken = texturegroupArrayFirstOpen;
//	printf("texturegrouparraysize = %i\n", texturegroupArraySize);
//	printf("texturegroupcount = %i\n", texturegroupcount);

	return &texturegrouplist[texturegroupArrayFirstOpen];

}
int createAndAddTexturegroupRINT(char * name){
	int m = findTexturegroupByNameRINT(name);
	if(m) return m;
	return addTexturegroupRINT(createAndLoadTexturegroup(name));
//	return &texturegrouplist[addtexturegroupToList(createAndLoadtexturegroup(name))];
}
texturegroup_t * createAndAddTexturegroupRPOINT(char * name){
	texturegroup_t * m = findTexturegroupByNameRPOINT(name);
	if(m) return m;
	return addTexturegroupRPOINT(createAndLoadTexturegroup(name));
//	return &texturegrouplist[addtexturegroupToList(createAndLoadtexturegroup(name))];
}
