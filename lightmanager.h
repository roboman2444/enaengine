#include "leaflink.h"

#ifndef LIGHTMANAGERHEADER
#define LIGHTMANAGERHEADER

typedef struct globallight_s {
	char * name;
	int myid;
	char type;

	vec3_t angle;
	float brightness;
} globallight_t;

typedef struct light_s {
	char * name;
	int myid;
	char type;

	// 0 is NA
	vec3_t pos;
	//todo offset/rotation from attachments
	vec3_t angle; // point lights dont use this
	//float spotangle
	float scale;
	float difbright;
	float specbright;
	vec3_t color;
	//int needsmatupdate; // redundant because of needsupdate?
	//todo figure out weather i put a viewport here or just a lot of matricies/fov
//	viewport_t *lvp;
	int attachmentid;
	int needsupdate;

	vec_t bboxp[24];
//	vec_t bboxp[27]; 9 for spot octa
	vec_t bbox[6];

	unsigned char treedepth;
	unsigned int leafpos;
	void * leaf;
	float fovx;
	float fovy;
	float near;
	float far; //todo remove this... should be the same as scale
	plane_t frustum[6];
	matrix4x4_t projection;
	matrix4x4_t view;
	matrix4x4_t viewproj;
	matrix4x4_t cam; //essentially same as an entity's matrix
	matrix4x4_t fixproj;
	matrix4x4_t camproj; //used for transferring a unit cube(or unit cylinder) to the shape of the light
	matrix4x4_t samplemat;
	GLuint shadowtexid;
	GLuint shadowsize;

        //dual linked list shits
	leaflinkitem_t ll;
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
/*
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
*/
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


typedef struct sLightUBOStruct_s {
        GLfloat mvp[16];
        GLfloat mv[16]; //needed?
        GLfloat lmv[16];
        GLfloat pos[3]; //padding for struct/other
        GLfloat size; //needed?
} sLightUBOStruct_t;

typedef struct aLightUBOStruct_s {
	GLfloat lmv[16];
	GLfloat pos[3];
	GLfloat size;
} aLightUBOStruct_t;

typedef struct pLightUBOStruct_s {
        GLfloat pos[3];
        GLfloat size;
} pLightUBOStruct_t;

light_t *lightlist;
int light_init(void);
int lightcount;
int lightArraySize;
int lightArrayLastTaken;
int light_ok;


int globallightcount;
int globallightArraySize;
int globallightArrayLastTaken;

int maxVisLights;
int maxShadowLights;

GLuint light_uboid;

//returns first light it finds with name
light_t * findLightByNameRPOINT(const char * name);
int findLightByNameRINT(const char * name);
//returns first globallight it finds with name
globallight_t * findGlobalLightByNameRPOINT(const char * nam);
int findGlobalLightByNameRINT(const char * name);
//returns a list of all entities with name
lightlistpoint_t findLightsByNameRPOINT(const char * nam);
lightlistint_t findLightsByNameRINT(const char * name);
//returns pointer by light id
light_t * returnLightById(const int id);
//returns pointer by light id
globallight_t * returnGlobalLightById(const int id);
//creates and adds light to listwith name
light_t * light_addRPOINT(const char * name);
int light_addRINT(const char * name);
//creates and adds global light to listwith name
globallight_t * globalLight_addRPOINT(const char * name);
int globalLight_addRINT(const char * name);
//deletes light, only id is accepted
int deleteLight(const int id);
void pruneLightList(void);
//deletes global light, only id is accepted
int deleteGlobalLight(const int id);
void pruneGlobalLightList(void);


int lightLoop(void);

lightrenderout_t readyLightsForRender(viewport_t *v, const unsigned int max, const unsigned int maxshadowed);
//todo move this to a seperate file... like lightdeferred.c/h
//int lights_addToRenderQueue(viewport_t *v, renderqueue_t *q, unsigned int numsamples);
#endif
