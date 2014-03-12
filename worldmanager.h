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
	float height;
	float negheight;
	vec2_t center;
	struct worldleaf_s *ne;
	struct worldleaf_s *se;
	struct worldleaf_s *sw;
	struct worldleaf_s *nw;
	int treedepth;
	vec_t * bbox[8];
	//todo actual list of shit and a vao
	worldobject_t * list;
	int numObjects;
} worldleaf_t;

int worldOK;
int objectNumber;

int initWorldSystem(void);

int generateWorldObject(int modelid, int textureid, matrix4x4_t mat);

int addObjectToWorld(worldobject_t * obj);

int addEntityToWorld(int entityid); //ease of use function

#endif
