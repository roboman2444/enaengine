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

/*
typedef struct lightbatch_s {
	unsigned int count;
	int * lightlist;
} lightbatch_t;
*/
/*
typedef struct lightbatches_s {
	lightbatch_t noshadowout;
	lightbatch_t noshadowin;
} lightbatches_t;
*/

/// might not do a quadtree for lights... too slow to dynamically recreate
typedef struct lightleaf_s {
	vec2_t center;
	struct lightleaf_s *children[4];
	int treedepth;
	vec_t bbox[6];
	vec_t bboxp[24];
	int numlights;
	light_t *list; // maybe set to a jumping adjust instead of dynamic... or linked list
	float size;
} lightleaf_t;

typedef struct lightlistpoint_s {
	//gotta use this stuff after you use it
	light_t **list;
	unsigned int count;
} lightlistpoint_t; // yeah yeah i know naming

typedef struct lightlistint_s {
	//gotta use this stuff after you use it
	int *list;
	unsigned int count;
} lightlistint_t; // yeah yeah i know naming



typedef struct lightrenderout_s {
	lightlistpoint_t lout;
	lightlistpoint_t lin;
	lightlistpoint_t lshadowout;
	lightlistpoint_t lshadowin;
} lightrenderout_t;

light_t *lightlist;
int light_init(void);
int lightcount;
int lightArraySize;
int lightArrayLastTaken;
int light_ok;

int maxVisLights;
int maxShadowLights;

//returns first light it finds with name
light_t * findLightByNameRPOINT(const char * nam);
int findLightByNameRINT(const char * name);
//returns a list of all entities with name
lightlistpoint_t findLightsByNameRPOINT(const char * nam);
lightlistint_t findLightsByNameRINT(const char * name);
//returns pointer by light id
light_t * returnLightById(const int id);
//creates and adds light to listwith name
light_t * light_addRPOINT(const char * name);
int light_addRINT(const char * name);
//deletes light, only id is accepted
int deleteLight(const int id);
void pruneLightList(void);


int lightLoop(void);

lightrenderout_t readyLightsForRender(viewport_t *v, const unsigned int max, const unsigned int maxshadowed);
#endif
