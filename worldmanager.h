#ifndef WORLDMANAGERHEADER
#define WORLDMANAGERHEADER

// 2^14
#define WORLDMAXSIZE 16384

//works out to be about smallest quadtree size is 64 meters by 64meters
#define WORLDTREEDEPTH 8

typedef struct worldobject_s{
//	int myid;
	matrix4x4_t mat;
	int modelid;
	int textureid;
	vec_t bboxp[24];
	vec_t bbox[6];
	vec3_t pos;
	float * interleaveddata;
	char status;

	GLuint vertstart; //useful for moving
	GLuint facestart; //useful for moving
} worldobject_t;

typedef struct worldleaf_s{
	vec2_t center;
	struct worldleaf_s *children[4];
	/* [0 1] ^^
	   [2 3] */
	int treedepth;
	vec_t bbox[6];
	vec_t bboxp[24];
	//todo actual list of shit and a vao
	int numObjects;
	worldobject_t * list;
	float size; // ease
} worldleaf_t;

int worldOK;
int objectNumber;

int initWorldSystem(void);

int generateWorldObject(int modelid, int textureid, matrix4x4_t mat);

int addObjectToWorld(worldobject_t * obj);

int addEntityToWorld(int entityid); //ease of use function

#endif
