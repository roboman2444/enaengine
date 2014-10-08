#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>
#include <sys/stat.h> //todo filesys


#include "globaldefs.h"
#include "texturemanager.h"

//#include "SDL_image.h"
#include "SDL.h"
#include "hashtables.h"
#include "console.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "glstates.h" //for states_bindTexture, states_bindActiveTexture and states_activeTexture

texturegroup_t * textureGroupCurrentBound = 0;

int texturesOK = 0;
int texturegroupcount = 0;
int texturegroupArrayFirstOpen = 0;
int texturegroupArrayLastTaken = -1;
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
int deleteTexturegroup(const int id){
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
	deleteFromHashTable(tex->name, id, texturegrouphashtable);
	free(tex->name);
	memset(tex,0, sizeof(texturegroup_t));
	if(texturegroupindex < texturegroupArrayFirstOpen) texturegroupArrayFirstOpen = texturegroupindex;
	for(; texturegroupArrayLastTaken > 0 && !texturegrouplist[texturegroupArrayLastTaken].type; texturegroupArrayLastTaken--);
	return i;
}
texturegroup_t * returnTexturegroupById(const int id){
	int texturegroupindex = (id & 0xFFFF);
	texturegroup_t * tex = &texturegrouplist[texturegroupindex];
	if(!tex->type) return FALSE;
	if(tex->myid == id) return tex;
	return FALSE;
}

texturegroup_t createTexturegroup(char * name, const int num){
	texturegroup_t texgroup;
	texgroup.textures = malloc(num*sizeof(texture_t));
	texgroup.num = num;
	texgroup.name = malloc(strlen(name)+1);
	strcpy(texgroup.name, name);
	return texgroup;
}

char resizeTexture(texture_t *t, unsigned int width, unsigned int height){
	if(!t->id) return FALSE;
	if(!width || !height) return FALSE;
	if(t->width == width && t->height == height) return FALSE;

	//todo make use of a state manager for texture binds
	states_bindTexture(GL_TEXTURE_2D, t->id);

	char componentflags = t->flags & 3;
	GLint texfmt0 = GL_RGB, texfmt1 = GL_RGB;
	GLenum type = GL_UNSIGNED_BYTE;
	switch(componentflags){
		case 0: texfmt0 = GL_RED; break;
		case 1: texfmt0 = GL_RG; break;
		case 2: texfmt0 = GL_RGB; break;
		case 3: texfmt0 = GL_RGBA; break;
		default: break;
	}
	texfmt1 = texfmt0;

	if(t->flags & TEXTUREFLAGFLOAT){
		type = GL_FLOAT;
		if(t->flags & TEXTUREFLAGFLOATHIGHP){
			switch(componentflags){
				case 0: texfmt1 = GL_R32F; break;
				case 1: texfmt1 = GL_RG32F; break;
				case 2: texfmt1 = GL_RGB32F; break;
				case 3: texfmt1 = GL_RGBA32F; break;
				default: break; // never hit this
			}
		} else {
			switch(componentflags){
				case 0: texfmt1 = GL_R16F; break;
				case 1: texfmt1 = GL_RG16F; break;
				case 2: texfmt1 = GL_RGB16F; break;
				case 3: texfmt1 = GL_RGBA16F; break;
				default: break; // never hit this
			}
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, texfmt1, width, height, 0, texfmt0, type, NULL);
	t->width = width;
	t->height = height;
	return TRUE;
}


texture_t createTextureFlagsSize(char flags, unsigned int width, unsigned int height){
	texture_t t;
	glGenTextures(1, &t.id);
	t.width = 0;
	t.height = 0;
	t.flags = flags;
	t.type = 0;
	resizeTexture(&t, width, height);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	//todo flags for mipmapping and stuff
	return t;

}
char resizeTextureMultisample(texture_t *t, unsigned int width, unsigned int height, unsigned char samples){
	if(!t->id) return FALSE;
	if(!width || !height) return FALSE;
	if(t->width == width && t->height == height) return FALSE;

	//todo make use of a state manager for texture binds
	states_bindTexture(GL_TEXTURE_2D_MULTISAMPLE, t->id);



	char componentflags = t->flags & 3;
	GLint texfmt0 = GL_RGB;
	switch(componentflags){
		case 0: texfmt0 = GL_RED; break;
		case 1: texfmt0 = GL_RG; break;
		case 2: texfmt0 = GL_RGB; break;
		case 3: texfmt0 = GL_RGBA; break;
		default: break;
	}

	if(t->flags & TEXTUREFLAGFLOAT){
		if(t->flags & TEXTUREFLAGFLOATHIGHP){
			switch(componentflags){
				case 0: texfmt0 = GL_R32F; break;
				case 1: texfmt0 = GL_RG32F; break;
				case 2: texfmt0 = GL_RGB32F; break;
				case 3: texfmt0 = GL_RGBA32F; break;
				default: break; // never hit this
			}
		} else {
			switch(componentflags){
				case 0: texfmt0 = GL_R16F; break;
				case 1: texfmt0 = GL_RG16F; break;
				case 2: texfmt0 = GL_RGB16F; break;
				case 3: texfmt0 = GL_RGBA16F; break;
				default: break; // never hit this
			}
		}
	}
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, texfmt0, width, height, FALSE);
	t->width = width;
	t->height = height;
	return TRUE;
}


texture_t createTextureFlagsSizeMultisample(char flags, unsigned int width, unsigned int height, unsigned char samples){
	texture_t t;
	glGenTextures(1, &t.id);
	t.width = 0;
	t.height = 0;
	t.flags = flags;
	t.type = 0;
	resizeTextureMultisample(&t, width, height, samples);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	//todo flags for mipmapping and stuff
	return t;

}

//todo something to load all textures for group *name
texture_t loadTexture(char * filepath, char type){
	texture_t tex;
	int x=0, y=0, n=0;
	unsigned char * imagedata = stbi_load(filepath, &x, &y, &n, 0);
	if(!imagedata) return tex; //todo error reporting


	GLint texformat = GL_RGB;
	switch(n){//todo different precisions
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
	states_bindTexture(GL_TEXTURE_2D, tex.id); //todo set filters, etc
	if(!tex.id) return tex;
/*
	int x,y;
	unsigned char * i;
	i = imagedata;
	for(y = 0; y < tex.height; y++){
		for(x = 0; x < tex.width; x++){
			console_printf("%i %i %i %i\t", (int)*i, (int)*i+1, (int)*i+2, (int) *i+3);
			i+=4;
		}
		printf("\n");
	}
*/
	//todo sdl surfaces miiiight have flipped texture data, look into this and maybe todo flip it like textmanager does
	tex.width = x;
	tex.height = y;

	//find mipmap max level
	unsigned char level;
	for(level = 0; level < 255; level++){
		if(1<<level > tex.width && 1<<level > tex.height) break;
	}

	//todo convert this into non "legacy" mipmapping code
	glTexImage2D(GL_TEXTURE_2D, 0, texformat, tex.width, tex.height, 0, texformat, GL_UNSIGNED_BYTE, imagedata); //todo different formats
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, level);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,16);//Anisotropic Filtering Attempt
	glGenerateMipmap(GL_TEXTURE_2D);

	tex.type = type;
	console_printf("loaded texture %s with dimensions %ix%i format %i and type %i\n", filepath, tex.width, tex.height, n, tex.type);

	stbi_image_free(imagedata);
	imagedata = 0;
//	SDL_FreeSurface(teximage);
//	states_BindTexture(GL_TEXTURE_2D, 0);
//	if(glIsTexture(tex.id))console_printf("yes, its a texture!\n");


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

/*
//SLOW, DONT USE
void unbindTexturegroup(void){

		glActiveTexture(GL_TEXTURE1);
		BindTexture(GL_TEXTURE_2D, 0);
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
*/
int bindTexturegroup(texturegroup_t * texturegroup){
	if(!texturegroup) return -1;
	int total = texturegroup->num;
	if(texturegroup == textureGroupCurrentBound) return total;
	int count = 0, i;
	texture_t * texturespointer = texturegroup->textures;
	if(!texturespointer) return -2;
	for(i = 0; i < total; i++){
//		if(!texturespointer[i].id) continue;
/*
		switch(texturespointer[i].type){
			//atm only model textures todo
			case 0: continue; break;
			case 1: states_activeTexture(GL_TEXTURE0);break;
			case 2: states_activeTexture(GL_TEXTURE1);break;
			case 3: states_activeTexture(GL_TEXTURE2);break;
			case 4: states_activeTexture(GL_TEXTURE3);break;
			case 5: states_activeTexture(GL_TEXTURE4);break;
			case 6: states_activeTexture(GL_TEXTURE5);break;
			case 10: continue; break;
			default: continue; break;
		}
*/
		if(!texturespointer[i].type) continue;
//		states_activeTexture(texturespointer[i].type -1);
		count++;
		states_bindActiveTexture(texturespointer[i].type -1, GL_TEXTURE_2D, texturespointer[i].id);
//		console_printf("error:%i\n",glGetError());
//		console_printf("texture bound with id %i and space %i\n", texturespointer[i].id, texturespointer[i].type-1);
	}
//	states_activeTexture(0);
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
void pruneTexturegroupList(void){
	if(texturegroupArraySize == texturegroupArrayLastTaken+1) return;
	texturegroupArraySize = texturegroupArrayLastTaken+1;
	texturegrouplist = realloc(texturegrouplist, texturegroupArraySize * sizeof(texturegroup_t));
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
