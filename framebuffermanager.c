//gloabl includes
#include <GL/glew.h>
#include <GL/gl.h>

//local includes
#include "globaldefs.h"
#include "hashtables.h"
#include "framebuffermanager.h"

int framebuffersOK = 0;
int framebuffercount = 0;
int framebufferArrayFirstOpen = 0;
int framebufferArrayLastTaken = -1;
int framebufferArraySize = 0;
framebuffer_t *framebufferlist;

hashbucket_t framebufferhashtable[MAXHASHBUCKETS];

extern framebuffer_t * addFramebufferRPOINT(framebuffer_t fb);

int initFramebufferSystem(void){
	memset(framebufferhashtable, 0, MAXHASHBUCKETS*sizeof(hashbucket_t));
	if(framebufferlist) free(framebufferlist);
	framebufferlist = 0;
	framebuffersOK = TRUE;
	framebuffer_t * screen = malloc(sizeof(framebuffer_t));
	screen->width = 800; //todo cvar?
	screen->height = 600;
	screen->id = 0;
	screen->texturegroupid = 0;
	screen->name = malloc(7);
	sprintf(screen->name, "screen");
	addFramebufferRPOINT(*screen);
	free(screen);
	return TRUE; // todo error check
}
framebufferlistpoint_t findFramebufferssByNameRPOINT(char * name){
	framebufferlistpoint_t ret;
	int hash = getHash(name);
	hashbucket_t * hb = &framebufferhashtable[hash];
	if(!hb->name) return ret;
        for(; hb; hb = hb->next){
		if(strcmp(hb->name, name)==0){
//			return returnById(hb->id);
			ret.count++;
			ret.list = realloc(ret.list, ret.count * sizeof(framebuffer_t *));
			ret.list[ret.count-1] = returnFramebufferById(hb->id);
		}
        }
	return ret;
}
framebufferlistint_t findFramebufferssByNameRINT(char * name){
	framebufferlistint_t ret;
	int hash = getHash(name);
	hashbucket_t * hb = &framebufferhashtable[hash];
	if(!hb->name) return ret;
        for(; hb; hb = hb->next){
		if(strcmp(hb->name, name)==0){
//			return returnById(hb->id);
			ret.count++;
			ret.list = realloc(ret.list, ret.count * sizeof(int));
			ret.list[ret.count-1] = hb->id;
		}
        }
	return ret;
}
framebuffer_t *findFramebufferByNameRPOINT(char * name){
	return returnFramebufferById(findByNameRINT(name, framebufferhashtable));
}
int findFramebufferByNameRINT(char * name){
	return findByNameRINT(name, framebufferhashtable);
}
int deleteFramebuffer(int id){
	int framebufferindex = (id & 0xFFFF);
	framebuffer_t * fb = &framebufferlist[framebufferindex];
	if(fb->myid != id) return FALSE;
	if(!fb->name) return FALSE;
	deleteFromHashTable(fb->name, id, framebufferhashtable);
	free(fb->name);

//todo free framebuffer
	memset(fb, 0, sizeof(framebuffer_t));
	if(framebufferindex < framebufferArrayFirstOpen) framebufferArrayFirstOpen = framebufferindex;
	for(; framebufferArrayLastTaken > 0 && !framebufferlist[framebufferArrayLastTaken].type; framebufferArrayLastTaken--);
	return TRUE;
}

framebuffer_t * returnFramebufferById(int id){
	int framebufferindex = (id & 0xFFFF);
	framebuffer_t * framebuffer = &framebufferlist[framebufferindex];
	if(!framebuffer->type) return FALSE;
	if(framebuffer->myid == id) return framebuffer;
	return FALSE;
}

framebuffer_t createFramebuffer (char * name, char type){
	framebuffer_t fb;
	fb.type = 0; //todo make useful
	fb.id = 0;
	//todo
	//todododo




	fb.name = malloc(strlen(name)+1);
	strcpy(fb.name, name);
	fb.type = type;
	return fb;
//todo
}

int addFramebufferRINT(framebuffer_t framebuffer){
	framebuffercount++;
	for(; framebufferArrayFirstOpen < framebufferArraySize && framebufferlist[framebufferArrayFirstOpen].type; framebufferArrayFirstOpen++);
	if(framebufferArrayFirstOpen == framebufferArraySize){	//resize
		framebufferArraySize++;
		framebufferlist = realloc(framebufferlist, framebufferArraySize * sizeof(framebuffer_t));
	}
	framebufferlist[framebufferArrayFirstOpen] = framebuffer;
	int returnid = (framebuffercount << 16) | framebufferArrayFirstOpen;
	framebufferlist[framebufferArrayFirstOpen].myid = returnid;

	addToHashTable(framebufferlist[framebufferArrayFirstOpen].name, returnid, framebufferhashtable);
	if(framebufferArrayLastTaken < framebufferArrayFirstOpen) framebufferArrayLastTaken = framebufferArrayFirstOpen; //todo redo
	return returnid;
}
framebuffer_t * addFramebufferRPOINT(framebuffer_t framebuffer){
	framebuffercount++;
	for(; framebufferArrayFirstOpen < framebufferArraySize && framebufferlist[framebufferArrayFirstOpen].type; framebufferArrayFirstOpen++);
	if(framebufferArrayFirstOpen == framebufferArraySize){	//resize
		framebufferArraySize++;
		framebufferlist = realloc(framebufferlist, framebufferArraySize * sizeof(framebuffer_t));
	}
	framebufferlist[framebufferArrayFirstOpen] = framebuffer;
	int returnid = (framebuffercount << 16) | framebufferArrayFirstOpen;
	framebufferlist[framebufferArrayFirstOpen].myid = returnid;

	addToHashTable(framebufferlist[framebufferArrayFirstOpen].name, returnid, framebufferhashtable);
	if(framebufferArrayLastTaken < framebufferArrayFirstOpen) framebufferArrayLastTaken = framebufferArrayFirstOpen; //todo redo
	return &framebufferlist[framebufferArrayFirstOpen];
}

framebuffer_t * createAndAddFramebufferRPOINT(char * name, char type){
	return addFramebufferRPOINT(createFramebuffer(name, type));
}
int createAndAddFramebufferRINT(char * name, char type){
	return addFramebufferRINT(createFramebuffer(name, type));
}
