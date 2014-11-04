#ifndef TEXTUREMANAGERHEADER
#define TEXTUREMANAGERHEADER

typedef struct texture_s {
	GLuint id;
	char type;
	GLuint width; // could do shorts...
	GLuint height; // could do shorts...
	unsigned char flags;
}texture_t;

//first two are components
#define TEXTUREFLAGFLOAT 4
#define TEXTUREFLAGFLOATHIGHP 8
//todo flags for mipmapping and stuff

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
	unsigned char num;
	texture_t * textures; //first should be diffuse, second normal, etc
	char * name;
	int myid;
}texturegroup_t;

int texture_currentBound;

int texture_ok;
int texture_groupnumber;

texturegroup_t *texture_grouplist;
int texture_groupcount;
int texture_groupArraySize;
int texture_groupArrayLastTaken;

texturegroup_t createTexturegroup(const char * name, const int num);

texturegroup_t * addTexturegroupRPOINT(texturegroup_t texgroup);
int addTexturegroupRINT(texturegroup_t texgroup);

int texture_init(void);

texturegroup_t * findTexturegroupByNameRPOINT(const char * name);
int findTexturegroupByNameRINT(const char * name);

int deleteTexturegroup(const int id);


texture_t loadTexture(const char *filepath, char type);
texturegroup_t createAndLoadTexturegroup(const char * name);


texturegroup_t * texture_createAndAddGroupRPOINT(const char * name);
int texture_createAndAddGroupRINT(const char * name);

texturegroup_t * returnTexturegroupById(const int id);

int bindTexturegroup(texturegroup_t * texgroup);

int deleteAllTexturegroups(void);

int deleteTexture(texture_t tex);

//void unbindTexturegroup(void);

void pruneTexturegroupList(void);



texture_t createTextureFlagsSize(char flags, unsigned int width, unsigned int height);
char resizeTexture(texture_t *t, unsigned int width, unsigned int height);

texture_t createTextureFlagsSizeMultisample(char flags, unsigned int width, unsigned int height, unsigned char samples);
char resizeTextureMultisample(texture_t *t, unsigned int width, unsigned int height, unsigned char samples);

#endif

