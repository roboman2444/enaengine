#ifndef RENDERQUEUEHEADER
#define RENDERQUEUEHEADER

typedef struct modelbatche_s {
	int modelid;
	int count;
	matrix4x4_t * matlist;
//	matrix4x4_t * cammatlist;
} modelbatche_t;
typedef struct texturebatche_s {
	int count;
	modelbatche_t * modelbatch;
//	struct texturebatch_s * next; //im lazy and would rather do arrays
	int textureid;
} texturebatche_t;
typedef struct shaderbatche_s {
	int count;
	texturebatche_t * texturebatch;
	int shaderid;
	int shaderperm;
} shaderbatche_t;
typedef struct renderbatche_s {
	int count;
	shaderbatche_t * shaderbatch;
	//int something;
}renderbatche_t; // the e stands for entity
typedef struct lightbatche_s {
	int count;
	int * lightlist;
} lightbatche_t;
int addEntityToRenderbatche(entity_t * ent, renderbatche_t * batch);
int addObjectToRenderbatche(worldobject_t * obj, renderbatche_t * batch);

int cleanupRenderbatche(renderbatche_t * batch);


void addLightToLightbatche( int lightid, lightbatche_t * batch);
void cleanupLightbatche(lightbatche_t * batch);

#endif
