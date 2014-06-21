//gloabl includes
#include <GL/glew.h>
#include <GL/gl.h>

//local includes
#include "globaldefs.h"
#include "hashtables.h"
#include "texturemanager.h"
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
	screen->textures = malloc(sizeof(texture_t));
	screen->textures->width = 800;
	screen->textures->width = 600;
	screen->textures->id = 0;
	screen->type = 1;
	screen->count = 1;
//	screen->texturegroupid = 0;
	screen->name = malloc(7);
	sprintf(screen->name, "screen");
	addFramebufferRPOINT(*screen);
	free(screen);
	return TRUE; // todo error check
}
framebufferlistpoint_t findFramebuffersByNameRPOINT(char * name){
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
framebufferlistint_t findFramebuffersByNameRINT(char * name){
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
	//todo
	free(fb->textures);
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

int resizeFramebuffer(framebuffer_t *fb, int width, int height){
	if(!fb) return FALSE;
	if(!fb->type) return FALSE;
	if(fb->type == 1){
		fb->width = width;
		fb->height = height;
		return TRUE;
	}
	if(height == fb->height) return TRUE;
	if(width  == fb->width)  return TRUE;
	if(height < 1) height = 1;
	if(width < 1) width = 1;
	unsigned char rbflags = fb->rbflags;
	GLsizei samples = 0;
	if(rbflags & FRAMEBUFFERRBFLAGSMSCOUNT){
		samples = 1<<(rbflags & FRAMEBUFFERRBFLAGSMSCOUNT);
	}


	glBindFramebuffer(GL_FRAMEBUFFER, fb->id); //do i need this?
	int i;
	if(samples){
		for(i = 0; i < fb->count; i++){
			if(!resizeTextureMultisample(&fb->textures[i], width, height, samples)){
				printf("Framebuffer %s resize failed at texture %i\n", fb->name, i);
				return FALSE;
			}
		}
	} else {
		for(i = 0; i < fb->count; i++){
			if(!resizeTexture(&fb->textures[i], width, height)){
				printf("Framebuffer %s resize failed at texture %i\n", fb->name, i);
				return FALSE;
			}
		}
	}
	if(rbflags & (FRAMEBUFFERRBFLAGSSTENCIL | FRAMEBUFFERRBFLAGSDEPTH)){
		//it has a renderbuffer!
		glBindRenderbuffer(GL_RENDERBUFFER, fb->rb);
		GLenum fmt = GL_DEPTH_COMPONENT;
		//only change if it has a stencil, because it has to be depth otherwise
		if(rbflags & FRAMEBUFFERRBFLAGSSTENCIL){
			if(rbflags & FRAMEBUFFERRBFLAGSDEPTH){
				fmt = GL_DEPTH24_STENCIL8;
			} else {
				fmt = GL_STENCIL_INDEX8;
			}
		}
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, fmt, width, height);
	}
//	glBindFramebuffer(GL_FRAMEBUFFER, 0); // do i need this?
	fb->width = width;
	fb->height = height;
	return TRUE;
}



GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7};
framebuffer_t createFramebuffer (char * name, unsigned char count, unsigned char rbflags, unsigned char * perflags){
	framebuffer_t fb;
	fb.type = 0;
	fb.id = 0;
	fb.width = 1;
	fb.height = 1;
	if(count >8) count = 8;
	fb.count = count;
	glGenFramebuffers(1, &fb.id);
	glBindFramebuffer(GL_FRAMEBUFFER, fb.id);
	glDrawBuffers(count, buffers);
	fb.textures = malloc(count * sizeof(texture_t));
	GLsizei samples = 0;
	if(rbflags & FRAMEBUFFERRBFLAGSMSCOUNT){
		samples = 1<<(rbflags & FRAMEBUFFERRBFLAGSMSCOUNT);
	}
	int i;
	if(samples){
		for(i = 0; i < count; i++){
			fb.textures[i] = createTextureFlagsSizeMultisample(perflags[i], 1, 1, samples);
			glFramebufferTexture2D(GL_FRAMEBUFFER, buffers[i], GL_TEXTURE_2D_MULTISAMPLE, fb.textures[i].id, 0);
		}
	} else {
		for(i = 0; i < count; i++){
			fb.textures[i] = createTextureFlagsSize(perflags[i], 1, 1);
			glFramebufferTexture2D(GL_FRAMEBUFFER, buffers[i], GL_TEXTURE_2D, fb.textures[i].id, 0);
		}
	}

	fb.rb = 0;
	if(rbflags & (FRAMEBUFFERRBFLAGSSTENCIL | FRAMEBUFFERRBFLAGSDEPTH)){
		//it has a renderbuffer!
		glGenRenderbuffers(1, &fb.rb);
		glBindRenderbuffer(GL_RENDERBUFFER, fb.rb);

		GLenum fmt = GL_DEPTH_COMPONENT;
		GLenum attachment = GL_DEPTH_ATTACHMENT;
//		printf("depth ahoy\n\n\n\n\n");
		//only change if it has a stencil, because it has to be depth otherwise
		if(rbflags & FRAMEBUFFERRBFLAGSSTENCIL){
			if(rbflags & FRAMEBUFFERRBFLAGSDEPTH){
				fmt = GL_DEPTH24_STENCIL8;
				attachment = GL_DEPTH_STENCIL_ATTACHMENT;
			} else {
				fmt = GL_STENCIL_INDEX8;
				attachment = GL_STENCIL_ATTACHMENT;
			}
		}
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, fmt, 1, 1);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, fb.rb);
	}
//	glBindFramebuffer(GL_FRAMEBUFFER, 0); // do i need this?
	fb.rbflags = rbflags;
	fb.name = malloc(strlen(name)+1);
	strcpy(fb.name, name);
	fb.type = 2;
	return fb;
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

framebuffer_t * createAndAddFramebufferRPOINT(char * name, unsigned char count, unsigned char rbflags, unsigned char * perflags){
	return addFramebufferRPOINT(createFramebuffer(name, count, rbflags, perflags));
}
int createAndAddFramebufferRINT(char * name, unsigned char count, unsigned char rbflags, unsigned char * perflags){
	return addFramebufferRINT(createFramebuffer(name, count, rbflags, perflags));
}
