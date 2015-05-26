#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>

#include "globaldefs.h"
#include "ubomanager.h"
#include "hashtables.h"
#include "console.h"
#include "glstates.h"

GLint ubo_maxsize;
GLint ubo_alignment;

int ubo_ok = 0;
int ubo_count = 0;
int ubo_arrayfirstopen = 0;
int ubo_arraylasttaken = -1;
int ubo_arraysize = 0;
ubo_t *ubo_list;


int ubo_init(void){
	if(ubo_list) free(ubo_list);
	ubo_list = 0;

	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &ubo_maxsize);
	console_printf("max uniform size is %i\n", ubo_maxsize);
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &ubo_alignment);
	console_printf("uniform alignment size is %i\n", ubo_alignment);


	ubo_ok = TRUE;
	return TRUE;
}
char ubo_flushData(ubo_t * u){
	unsigned int place = u->place;
	if(!place) return FALSE;
#if UBOPINGPONG > 1
	unsigned int pingplace = u->pingplace = (u->pingplace + 1) % UBOPINGPONG;
	states_bindBuffer(GL_UNIFORM_BUFFER, u->id[pingplace]);
#else
	states_bindBuffer(GL_UNIFORM_BUFFER, u->id);
#endif
	glBufferData(GL_UNIFORM_BUFFER, place, u->data, GL_DYNAMIC_DRAW);
//	u->oldplace = place;
	u->place = 0;
	return TRUE;
}

//returns offset in bytes
int ubo_pushData(ubo_t * u, const unsigned int size, const void * data){
	if(!size || !data) return -1;
	unsigned int mysize = (size + ubo_alignment-1) & ~(ubo_alignment-1);
	unsigned int place = u->place;
	unsigned int newsize = place + mysize;
	if(newsize > u->size){
		u->data = realloc(u->data, newsize);
		u->size = newsize;
	}
	memcpy(u->data + place, data, size);
	u->place = newsize;
	return place;
}
//returns offset in bytes
int ubo_pushDataNoAlign(ubo_t * u, const unsigned int size, const void * data){
	if(!size || !data) return -1;
	unsigned int place = u->place;
	unsigned int newsize = place + size;
	if(newsize > u->size){
		u->data = realloc(u->data, newsize);
		u->size = newsize;
	}
	memcpy(u->data + place, data, size);
	u->place = newsize;
	return place;
}
//returns offset in bytes
unsigned int ubo_alignData(ubo_t * u){
	unsigned int mysize = (u->place + ubo_alignment-1) & ~(ubo_alignment-1);
	if(mysize > u->size){
		u->data = realloc(u->data, mysize);
		u->size = mysize;
	}
	u->place = mysize;
	return mysize;
}


unsigned int ubo_halfData(ubo_t *u){
	unsigned int newsize = u->size/2;
	u->size = newsize;
	u->data = realloc(u->data, newsize);
	return newsize;
}
unsigned int ubo_sizeData(ubo_t *u, const unsigned int newsize){
	u->size = newsize;
	u->data = realloc(u->data, newsize);
	return newsize;
}
unsigned int ubo_pruneData(ubo_t *u){
	unsigned int newsize = u->place;
	u->size = newsize;
	u->data = realloc(u->data, newsize);
	return newsize;
}


int ubo_delete(const int id){
	int index = (id & 0xFFFF);
	ubo_t * ubo = &ubo_list[index];
	if(ubo->myid != id) return FALSE;
#if UBOPINGPONG > 1
	glDeleteBuffers(UBOPINGPONG, ubo->id);
#else
	glDeleteBuffers(1, &ubo->id);
#endif

	memset(ubo, 0, sizeof(ubo_t));
	if(index < ubo_arrayfirstopen) ubo_arrayfirstopen = index;
	for(; ubo_arraylasttaken > 0 && !ubo_list[ubo_arraylasttaken].type; ubo_arraylasttaken--);
	return TRUE;
}

ubo_t * ubo_returnById(int id){
	int index = (id & 0xFFFF);
	ubo_t * ubo = &ubo_list[index];
	if(!ubo->type) return FALSE;
	if(ubo->myid == id) return ubo;
	return FALSE;
}
ubo_t ubo_create(void){
#if UBOPINGPONG > 1
	ubo_t u = {0, 0, 0, 0, 0, 0, {0}};
	glGenBuffers(UBOPINGPONG, u.id);	if(!u.id[0]) return u;
#else
	ubo_t u = {0, 0, 0, 0, 0, 0};
	glGenBuffers(1, &u.id);			if(!u.id) return u;
#endif
	u.type = 1;
	return u;
}

int ubo_addRINT(ubo_t ubo){
	ubo_count++;
	for(; ubo_arrayfirstopen < ubo_arraysize && ubo_list[ubo_arrayfirstopen].type; ubo_arrayfirstopen++);
	if(ubo_arrayfirstopen == ubo_arraysize){	//resize
		ubo_arraysize++;
		ubo_list = realloc(ubo_list, ubo_arraysize * sizeof(ubo_t));
	}
	ubo_list[ubo_arrayfirstopen] = ubo;
	int returnid = (ubo_count << 16) | ubo_arrayfirstopen;
	ubo_list[ubo_arrayfirstopen].myid = returnid;

	if(ubo_arraylasttaken < ubo_arrayfirstopen) ubo_arraylasttaken = ubo_arrayfirstopen; //todo redo
	return returnid;
}
ubo_t * ubo_addRPOINT(ubo_t ubo){
	ubo_count++;
	for(; ubo_arrayfirstopen < ubo_arraysize && ubo_list[ubo_arrayfirstopen].type; ubo_arrayfirstopen++);
	if(ubo_arrayfirstopen == ubo_arraysize){	//resize
		ubo_arraysize++;
		ubo_list = realloc(ubo_list, ubo_arraysize * sizeof(ubo_t));
	}
	ubo_list[ubo_arrayfirstopen] = ubo;
	int returnid = (ubo_count << 16) | ubo_arrayfirstopen;
	ubo_list[ubo_arrayfirstopen].myid = returnid;

	if(ubo_arraylasttaken < ubo_arrayfirstopen) ubo_arraylasttaken = ubo_arrayfirstopen; //todo redo
	return &ubo_list[ubo_arrayfirstopen];
}

ubo_t * ubo_createAndAddRPOINT(void){
	return ubo_addRPOINT(ubo_create());
}
int ubo_createAndAddRINT(void){
	return ubo_addRINT(ubo_create());
}



void ubo_pruneList(void){
	if(ubo_arraysize == ubo_arraylasttaken+1) return;
	ubo_arraysize = ubo_arraylasttaken+1;
	ubo_list = realloc(ubo_list, ubo_arraysize * sizeof(ubo_t));
}
