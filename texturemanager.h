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
	char type;
	int num;
	texture_t * textures; //first should be diffuse, second normal, etc
	char * name;
	int myid;
}texturegroup_t;

int texturesOK;
int texturegroupnumber;

texturegroup_t *texturegrouplist;
int texturegroupcount;
int texturegroupArraySize;
int texturegroupArrayLastTaken;

texturegroup_t createTexturegroup(char * name, int num);

texturegroup_t * addTexturegroupRPOINT(texturegroup_t texgroup);
int addTexturegroupRINT(texturegroup_t texgroup);

int initTextureSystem(void);

texturegroup_t * findTexturegroupByNameRPOINT(char * name);
int findTexturegroupByNameRINT(char * name);

int deleteTexturegroup(int id);


texture_t loadTexture(char *filepath, char type);
texturegroup_t createAndLoadTexturegroup(char *name);


texturegroup_t * createAndAddTexturegroupRPOINT(char * name);
int createAndAddTexturegroupRINT(char * name);

texturegroup_t * returnTexturegroupById(int id);

int bindTexturegroup(texturegroup_t * texgroup);

int deleteAllTexturegroups(void);

int deleteTexture(texture_t tex);

void unbindTexturegroup(void);

#endif

