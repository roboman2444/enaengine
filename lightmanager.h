#ifndef LIGHTMANAGERHEADER
#define LIGHTMANAGERHEADER

typedef struct light_s {
	char * name;
	int myid;
	char type;

	// 0 is NA
	vec3_t pos;
	vec3_t angle;
	//float spotangle
	float scale;
	float difbright;
	float specbright;
	vec3_t color;
	//int needsmatupdate;
//	matrix4x4_t mat;
	int attachmentid;

	int needsupdate;

	vec_t bboxp[24];
	vec_t bbox[6];

} light_t;


/// might not do a quadtree for lights... too slow to dynamically recreate
typedef struct lightleaf_s {
	vec2_t center;
	struct lightleaf_s *children[4];
	int treedepth;
	vec_t bbox[6];
	vec_t bboxp[24];
	int numlights;
	light_t *list; // maybe set to a jumping adjust isntead of dynamic... or linked list
	float size;
} lightleaf_t;

typedef struct lightlistpoint_s {
	//gotta use this stuff after you use it
	light_t **list;
	int count;
} lightlistpoint_t; // yeah yeah i know naming

typedef struct lightlistint_s {
	//gotta use this stuff after you use it
	int *list;
	int count;
} lightlistint_t; // yeah yeah i know naming

light_t *lightlist;
int initLightSystem(void);
int lightcount;
int lightArraySize;
int lightArrayLastTaken;
int lightsOK;

//returns first light it finds with name
light_t * findLightByNameRPOINT(char * nam);
int findLightByNameRINT(char * name);
//returns a list of all entities with name
lightlistpoint_t findLightsByNameRPOINT(char * nam);
lightlistint_t findLightsByNameRINT(char * name);
//returns pointer by light id
light_t * returnLightById(int id);
//creates and adds light to listwith name
light_t * addLightRPOINT(char * name);
int addLightRINT(char * name);
//deletes light, only id is accepted
int deleteLight(int id);

int lightLoop(void);
#endif
