#ifndef MODELMANAGERHEADER
#define MODELMANAGERHEADER

typedef struct joint_s { //pretty much a copy of IQMv2 joints
	unsigned int name;
	int parent;
	float translate[3], rotate[4], scale[3];
} joint_t;


typedef struct model_s {
	char * name;
	int myid;
	int vbo;
	vec_t bboxp[24];
	vec_t bbox[6];
	float spheresize;
	char type; // maybe not
	GLfloat * interleaveddata;
//	iqmjoint * joints;
	GLuint * tris;
	GLuint numverts;
	GLuint numfaces;

	unsigned int numjoints;
	joint_t *joints;
	float * bbpinverse;
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
int deleteAllModels(void);

model_t * createAndAddModelRPOINT(char * name);
int createAndAddModelRINT(char * name);
//int makeCubeModel(void);

#endif

