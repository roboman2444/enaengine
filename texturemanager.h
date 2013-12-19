#ifndef TEXTUREMANAGERHEADER
#define TEXTUREMANAGERHEADER

typedef struct texture_s {
	GLuint id;
	char type;
	GLuint width; // could do shorts...
	GLuint height; // could do shorts...
	char components; //todo bitfield
}texture_t;


/* types of textures ... i should do enums for these

0 - unused/error
1 - diffuse
2 - normal
3 - bump/displacement
4 - spec
5 - gloss

10 - envmap(cubemap) unused/error
11 - envmap color
12 - envmap depth (used for shadowmaps of point lights)

20 - screenspace unused/error
21 - screenspace color
22 - screenspace depth
23 - screenspace normal

30 - miscview unused/error
31 - miscview color
32 - miscview depth (used for shadowmaps of spotlights)

*/

typedef struct texturegroup_s{
	int num;
	texture_t * textures; //first should be diffuse, second normal, etc
	char * name;
}texturegroup_t;

texturegroup_t * defaultTexture;//todo

texturegroup_t * defaultTextureGroup;//todo

texturegroup_t * findTextureGroupByName(char * name);

texturegroup_t createTextureGroup(char * name, int num);

int addTextureGroupToList(texturegroup_t texgroup);

int initTextureSystem(void);

texture_t loadTexture(char *filepath, char type);

texturegroup_t createAndLoadTextureGroup(char *name);

int deleteTextureGroup(texturegroup_t texgroup);

int deleteTexture(texture_t texture);

int deleteAllTextureGroups(void);

#endif

