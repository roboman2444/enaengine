#ifndef DLLINCLUDEMEHEADER
#define DLLINCLUDEMEHEADER

#include "globaldefs.h"
#include "matrixlib.h"

//cvar
struct cvar_s;
typedef void (* cvarcallback_t)(struct cvar_s * c);

typedef struct cvar_s {
	char type;
	char * name;
	char * helptext;
	char * defaultstring;
	cvarcallback_t onchange;

	int valueint;
	float valuefloat;
	float valuevector[3]; //todo write something for this...
	char * valuestring; //ALWAYS freeable
	unsigned int valuestringlength;
	int myid;



//	unsigned int defaultstringlength;
	char notdefault;

} cvar_t;

//light
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
	unsigned char treedepth;
	unsigned int leafpos;
	void * leaf;

} light_t;

//ent
typedef struct entity_s {
	char * name;
	int myid;
	char type;
	vec3_t pos;
	vec3_t angle;
	vec3_t anglevel;
	vec3_t vel;
	float scale;
	int needsmatupdate;
	matrix4x4_t mat;
	double nextthink;
	void (*spawn)(struct entity_s * e);
	void (*remove)(struct entity_s * e);
	void (*think)(struct entity_s * e);
	void (*touch)(struct entity_s *e, int touchid); //should be entity_t * toucher
	int parentid;
	int attachmentid;

	int shaderid;		//frist priority for grouping
	int shaderperm;
	int texturegroupid;	//second priority
	int modelid;		//third priority

	int needsbboxupdate;

	vec_t bboxp[24];
	vec_t bbox[6];
	char flags;

	unsigned char treedepth;
	unsigned int leafpos;
	void * leaf;

//	vec3_t maxs; //worldspace bounding box
//	vec3_t mins;
} entity_t;

typedef struct entitylistpoint_s {
	//gotta use this stuff after you use it
	entity_t **list;
	unsigned int count;
} entitylistpoint_t; // yeah yeah i know naming

typedef struct entitylistint_s {
	//gotta use this stuff after you use it
	int *list;
	unsigned int count;
} entitylistint_t; // yeah yeah i know naming

#include "gamecodeincludes.h"


#endif
