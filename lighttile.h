#ifndef LIGHTTILEHEADER
#define LIGHTTILEHEADER
#define PERTILEMAXLIGHTS 8
typedef struct pTileLightUBOStruct_s {
	GLfloat pos[3];
	GLfloat size;
} pTileLightUBOStruct_t;
typedef struct tileUBOStruct_s {
	GLfloat offset[2];
	int lcount;
	pTileLightUBOStruct_t lights[PERTILEMAXLIGHTS];
} tileUBOStruct_t;


typedef struct lighttile_s {
	GLfloat x;
	GLfloat y;
	unsigned int plsize;
	unsigned int plcount;
	tileUBOStruct_t * pllist;

	unsigned int slsize;
	unsigned int slcount;
	tileUBOStruct_t * sllist;
} lighttile_t;

typedef struct lighttilebuffer_s {
	unsigned int count;
	lighttile_t *list;
} lighttilebuffer_t;

int lighttile_ok;

int lighttile_init(void);
unsigned int lighttile_addToRenderQueue(viewport_t *v, renderqueue_t *q, const unsigned int width, const unsigned int height);


#endif
