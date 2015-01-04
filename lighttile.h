#ifndef LIGHTTILEHEADER
#define LIGHTTILEHEADER


typedef struct lighttile_s {
	unsigned int size;
	unsigned int count;
	unsigned int * list; //list of indices
//	light_t ** list;
} lighttile_t;

unsigned int lighttile_count;
lighttile_t * lighttile_list;
int lighttile_ok;

int lighttile_init(void);
void lighttile_tileLights(const viewport_t *v, const unsigned int width, const unsigned int height, const lightlistpoint_t l);
int lighttile_addToRenderQueue(viewport_t *v, renderqueue_t *q, const unsigned int width, const unsigned int height);


#endif
