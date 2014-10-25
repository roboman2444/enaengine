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

void lighttile_tileLights(const viewport_t *v, const unsigned int width, const unsigned int height, const lightlistpoint_t l);

#endif
