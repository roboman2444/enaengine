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
#define RADIXSORTSIZE 12 // currently 2 for shader id, 4 for permutation, 2 for modelid/vboid (if any, ones with 0 probably use the vert data method, have to check), 2 for texture id, 1 for depth, 1 for misc flags (such as alpha blending or not)

typedef void * (* renderqueueCallback_t)(void ** data, unsigned int count);
typedef struct renderlistitem_s {
	void * data;
	unsigned int datasize;
	renderqueueCallback_t setup;
	renderqueueCallback_t draw;
	unsigned char flags; //1 is nonfreeable
	unsigned char sort[RADIXSORTSIZE];
} renderlistitem_t;

int addEntityToRenderbatche(entity_t * ent, renderbatche_t * batch);
int addObjectToRenderbatche(worldobject_t * obj, renderbatche_t * batch);

int cleanupRenderbatche(renderbatche_t * batch);

int readyRenderQueueBuffers(void);

//returns -1 on failure, returns offset into facebuffer (per int, not per face)
int pushDataToVertCache(const unsigned int vertcount, const unsigned int facecount, const unsigned int * face, const float * posdata, const float * normdata, const float * tcdata, const float * tangentdata, const unsigned int * blendidata, const unsigned int * blendwdata);
char flushVertCacheToBuffers(void);


//returns -1 on failure, returns offset into ubo buffer in bytes
int pushDataToUBOCache(const unsigned int size, const void * data);
char flushUBOCacheToBuffers(void);


char createAndAddRenderlistitem(const void * data, const unsigned int datasize, const renderqueueCallback_t setup, const renderqueueCallback_t draw, const unsigned char flags, const unsigned char sort[RADIXSORTSIZE]);
char addRenderlistitem(renderlistitem_t r);

void renderqueueDraw(void);
void renderqueueSetup(void);

//void addLightToLightbatche(light_t * l, lightbatche_t * batch);
//void cleanupLightbatche(lightbatche_t * batch);

#endif
