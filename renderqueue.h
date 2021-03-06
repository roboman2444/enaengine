#ifndef RENDERQUEUEHEADER
#define RENDERQUEUEHEADER
/*
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
*/
//ubo_t renderqueue_ubo1;
GLuint renderqueueuboid;//need this?
GLuint renderqueueubo2id;//need this

#define RADIXSORTSIZE 10 // 4 for shader, 2 for modelid/vboid (if any, ones with 0 probably use the vert data method, have to check), 2 for texture id, 1 for depth, 1 for misc flags (such as alpha blending or not)

#define MAXINSTANCESIZE 4096 //for databuf

//typedef void (* renderqueueCallback_t)(void ** data, unsigned int count);
struct renderlistitem_s;
typedef void (* renderqueueCallback_t)(struct renderlistitem_s * ilist, unsigned int count);
typedef struct renderlistitem_s {
	void * data;
	unsigned int datasize;
	unsigned int dataoffset;
	renderqueueCallback_t setup;
	renderqueueCallback_t draw;
	unsigned char flags; //1 is freeable, 2 is copyable, 4 is instanceable
	unsigned int counter; //for instancecounts
	unsigned char sort[RADIXSORTSIZE];
} renderlistitem_t;

typedef struct renderqueue_s {
	unsigned int size;
	unsigned int place;
	renderlistitem_t * list;
	unsigned int datasize;
	unsigned int dataplace;
	unsigned char * data;
	//todo maybe have scratch in here for sorting, so multithreading doesnt have issues
} renderqueue_t;


int readyRenderQueueBuffers(void);

//returns -1 on failure, returns offset into facebuffer (per int, not per face)
int pushDataToVertCache(const unsigned int vertcount, const unsigned int faceinsize, const unsigned int * face, const float * posdata, const float * normdata, const float * tcdata, const float * tangentdata, const unsigned int * blendidata, const unsigned int * blendwdata);
char flushVertCacheToBuffers(void);


//returns -1 on failure, returns offset into ubo buffer in bytes

int pushDataToUBOCache(const unsigned int size, const void * data);
char flushUBOCacheToBuffers(void);
int pushDataToUBO2Cache(const unsigned int size, const void * data);
char flushUBO2CacheToBuffers(void);


char createAndAddRenderlistitem(renderqueue_t * queue, const void * data, const unsigned int datasize, const renderqueueCallback_t setup, const renderqueueCallback_t draw, const unsigned char flags, const unsigned char sort[RADIXSORTSIZE]);
char addRenderlistitem(renderqueue_t * queue, renderlistitem_t r);

void renderqueueDraw(renderqueue_t * queue);
void renderqueueSetup(const renderqueue_t * queue);

unsigned int renderqueueHalfQueue(renderqueue_t * queue);
unsigned int renderqueuePruneQueue(renderqueue_t * queue);
unsigned int renderqueueHalfData(renderqueue_t * queue);
unsigned int renderqueuePruneData(renderqueue_t * queue);
unsigned int renderqueuePruneUBO2(void);
unsigned int renderqueueHalfUBO2(void);
void renderqueueHalfVBO(void);

unsigned int renderqueueCleanup(renderqueue_t * queue);

void renderqueueRadixSort(const renderqueue_t * queue);

#endif
