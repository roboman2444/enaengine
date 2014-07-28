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





//start fresh, no idiota this time
#define RADIXSORTSIZE 12 // currently 2 for shader id, 4 for permutation, 2 for modelid (if any, ones with 0 probably use the vert data method), 2 for texture id, 1 for depth, 1 for misc flags (such as alpha blending or not)

typedef void * (* renderqueueCallback_t)(void * data);
typedef struct renderlistitem_s {
	renderqueueCallback_t setup;
	renderqueueCallback_t draw;
	unsigned char sort[RADIXSORTSIZE];
} renderlistitem_t;

int addEntityToRenderbatche(entity_t * ent, renderbatche_t * batch);
int addObjectToRenderbatche(worldobject_t * obj, renderbatche_t * batch);

int cleanupRenderbatche(renderbatche_t * batch);

int readyRenderQueueVBO(void);


//void addLightToLightbatche(light_t * l, lightbatche_t * batch);
//void cleanupLightbatche(lightbatche_t * batch);

#endif
