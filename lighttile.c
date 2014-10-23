//global includes
#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>
//local includes
#include "globaldefs.h"
#include "matrixlib.h"
#include "mathlib.h"
#include "viewportmanager.h"
#include "lightmanager.h"
#include "lighttile.h"

unsigned int lighttile_count = 0;
lighttile_t * lighttile_list;

void addLightToTile(light_t *l, lighttile_t *t){
	unsigned int mycount = t->count;
	t->count++;
	if(t->count > t->size){
		t->list = realloc(t->list, t->count * sizeof(light_t *));
		t->size = t->count;
	}
	t->list[mycount] = l;
}

void lighttile_tileLights(const viewport_t *v, const unsigned int width, const unsigned int height, const lightlistpoint_t l){
	if(!l.count) return;
	unsigned int mycount = width * height;
	//resize tilelist if needed
	if(mycount > lighttile_count){
		lighttile_list = realloc(lighttile_list, mycount * sizeof(lighttile_t));
		memset(lighttile_list + (lighttile_count * sizeof(lighttile_t)), 0, (mycount - lighttile_count) * sizeof(lighttile_t));
		lighttile_count = mycount;
	}
	//loop through lights, add to tiles
	unsigned int i;
	for(i = 0; i < l.count; i++){
		light_t *mylight = l.list[i];
		//transform light bboxp to screen, then get scissor rect
		float maxx = -1.0;
		float maxy = -1.0;
		float minx =  1.0;
		float miny =  1.0;
		vec3_t tvec;
		unsigned int j;
		for(j = 0; j < 24; j+=3){
			Matrix4x4_Transform(&v->viewproj, &mylight->bboxp[j], tvec);
			if(tvec[0] > maxx) maxx = tvec[0];
			if(tvec[0] < minx) minx = tvec[0];
			if(tvec[1] > maxy) maxy = tvec[1];
			if(tvec[1] < miny) miny = tvec[1];
		}
		//find bounds
		unsigned int loopmaxx = (int)(((maxx+1.0)/2.0) * width) + 1;
		if(loopmaxx > width) loopmaxx = width;
		unsigned int loopmaxy = (int)(((maxy+1.0)/2.0) * height)+ 1;
		if(loopmaxy > height) loopmaxy = height;
		unsigned int loopminx = (int)(((minx+1.0)/2.0) * width);
		if(loopminx < 0) loopminx = 0;
		unsigned int loopminy = (int)(((miny+1.0)/2.0) * height);
		if(loopminy < 0) loopminy = 0;
		//use bounds and rectangle insert into tiles
		unsigned int lx, ly;
		for(ly = loopminy; ly < loopmaxy; ly++){
			lighttile_t * start = &lighttile_list[ly * width];
			for(lx = loopminx; lx < loopmaxx; lx++){
				addLightToTile(mylight, &start[lx]);
			}
		}
	}
}
