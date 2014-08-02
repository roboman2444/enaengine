#include <GL/glew.h>
#include <GL/gl.h>

#include "globaldefs.h"
#include "matrixlib.h"
#include "entitymanager.h"
#include "worldmanager.h"
//#include "lightmanager.h"
#include "vbomanager.h"
#include "renderqueue.h"
#include "glmanager.h"

//VERTEX BUFFER STUFF

vboseperate_t renderqueuevbo;

unsigned int vertdatasize = 0; // number of verts
unsigned int vertdataplace = 0;// this one is in number of verts, all others are in number of floats/ints
unsigned int vertposdataplace = 0;
unsigned int vertposdatasize = 0;
GLfloat * vertposdata = 0;

unsigned int vertnormdataplace = 0;
unsigned int vertnormdatasize = 0;
GLfloat * vertnormdata = 0;

unsigned int verttcdataplace = 0;
unsigned int verttcdatasize = 0;
GLfloat * verttcdata = 0;

unsigned int verttangentdataplace = 0;
unsigned int verttangentdatasize = 0;
GLfloat * verttangentdata = 0;

unsigned int vertblendidataplace = 0;
unsigned int vertblendidatasize = 0;
GLuint * vertblendidata = 0;

unsigned int vertblendwdataplace = 0;
unsigned int vertblendwdatasize = 0;
GLuint * vertblendwdata = 0;

unsigned int facedatasize = 0;
unsigned int facedataplace = 0;
GLuint * facedata = 0;


//UNIFORM BUFFER STUFF

unsigned int ubodatasize = 0; //in bytes
unsigned int ubodataplace = 0; // in bytes
GLubyte * ubodata = 0;
GLuint uboid;

// RENDERQUEUE STUFF
unsigned int renderqueuesize = 0;
unsigned int renderqueueplace = 0;
renderlistitem_t * renderqueue = 0;
unsigned int renderscratchsize = 0;
renderlistitem_t * renderscratch = 0;

void renderqueueDraw(void){
	//todo instancing support
	unsigned int i;
	for(i = 0; i < renderqueueplace; i++){
		renderqueue[i].draw(&renderqueue->data, 1);
	}
}
void renderqueueSetup(void){
	//todo instancing support?
	unsigned int i;
	for(i = 0; i < renderqueueplace; i++){
		renderqueue[i].setup(&renderqueue->data, 1);
	}
	flushVertCacheToBuffers();
	flushUBOCacheToBuffers();
}





//modified ioquake3 code
static inline void renderqueue_radix(int byte, int size, renderlistitem_t *source, renderlistitem_t *dest){
	int           count[ 256 ] = {0};
	int           index[ 256 ];
	int           i;
	unsigned char *sortKey = NULL;
	unsigned char *end = NULL;

	sortKey = ( (unsigned char *)&source[0].sort ) + byte;
	end = sortKey + ( size * sizeof( renderlistitem_t ) );
	for( ; sortKey < end; sortKey += sizeof( renderlistitem_t ) )
		++count[*sortKey];

	index[ 0 ] = 0;

	for( i = 1; i < 256; ++i )
		index[i] = index[i - 1] + count[i - 1];

	sortKey = ( (unsigned char *)&source[ 0 ].sort ) + byte;
	for( i = 0; i < size; ++i, sortKey += sizeof( renderlistitem_t ) )
		dest[ index[*sortKey]++ ] = source[i];
}

//modified ioquake3 code
void renderqueueRadixSort(void){
	if(renderscratchsize != renderqueuesize){
		renderscratch = realloc(renderscratch, renderqueuesize * sizeof(renderlistitem_t));
		renderscratchsize = renderqueuesize;
	}
	//todo use some preprocesor magics here
	renderqueue_radix(0, renderqueueplace, renderqueue, renderscratch);
	renderqueue_radix(1, renderqueueplace, renderscratch, renderqueue);
	renderqueue_radix(2, renderqueueplace, renderqueue, renderscratch);
	renderqueue_radix(3, renderqueueplace, renderscratch, renderqueue);
	renderqueue_radix(4, renderqueueplace, renderqueue, renderscratch);
	renderqueue_radix(5, renderqueueplace, renderscratch, renderqueue);
	renderqueue_radix(6, renderqueueplace, renderqueue, renderscratch);
	renderqueue_radix(7, renderqueueplace, renderscratch, renderqueue);
	renderqueue_radix(8, renderqueueplace, renderqueue, renderscratch);
	renderqueue_radix(9, renderqueueplace, renderscratch, renderqueue);
	renderqueue_radix(10,renderqueueplace, renderqueue, renderscratch);
	renderqueue_radix(11,renderqueueplace, renderscratch, renderqueue);
	//todo SHOULD BE RADIXSORTSIZE-1
}





char addRenderlistitem(const renderlistitem_t r){
	if((r.setup || r.draw)) return FALSE;
	//check if it needs a resize
	unsigned int renderqueuenewsize = renderqueueplace + 1 + 1; //todo need 1?
	if(renderqueuenewsize > renderqueuesize){
		renderqueue = realloc(renderqueue, renderqueuenewsize * sizeof(renderlistitem_t));
		renderqueuesize = renderqueuenewsize;
	}
	memcpy(&renderqueue[renderqueueplace], &r, sizeof(renderlistitem_t));
	renderqueueplace ++;
	return TRUE;
}
char createAndAddRenderlistitem(const void * data, const unsigned int datasize, const renderqueueCallback_t setup, const renderqueueCallback_t draw, const unsigned char flags, const unsigned char sort[RADIXSORTSIZE]){
	renderlistitem_t r;
	r.data = (void*)data;
	r.datasize = datasize;
	r.setup = setup;
	r.draw = draw;
	r.flags = flags;
	memcpy(r.sort, sort, RADIXSORTSIZE);
	return addRenderlistitem(r);
}

char flushUBOCacheToBuffers(void){
	if(!ubodataplace) return FALSE;
	glBindBuffer(GL_ARRAY_BUFFER, uboid);
	glBufferData(GL_ARRAY_BUFFER, ubodataplace, ubodata, GL_DYNAMIC_DRAW);
	ubodataplace = 0;
	return TRUE;
}

//returns the offset, in bytes
int pushDataToUBOCache(const unsigned int size, const void * data){
	if(!size || !data) return -1;
	//check if it needs a resize
	unsigned int ubodatanewsize = ubodataplace + size +1; //todo need 1?
	if(ubodatanewsize > ubodatasize){
		ubodata = realloc(ubodata, ubodatanewsize);
		ubodatasize = ubodatanewsize;
	}
	memcpy(ubodata + ubodataplace, data, size);
	unsigned int ubodataoldplace = ubodataplace;
	ubodataplace += size;
	return ubodataoldplace;
}

char flushVertCacheToBuffers(void){
	if(!facedataplace) return FALSE;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderqueuevbo.indicesid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, facedatasize * 3 * sizeof(GLuint), facedata, GL_DYNAMIC_DRAW);

	//check if its actually used
	if(vertposdataplace){
		//check if needs resize
		unsigned int newvertposdatasize = (vertdatasize * 3);
		if(newvertposdatasize > vertposdatasize){
			vertposdata = realloc(vertposdata, newvertposdatasize * sizeof(GLfloat));
			vertposdatasize = newvertposdatasize;
		}
		//check if i gotta fill up with 0s
		unsigned int vertdataplacemul = vertdataplace*3;
		if(vertdataplacemul > vertposdataplace) memset(&vertposdata[vertposdataplace], 0, (newvertposdatasize - vertposdataplace) * sizeof(GLfloat));
		//upload
		glBindBuffer(GL_ARRAY_BUFFER, renderqueuevbo.vboposid);
		glBufferData(GL_ARRAY_BUFFER, newvertposdatasize * sizeof(GLfloat), vertposdata, GL_DYNAMIC_DRAW);
	}
	if(vertnormdataplace){
		//check if needs resize
		unsigned int newvertnormdatasize = (vertdatasize * 3);
		if(newvertnormdatasize > vertnormdatasize){
			vertnormdata = realloc(vertnormdata, newvertnormdatasize * sizeof(GLfloat));
			vertnormdatasize = newvertnormdatasize;
		}
		//check if i gotta fill up with 0s
		unsigned int vertdataplacemul = vertdataplace*3;
		if(vertdataplacemul > vertnormdataplace) memset(&vertnormdata[vertnormdataplace], 0, (newvertnormdatasize - vertnormdataplace) * sizeof(GLfloat));
		//upload
		glBindBuffer(GL_ARRAY_BUFFER, renderqueuevbo.vbonormid);
		glBufferData(GL_ARRAY_BUFFER, newvertnormdatasize * sizeof(GLfloat), vertnormdata, GL_DYNAMIC_DRAW);
	}
	if(verttcdataplace){
		//check if needs resize
		unsigned int newverttcdatasize = (vertdatasize * 2);
		if(newverttcdatasize > verttcdatasize){
			verttcdata = realloc(verttcdata, newverttcdatasize * sizeof(GLfloat));
			verttcdatasize = newverttcdatasize;
		}
		//check if i gotta fill up with 0s
		unsigned int vertdataplacemul = vertdataplace*2;
		if(vertdataplacemul > verttcdataplace) memset(&verttcdata[verttcdataplace], 0, (newverttcdatasize - verttcdataplace) * sizeof(GLfloat));
		//upload
		glBindBuffer(GL_ARRAY_BUFFER, renderqueuevbo.vbotcid);
		glBufferData(GL_ARRAY_BUFFER, newverttcdatasize * sizeof(GLfloat), verttcdata, GL_DYNAMIC_DRAW);
	}
	if(verttangentdataplace){
		//check if needs resize
		unsigned int newverttangentdatasize = (vertdatasize * 3);
		if(newverttangentdatasize > verttangentdatasize){
			verttangentdata = realloc(verttangentdata, newverttangentdatasize * sizeof(GLfloat));
			verttangentdatasize = newverttangentdatasize;
		}
		//check if i gotta fill up with 0s
		unsigned int vertdataplacemul = vertdataplace*3;
		if(vertdataplacemul > verttangentdataplace) memset(&verttangentdata[verttangentdataplace], 0, (newverttangentdatasize - verttangentdataplace) * sizeof(GLfloat));
		//upload
		glBindBuffer(GL_ARRAY_BUFFER, renderqueuevbo.vbotangentid);
		glBufferData(GL_ARRAY_BUFFER, newverttangentdatasize * sizeof(GLfloat), verttangentdata, GL_DYNAMIC_DRAW);
	}
	if(vertblendidataplace){
		//check if needs resize
		unsigned int newvertblendidatasize = (vertdatasize);
		if(newvertblendidatasize > vertblendidatasize){
			vertblendidata = realloc(vertblendidata, newvertblendidatasize * sizeof(GLuint));
			vertblendidatasize = newvertblendidatasize;
		}
		//check if i gotta fill up with 0s
		unsigned int vertdataplacemul = vertdataplace;
		if(vertdataplacemul > vertblendidataplace) memset(&vertblendidata[vertblendidataplace], 0, (newvertblendidatasize - vertblendidataplace) * sizeof(GLuint));
		//upload
		glBindBuffer(GL_ARRAY_BUFFER, renderqueuevbo.vboblendiid);
		glBufferData(GL_ARRAY_BUFFER, newvertblendidatasize * sizeof(GLuint), vertblendidata, GL_DYNAMIC_DRAW);
	}
	if(vertblendwdataplace){
		//check if needs resize
		unsigned int newvertblendwdatasize = (vertdatasize);
		if(newvertblendwdatasize > vertblendwdatasize){
			vertblendwdata = realloc(vertblendwdata, newvertblendwdatasize * sizeof(GLuint));
			vertblendwdatasize = newvertblendwdatasize;
		}
		//check if i gotta fill up with 0s
		unsigned int vertdataplacemul = vertdataplace;
		if(vertdataplacemul > vertblendwdataplace) memset(&vertblendwdata[vertblendwdataplace], 0, (newvertblendwdatasize - vertblendwdataplace) * sizeof(GLuint));
		//upload
		glBindBuffer(GL_ARRAY_BUFFER, renderqueuevbo.vboblendwid);
		glBufferData(GL_ARRAY_BUFFER, newvertblendwdatasize * sizeof(GLuint), vertblendwdata, GL_DYNAMIC_DRAW);
	}
	//reset places
	facedataplace = 0;
	vertdataplace = 0;
	vertposdataplace = 0;
	vertnormdataplace = 0;
	verttcdataplace = 0;
	verttangentdataplace = 0;
	vertblendidataplace = 0;
	vertblendwdataplace = 0;


	return TRUE;
}


//returns the offset into the indices array, in number of floats
int pushDataToVertCache(const unsigned int vertcount, const unsigned int facecount, const unsigned int * face, const float * posdata, const float * normdata, const float * tcdata, const float * tangentdata, const unsigned int * blendidata, const unsigned int *blendwdata){
	if(!vertcount || !facecount || !face) return -1;

	//push vert stuff
	unsigned int faceinsize = facecount*3;
	//todo do i need this +1? (change all others if not as well)
	unsigned int facedatanewsize = facedataplace + faceinsize + 1;
	if(facedatanewsize > facedatasize){
		facedata = realloc(facedata, facedatanewsize * sizeof(GLuint));
		facedatasize = facedatanewsize;
	}
	unsigned int i;
	for(i = 0; i < faceinsize; i++){
		facedata[i+facedataplace] = face[i] + vertdataplace;
	}
	unsigned int oldfacepos = facedataplace;
	facedataplace += faceinsize;
	//push pos stuff
	//todo copy and paste this for every input
	if(posdata){
		//check if needs resize
		unsigned int newvertposdatasize = (vertdataplace + vertcount + 1) * 3;
		if(newvertposdatasize > vertposdatasize){
			vertposdata = realloc(vertposdata, newvertposdatasize * sizeof(GLfloat));
			vertposdatasize = newvertposdatasize;
		}
		//check if i gotta fill up with 0s
		unsigned int vertdataplacemul = vertdataplace*3;
		if(vertdataplacemul > vertposdataplace) memset(&vertposdata[vertposdataplace], 0, (vertdataplacemul - vertposdataplace) * sizeof(GLfloat));
		unsigned int vertcountmul = vertcount*3;
		//data should be at vertdataplacemul now, commence copy
		memcpy(&vertposdata[vertdataplacemul], posdata, vertcountmul * sizeof(GLfloat));
		vertposdataplace = vertdataplacemul + vertcountmul;
	}
	if(normdata){
		//check if needs resize
		unsigned int newvertnormdatasize = (vertdataplace + vertcount + 1) * 3;
		if(newvertnormdatasize > vertnormdatasize){
			vertnormdata = realloc(vertnormdata, newvertnormdatasize * sizeof(GLfloat));
			vertnormdatasize = newvertnormdatasize;
		}

		//check if i gotta fill up with 0s
		unsigned int vertdataplacemul = vertdataplace*3;
		if(vertdataplacemul > vertnormdataplace) memset(&vertnormdata[vertnormdataplace], 0, (vertdataplacemul - vertnormdataplace) * sizeof(GLfloat));
		unsigned int vertcountmul = vertcount*3;
		//data should be at vertdataplacemul now, commence copy
		memcpy(&vertnormdata[vertdataplacemul], normdata, vertcountmul * sizeof(GLfloat));
		vertnormdataplace = vertdataplacemul + vertcountmul;
	}
	if(tcdata){
		//check if needs resize
		unsigned int newverttcdatasize = (vertdataplace + vertcount + 1) * 2;
		if(newverttcdatasize > verttcdatasize){
			verttcdata = realloc(verttcdata, newverttcdatasize * sizeof(GLfloat));
			verttcdatasize = newverttcdatasize;
		}
		//check if i gotta fill up with 0s
		unsigned int vertdataplacemul = vertdataplace*2;
		if(vertdataplacemul > verttcdataplace) memset(&verttcdata[verttcdataplace], 0, (vertdataplacemul - verttcdataplace) * sizeof(GLfloat));
		unsigned int vertcountmul = vertcount*2;
		//data should be at vertdataplacemul now, commence copy
		memcpy(&verttcdata[vertdataplacemul], tcdata, vertcountmul * sizeof(GLfloat));
		verttcdataplace = vertdataplacemul + vertcountmul;
	}
	if(tangentdata){
		//check if needs resize
		unsigned int newverttangentdatasize = (vertdataplace + vertcount + 1) * 3;
		if(newverttangentdatasize > verttangentdatasize){
			verttangentdata = realloc(verttangentdata, newverttangentdatasize * sizeof(GLfloat));
			verttangentdatasize = newverttangentdatasize;
		}
		//check if i gotta fill up with 0s
		unsigned int vertdataplacemul = vertdataplace*3;
		if(vertdataplacemul > verttangentdataplace) memset(&verttangentdata[verttangentdataplace], 0, (vertdataplacemul - verttangentdataplace) * sizeof(GLfloat));
		unsigned int vertcountmul = vertcount*3;
		//data should be at vertdataplacemul now, commence copy
		memcpy(&verttangentdata[vertdataplacemul], tangentdata, vertcountmul * sizeof(GLfloat));
		verttangentdataplace = vertdataplacemul + vertcountmul;
	}
	if(blendidata){
		//check if needs resize
		unsigned int newvertblendidatasize = (vertdataplace + vertcount + 1);
		if(newvertblendidatasize > vertblendidatasize){
			vertblendidata = realloc(vertblendidata, newvertblendidatasize * sizeof(GLfloat));
			vertblendidatasize = newvertblendidatasize;
		}
		//check if i gotta fill up with 0s
		unsigned int vertdataplacemul = vertdataplace;
		if(vertdataplacemul > vertblendidataplace) memset(&vertblendidata[vertblendidataplace], 0, (vertdataplacemul - vertblendidataplace) * sizeof(GLuint));
		unsigned int vertcountmul = vertcount;
		//data should be at vertdataplacemul now, commence copy
		memcpy(&vertblendidata[vertdataplacemul], blendidata, vertcountmul * sizeof(GLuint));
		vertblendidataplace = vertdataplacemul + vertcountmul;
	}
	if(blendwdata){
		//check if needs resize
		unsigned int newvertblendwdatasize = (vertdataplace + vertcount + 1);
		if(newvertblendwdatasize > vertblendwdatasize){
			vertblendwdata = realloc(vertblendwdata, newvertblendwdatasize * sizeof(GLfloat));
			vertblendwdatasize = newvertblendwdatasize;
		}
		//check if i gotta fill up with 0s
		unsigned int vertdataplacemul = vertdataplace;
		if(vertdataplacemul > vertblendwdataplace) memset(&vertblendwdata[vertblendwdataplace], 0, (vertdataplacemul - vertblendwdataplace) * sizeof(GLuint));
		unsigned int vertcountmul = vertcount;
		//data should be at vertdataplacemul now, commence copy
		memcpy(&vertblendwdata[vertdataplacemul], blendwdata, vertcountmul * sizeof(GLuint));
		vertblendwdataplace = vertdataplacemul + vertcountmul;
	}


	vertdataplace += vertcount;
	return oldfacepos;
}

int readyRenderQueueBuffers(void){
//	vbo_t * vbo =  createAndAddVBORPOINT("renderqueue", 1);
//	renderqueuevboid = vbo->myid;
	renderqueuevbo.type = 0;
	glGenVertexArrays(1, &renderqueuevbo.vaoid); if(!renderqueuevbo.vaoid) return FALSE;
	glBindVertexArray(renderqueuevbo.vaoid);
	glGenBuffers(7, &renderqueuevbo.vboposid);
	char name[] = "renderqueue";
	renderqueuevbo.name = malloc(strlen(name)+1);
	renderqueuevbo.setup = 0;
	strcpy(renderqueuevbo.name, name);
	//todo add more checks

	glBindBuffer(GL_ARRAY_BUFFER, renderqueuevbo.vboposid);
		glEnableVertexAttribArray(POSATTRIBLOC);
		glVertexAttribPointer(POSATTRIBLOC, 3, GL_FLOAT, GL_FALSE, 3, 0);
	glBindBuffer(GL_ARRAY_BUFFER, renderqueuevbo.vbonormid);
		glEnableVertexAttribArray(NORMATTRIBLOC);
		glVertexAttribPointer(NORMATTRIBLOC, 3, GL_FLOAT, GL_FALSE, 3, 0);
	glBindBuffer(GL_ARRAY_BUFFER, renderqueuevbo.vbotcid);
		glEnableVertexAttribArray(TCATTRIBLOC);
		glVertexAttribPointer(TCATTRIBLOC, 2, GL_FLOAT, GL_FALSE, 2, 0);
	glBindBuffer(GL_ARRAY_BUFFER, renderqueuevbo.vbotangentid);
		glEnableVertexAttribArray(TANGENTATTRIBLOC);
		glVertexAttribPointer(TANGENTATTRIBLOC, 3, GL_FLOAT, GL_FALSE, 3, 0);
		//todo are these really 1?
	glBindBuffer(GL_ARRAY_BUFFER, renderqueuevbo.vboblendiid);
		glEnableVertexAttribArray(BLENDIATTRIBLOC);
		glVertexAttribPointer(BLENDIATTRIBLOC, 1, GL_UNSIGNED_BYTE, GL_FALSE, 1, 0);
	glBindBuffer(GL_ARRAY_BUFFER, renderqueuevbo.vboblendwid);
		glEnableVertexAttribArray(BLENDWATTRIBLOC);
		glVertexAttribPointer(BLENDWATTRIBLOC, 1, GL_UNSIGNED_BYTE, GL_TRUE, 1, 0);

	glGenBuffers(1, &uboid);

	return TRUE;
}








//start of old


int addEntityToModelbatche(entity_t * ent, modelbatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	if(!batch->matlist) count = 0;
	//doesnt have texture in it
	batch->count++;
	batch->matlist = realloc(batch->matlist, batch->count * sizeof(matrix4x4_t));
	batch->matlist[count] = ent->mat;
//	batch->matlist = realloc(batch->cammatlist, batch->count * sizeof(matrix4x4_t));
//	Matrix4x4_Concat(&cammatlist[count], viewport mat, ent->mat);
//	batch->cammatlist[count] = ent->mat;
	return count+1;
}
int addEntityToTexturebatche(entity_t * ent, texturebatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	int modelid = ent->modelid;
//	modelbatche_t * b = batch->modelbatch;
	if(!batch->modelbatch) count = 0;
//	if(!b) count = 0;
	int i;
	for(i = 0; i < count; i++){
		if(batch->modelbatch[i].modelid == modelid){
//		if(b[i].modelid == modelid){
			return addEntityToModelbatche(ent, &batch->modelbatch[i]);
	//		return addEntityToModelbatche(ent, &b[i]);
		}
	}
	//doesnt have texture in it
	batch->count++;
	batch->modelbatch = realloc(batch->modelbatch, batch->count * sizeof(modelbatche_t));
//	b = batch->modelbatch = realloc(b, batch->count * sizeof(modelbatche_t));
	batch->modelbatch[count].count = 0;
	batch->modelbatch[count].modelid = modelid;
	batch->modelbatch[count].matlist = 0;
//	b->count = 0;
//	b->modelid = modelid;
//	b->matlist = 0;

	return addEntityToModelbatche(ent, &batch->modelbatch[count]);
//	return addEntityToModelbatche(ent, &b[count]);
}
int addEntityToShaderbatche(entity_t * ent, shaderbatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	int textureid = ent->texturegroupid;
//	texturebatche_t * b = batch->texturebatch;
	if(!batch->texturebatch) count = 0;
//	if(!b) count = 0;
	int i;
	for(i = 0; i < count; i++){
		if(batch->texturebatch[i].textureid == textureid){
//		if(b[i].textureid == textureid){
			return addEntityToTexturebatche(ent, &batch->texturebatch[i]);
//			return addEntityToTexturebatche(ent, &b[i]);
		}
	}
	//doesnt have shader in it
	batch->count++;
	batch->texturebatch = realloc(batch->texturebatch, batch->count * sizeof(texturebatche_t));
//	batch->texturebatch = realloc(b, batch->count * sizeof(texturebatche_t));
	batch->texturebatch[count].count = 0;
	batch->texturebatch[count].textureid = textureid;
	batch->texturebatch[count].modelbatch = 0;

	return addEntityToTexturebatche(ent, &batch->texturebatch[count]);
//	return addEntityToTexturebatche(ent, &b[count]);
}

int addEntityToRenderbatche(entity_t * ent, renderbatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	int shaderid = ent->shaderid;
	int shaderperm = ent->shaderperm;
//	shaderbatche_t *b = batch->shaderbatch;
	if(!batch->shaderbatch) count = 0;
//	if(!b) count = 0;
	int i;
	for(i = 0; i < count; i++){
		if(batch->shaderbatch[i].shaderid == shaderid && batch->shaderbatch[i].shaderperm == shaderperm){
//		if(b[i].shaderid == shaderid && b[i].shaderperm == shaderperm){
			return addEntityToShaderbatche(ent, &batch->shaderbatch[i]);
//			return addEntityToShaderbatche(ent, &b[i]);
		}
	}
	//doesnt have shader in it
	batch->count++;
	batch->shaderbatch = realloc(batch->shaderbatch, batch->count * sizeof(shaderbatche_t));
	batch->shaderbatch[count].count = 0;
	batch->shaderbatch[count].shaderid = shaderid;
	batch->shaderbatch[count].shaderperm = shaderperm;
	batch->shaderbatch[count].texturebatch = 0;

	return addEntityToShaderbatche(ent, &batch->shaderbatch[count]);
//	return addEntityToShaderbatche(ent, &b[count]);
}




//todo more streamlined for objects
int addObjectToModelbatche(worldobject_t * obj, modelbatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	if(!batch->matlist) count = 0;
	//doesnt have texture in it
	batch->count++;
	batch->matlist = realloc(batch->matlist, batch->count * sizeof(matrix4x4_t));
	batch->matlist[count] = obj->mat;
//	batch->matlist = realloc(batch->cammatlist, batch->count * sizeof(matrix4x4_t));
//	Matrix4x4_Concat(&cammatlist[count], viewport mat, ent->mat);
//	batch->cammatlist[count] = ent->mat;
	return count+1;
}
int addObjectToTexturebatche(worldobject_t * obj, texturebatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	int modelid = obj->modelid;
//	modelbatche_t * b = batch->modelbatch;
	if(!batch->modelbatch) count = 0;
//	if(!b) count = 0;
	int i;
	for(i = 0; i < count; i++){
		if(batch->modelbatch[i].modelid == modelid){
//		if(b[i].modelid == modelid){
			return addObjectToModelbatche(obj, &batch->modelbatch[i]);
//			return addObjectToModelbatche(obj, &b[i]);
		}
	}
	//doesnt have texture in it
	batch->count++;
	batch->modelbatch = realloc(batch->modelbatch, batch->count * sizeof(modelbatche_t));
//	batch->modelbatch = realloc(b, batch->count * sizeof(modelbatche_t));
//	b = batch->modelbatch = realloc(b, batch->count * sizeof(modelbatche_t));
	batch->modelbatch[count].count = 0;
	batch->modelbatch[count].modelid = modelid;
	batch->modelbatch[count].matlist = 0;
//	b[count].count = 0;
//	b[count].modelid = modelid;
//	b[count].matlist = 0;

	return addObjectToModelbatche(obj, &batch->modelbatch[count]);
//	return addObjectToModelbatche(obj, &b[count]);
}



int addObjectToShaderbatche(worldobject_t * obj, shaderbatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	int textureid = obj->textureid;
	if(!batch->texturebatch) count = 0;
	int i;
	for(i = 0; i < count; i++){
		if(batch->texturebatch[i].textureid == textureid){
			return addObjectToTexturebatche(obj, &batch->texturebatch[i]);
		}
	}
	//doesnt have shader in it
	batch->count++;
	batch->texturebatch = realloc(batch->texturebatch, batch->count * sizeof(texturebatche_t));
	batch->texturebatch[count].count = 0;
	batch->texturebatch[count].textureid = textureid;
	batch->texturebatch[count].modelbatch = 0;

	return addObjectToTexturebatche(obj, &batch->texturebatch[count]);
}



int addObjectToRenderbatche(worldobject_t * obj, renderbatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	int shaderid = obj->shaderid;
	int shaderperm = obj->shaderperm;
	if(!batch->shaderbatch) count = 0;
	int i;
	for(i = 0; i < count; i++){
		if(batch->shaderbatch[i].shaderid == shaderid && batch->shaderbatch[i].shaderperm == shaderperm){
			return addObjectToShaderbatche(obj, &batch->shaderbatch[i]);
		}
	}
	//doesnt have shader in it
	batch->count++;
	batch->shaderbatch = realloc(batch->shaderbatch, batch->count * sizeof(shaderbatche_t));
	batch->shaderbatch[count].count = 0;
	batch->shaderbatch[count].shaderid = shaderid;
	batch->shaderbatch[count].shaderperm = shaderperm;
	batch->shaderbatch[count].texturebatch = 0;

	return addObjectToShaderbatche(obj, &batch->shaderbatch[count]);
}


int cleanupModelbatche(modelbatche_t * batch){
	if(!batch) return FALSE;
	if(batch->matlist)free(batch->matlist);
	batch->count = 0;
	batch->matlist = 0;
	return TRUE;
}
int cleanupTexturebatche(texturebatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	if(!batch->modelbatch) count = 0;
	int i;
	for(i = 0; i < count; i++){
		cleanupModelbatche(&batch->modelbatch[i]);
	}
	if(batch->modelbatch)free(batch->modelbatch);
	batch->count = 0;
	batch->modelbatch = 0;
	return TRUE;
}
int cleanupShaderbatche(shaderbatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	if(!batch->texturebatch) count = 0;
	int i;
	for(i = 0; i < count; i++){
		cleanupTexturebatche(&batch->texturebatch[i]);
	}
	if(batch->texturebatch)free(batch->texturebatch);
	batch->count = 0;
	batch->texturebatch = 0;
	return TRUE;
}
int cleanupRenderbatche(renderbatche_t * batch){
	if(!batch) return FALSE;
	int count = batch->count;
	if(!batch->shaderbatch) count = 0;
	int i;
	for(i = 0; i < count; i++){
		cleanupShaderbatche(&batch->shaderbatch[i]);
	}
	if(batch->shaderbatch)free(batch->shaderbatch);
	batch->count = 0;
	batch->shaderbatch = 0;
	return TRUE;
}

//end of old

