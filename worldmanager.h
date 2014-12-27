#ifndef WORLDMANAGERHEADER
#define WORLDMANAGERHEADER

// 2^14
#define WORLDMAXSIZE 16384

//works out to be about smallest quadtree size is 64 meters by 64meters
#define WORLDTREEDEPTH 8

#define WORLDTREEOBJECT 1
#define WORLDTREEENTITY 2
#define WORLDTREELIGHT  4
typedef struct worldobject_s{
//	int myid;
	matrix4x4_t mat;
	int modelid;
	int texturegroupid;
	int shaderid;
	int shaderperm;
	vec_t bboxp[24];
	vec_t bbox[6];
	vec3_t pos;
	char status;
	unsigned char treedepth;
	unsigned int leafpos;
	//note, i use this as a flag, if set to 0, it means "empty cell, able to be written over"
	struct worldleaf_s * leaf;

	char flags;
} worldobject_t;
typedef struct worldleaf_s{
	vec2_t center;
	struct worldleaf_s *children[4];
	struct worldleaf_s * parent;
	/* [0 1] ^^
	   [2 3] */
	int treedepth;
	vec_t bbox[6];
	vec_t bboxp[24];
	worldobject_t * list;
	int * entlist;
	int * lightlist;
	int numobjects;
	int numents;
	int numlights;
	int objectarraysize;
	int entityarraysize;
	int lightarraysize;
	int objectarrayfirstopen;
	int entityarrayfirstopen;
	int lightarrayfirstopen;
	int objectarraylasttaken;
	int entityarraylasttaken;
	int lightarraylasttaken;
	float size; // ease

	char includes;
	char myincludes;
} worldleaf_t;

int worldOK;
unsigned int world_numObjects;
unsigned int world_numEnts;
unsigned int world_numLights;
worldleaf_t * worldroot;

int initWorldSystem(void);

int generateWorldObject(const int modelid, const int texturegroupid, const matrix4x4_t mat);

int addObjectToWorld(worldobject_t * obj);

int addEntityToWorldOBJ(const int entityid); //ease of use function

int saveWorld(const char * filename);
int loadWorld(const char * filename);

int deleteObject(worldobject_t *o);
int walkAndDeleteObject(worldleaf_t * l, worldobject_t *o);

worldleaf_t * walkAndFindObject(worldleaf_t * l, worldobject_t * o);
worldleaf_t * findObject(const worldobject_t * o);


int deleteLeaf(worldleaf_t *l);
int deleteWorld(void);

int moveEntInWorld(void * ep);
int addEntityToWorld(void * e);

#endif
