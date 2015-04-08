#ifndef MODELMANAGERHEADER
#define MODELMANAGERHEADER

typedef struct joint_s { //pretty much a copy of IQMv2 joints
	unsigned int name;
	int parent;
	float translate[3], rotate[4], scale[3];
	char setbbox;
	vec_t bbox[6];
	vec_t bboxp[24]; // aligned to bone
	matrix4x4_t bbpinverse; //todo might make this a pointer in which i can free later
} joint_t;


typedef struct model_s {
	char * name;
	int myid;
	int vbo;
	vec_t bboxp[24];
	vec_t bbox[6];
	float spheresize;
	char type; // maybe not
	GLfloat * interleaveddata; //only needed for the joint bboxp, after that its freed

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

int model_ok;
int modelnumber;


model_t * model_list;
int model_count;
int model_arraySize;
int model_arrayLastTaken;

int model_init(void);

model_t * model_findByNameRPOINT(const char * name);
int model_findByNameRINT(const char * name);

model_t * model_returnById(const int id);

model_t * model_addRPOINT(model_t mod);
int model_addRINT(model_t mod);
int model_delete(const int id);
int model_deleteAll(void);
void model_pruneList(void);


model_t * model_createAndAddRPOINT(const char * name);
int model_createAndAddRINT(const char * name);

//int model_makeAnyModel(const char * name, GLfloat * verts, GLuint * faces, GLuint numfaces, GLuint numverts);

#endif
