#ifndef MODELMANAGERHEADER
#define MODELMANAGERHEADER


typedef struct framegroup_s {
	GLuint offset;
	GLuint count;
} framegroup_t;

typedef struct anim_s {
	char * name;
	GLuint totalframes;
	GLuint totalframegroups;
	framegroup_t * frames;
	//GLuint uboid;
} anim_t;

typedef struct model_s {
	char * name;
	int myid;
//	texturegroup_t * texture;
	int vbo;
	vec_t bboxp[24];
	vec_t bbox[6];
	float spheresize;
	char type; // maybe not
//	int animid;
	GLfloat * interleaveddata;
//	GLuint * numfaces;
//	GLuint numlod;
	GLuint * tris;
	GLuint numverts;
	GLuint numfaces;
	GLuint stride;
}model_t;

/* types of models ... i should do enums for these

0 - unused/error
1 - static
2 - nonstatic, no bones
3 - nonstatic with bones // maybe sperate struct

*/

int modelsOK;
int modelnumber;


model_t * modellist;
int modelcount;
int modelArraySize;
int modelArrayLastTaken;

int initModelSystem(void);

model_t * findModelByNameRPOINT(char * name);
int findModelByNameRINT(char * name);

model_t * returnModelById(int id);

model_t * addModelRPOINT(model_t mod);
int addModelRINT(model_t mod);
int deleteModel(int id);

model_t * createAndAddModelRPOINT(char * name);
int createAndAddModelRINT(char * name);

#endif

