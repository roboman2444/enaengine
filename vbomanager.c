#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>

#include "globaldefs.h"
#include "vbomanager.h"
#include "hashtables.h"
#include "glstates.h"

#include "glmanager.h"

int vbo_ok = 0;
int vbocount = 0;
int vboArrayFirstOpen = 0;
int vboArrayLastTaken = -1;
int vboArraySize = 0;
vbo_t *vbolist;


hashbucket_t vbohashtable[MAXHASHBUCKETS];

int vbo_init(void){
	memset(vbohashtable, 0, MAXHASHBUCKETS*sizeof(hashbucket_t));
	if(vbolist) free(vbolist);
	vbolist = 0;

	vbo_ok = TRUE;
	return TRUE;
}


vbo_t * findVBOByNameRPOINT(char * name){
	return returnVBOById(findByNameRINT(name, vbohashtable));
}
int findVBOByNameRINT(char * name){
	return findByNameRINT(name, vbohashtable);
}

int deleteVBO(const int id){
	int vboindex = (id & 0xFFFF);
	vbo_t * vbo = &vbolist[vboindex];
	if(vbo->myid != id) return FALSE;
	if(!vbo->name) return FALSE;
	deleteFromHashTable(vbo->name, id, vbohashtable);
	free(vbo->name);
	glDeleteBuffers(2, &vbo->vboid); //deletes both
	glDeleteVertexArrays(1, &vbo->vaoid);

	memset(vbo,0, sizeof(vbo_t));
	if(vboindex < vboArrayFirstOpen) vboArrayFirstOpen = vboindex;
	for(; vboArrayLastTaken > 0 && !vbolist[vboArrayLastTaken].type; vboArrayLastTaken--);
	return TRUE;
}

vbo_t * returnVBOById(const int id){
	int vboindex = (id & 0xFFFF);
	vbo_t * vbo = &vbolist[vboindex];
	if(!vbo->type) return FALSE;
	if(vbo->myid == id) return vbo;
	return FALSE;
}
vbo_t createVBO(char * name, char type){
	vbo_t v = {0};
	glGenVertexArrays(1, &v.vaoid);	if(!v.vaoid) return v;
	states_bindVertexArray(v.vaoid);
	glGenBuffers(1, &v.vboid);	if(!v.vboid) return v;
	glGenBuffers(1, &v.indicesid);	if(!v.indicesid) return v;
	v.name = malloc(strlen(name)+1);
	v.setup =0;
	strcpy(v.name, name);
//	v.type = type; //todo type stuff
	v.type = 2;
	return v;
}

int addVBORINT(vbo_t vbo){
	vbocount++;
	for(; vboArrayFirstOpen < vboArraySize && vbolist[vboArrayFirstOpen].type; vboArrayFirstOpen++);
	if(vboArrayFirstOpen == vboArraySize){	//resize
		vboArraySize++;
		vbolist = realloc(vbolist, vboArraySize * sizeof(vbo_t));
	}
	vbolist[vboArrayFirstOpen] = vbo;
	int returnid = (vbocount << 16) | vboArrayFirstOpen;
	vbolist[vboArrayFirstOpen].myid = returnid;

	addToHashTable(vbolist[vboArrayFirstOpen].name, returnid, vbohashtable);
	if(vboArrayLastTaken < vboArrayFirstOpen) vboArrayLastTaken = vboArrayFirstOpen; //todo redo
	return returnid;
}
vbo_t * addVBORPOINT(vbo_t vbo){
	vbocount++;
	for(; vboArrayFirstOpen < vboArraySize && vbolist[vboArrayFirstOpen].type; vboArrayFirstOpen++);
	if(vboArrayFirstOpen == vboArraySize){	//resize
		vboArraySize++;
		vbolist = realloc(vbolist, vboArraySize * sizeof(vbo_t));
	}
	vbolist[vboArrayFirstOpen] = vbo;
	int returnid = (vbocount << 16) | vboArrayFirstOpen;
	vbolist[vboArrayFirstOpen].myid = returnid;

	addToHashTable(vbolist[vboArrayFirstOpen].name, returnid, vbohashtable);
	if(vboArrayLastTaken < vboArrayFirstOpen) vboArrayLastTaken = vboArrayFirstOpen; //todo redo
	return &vbolist[vboArrayFirstOpen];
}
void pruneVBOList(void){
	if(vboArraySize == vboArrayLastTaken+1) return;
	vboArraySize = vboArrayLastTaken+1;
	vbolist = realloc(vbolist, vboArraySize * sizeof(vbo_t));
}


vbo_t * createAndAddVBORPOINT(char * name, char type){
	return addVBORPOINT(createVBO(name, type));
}
int createAndAddVBORINT(char * name, char type){
	return addVBORINT(createVBO(name, type));
}
int setUpVBOStride(vbo_t * vbo, unsigned char posstride, unsigned char normstride, unsigned char tcstride, unsigned char tangentstride, unsigned char blendistride, unsigned char blendwstride){
	GLuint totalstride = posstride + normstride + tcstride + tangentstride + blendistride + blendwstride;
	if(!totalstride) return FALSE;
	if(!vbo) return FALSE;
	states_bindVertexArray(vbo->vaoid);
	states_bindBuffer(GL_ARRAY_BUFFER, vbo->vboid);
	size_t curstride = 0;
	GLuint totalstridesize = totalstride * sizeof(GLfloat);
	//todo make this more general?
	if(posstride){
		glEnableVertexAttribArray(POSATTRIBLOC);
		glVertexAttribPointer(POSATTRIBLOC, posstride, GL_FLOAT, GL_FALSE, totalstridesize, (void*)curstride);
		curstride += posstride * sizeof(GLfloat);
	}
	if(normstride){
		glEnableVertexAttribArray(NORMATTRIBLOC);
		glVertexAttribPointer(NORMATTRIBLOC, normstride, GL_FLOAT, GL_FALSE, totalstridesize, (void*)curstride);
		curstride += normstride * sizeof(GLfloat);
	}
	if(tcstride){
		glEnableVertexAttribArray(TCATTRIBLOC);
		glVertexAttribPointer(TCATTRIBLOC, tcstride, GL_FLOAT, GL_FALSE, totalstridesize, (void*)curstride);
		curstride += tcstride * sizeof(GLfloat);
	}
	if(tangentstride){
		glEnableVertexAttribArray(TANGENTATTRIBLOC);
		glVertexAttribPointer(TANGENTATTRIBLOC, tangentstride, GL_FLOAT, GL_FALSE, totalstridesize, (void*)curstride);
		curstride += tangentstride * sizeof(GLfloat);
	}
	if(blendistride){
		glEnableVertexAttribArray(BLENDIATTRIBLOC);
		glVertexAttribPointer(BLENDIATTRIBLOC, blendistride, GL_UNSIGNED_BYTE, GL_FALSE, totalstridesize, (void*)curstride);
		curstride += blendistride * sizeof(GLfloat);
	}
	if(blendwstride){
		glEnableVertexAttribArray(BLENDWATTRIBLOC);
		glVertexAttribPointer(BLENDWATTRIBLOC, blendwstride, GL_UNSIGNED_BYTE, GL_TRUE, totalstridesize, (void*)curstride);
		curstride += blendwstride * sizeof(GLfloat);//todo GLFloat?
	}
	vbo->setup = TRUE;
	vbo->posstride = posstride;
	vbo->normstride = normstride;
	vbo->tcstride = tcstride;
	vbo->tangentstride = tangentstride;
	vbo->totalstride = totalstride;

	return totalstride;
}
