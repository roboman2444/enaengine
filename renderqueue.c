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

GLint uboalignment = 0;

unsigned int ubodatasize = 0; //in bytes
unsigned int ubodataplace = 0; // in bytes
GLubyte * ubodata = 0;
//GLbyte * ubodata = 0;
//unsigned char * ubodata = 0;
GLuint renderqueueuboid = 0;

// RENDERQUEUE STUFF
//unsigned int renderqueuesize = 0;
//unsigned int renderqueueplace = 0;
//renderlistitem_t * renderqueue = 0;
unsigned int renderscratchsize = 0;
renderlistitem_t * renderscratch = 0;

unsigned int renderqueueCleanup(renderqueue_t *queue){
	if(!queue) return FALSE;
	renderlistitem_t * list = queue->list;
	if(!list) return FALSE;
	unsigned int i, counter = 0;
	unsigned int place = queue->place;
	for(i = 0; i < place; i++){
		if(list[i].flags & 1){
			free(list[i].data);
			counter++;
		}
	}
	free(list);
	memset(queue, 0, sizeof(renderqueue_t));
	return counter;
}



unsigned int renderqueueHalfQueue(renderqueue_t *queue){
	queue->size /= 2;
	queue->list = realloc(queue->list, queue->size * sizeof(renderlistitem_t));
	return queue->size;
}
unsigned int renderqueuePruneQueue(renderqueue_t *queue){
	queue->size = queue->place;
	queue->list = realloc(queue->list, queue->size * sizeof(renderlistitem_t));
	return queue->size;
}
unsigned int renderqueueHalfData(renderqueue_t *queue){
	queue->datasize /= 2;
	queue->data = realloc(queue->data, queue->datasize);
	return queue->datasize;
}
unsigned int renderqueuePruneData(renderqueue_t *queue){
	queue->datasize = queue->dataplace;
	queue->data = realloc(queue->data, queue->datasize);
	return queue->datasize;
}
unsigned int renderqueueHalfUBO(void){
	ubodatasize = ubodatasize/2;
	ubodata = realloc(ubodata, ubodatasize);
	return ubodatasize;
}
unsigned int renderqueuePruneUBO(void){
	ubodatasize = ubodataplace;
	ubodata = realloc(ubodata, ubodatasize);
	return ubodatasize;
}
void renderqueueHalfVBO(void){
	facedatasize = facedatasize/2;
	facedata = realloc(facedata, facedatasize * 3 * sizeof(GLuint));

	vertposdatasize = vertposdatasize/2;
	vertposdata = realloc(vertposdata, vertposdatasize * 3 * sizeof(GLfloat));

	vertnormdatasize = vertnormdatasize/2;
	vertnormdata = realloc(vertnormdata, vertnormdatasize * 3 * sizeof(GLfloat));

	verttcdatasize = verttcdatasize/2;
	verttcdata = realloc(verttcdata, verttcdatasize * 2 * sizeof(GLfloat));

	verttangentdatasize = verttangentdatasize/2;
	verttangentdata = realloc(verttangentdata, verttangentdatasize * 3 * sizeof(GLfloat));

	vertblendidatasize = vertblendidatasize/2;
	vertblendidata = realloc(vertblendidata, vertblendidatasize * sizeof(GLfloat));

	vertblendwdatasize = vertblendwdatasize/2;
	vertblendwdata = realloc(vertblendwdata, vertblendwdatasize * sizeof(GLfloat));


	vertdatasize = vertdatasize/2;
}

void renderqueueDraw(renderqueue_t * queue){
	//todo instancing support
	unsigned int i;
	unsigned int place = queue->place;
	renderlistitem_t * list = queue->list;
	for(i = 0; i < place; i++){
		list[i].draw(&list[i].data, 1);
		if(list[i].flags & 1) free(list[i].data);
	}
	//reset it
	queue->place = 0;
	queue->dataplace = 0;
}
void renderqueueSetup(const renderqueue_t * queue){
	//todo instancing support?
	unsigned int i;
	unsigned int place = queue->place;
	renderlistitem_t * list = queue->list;
	for(i = 0; i < place; i++){
		if(list[i].flags & 2) list[i].data = queue->data + list[i].dataoffset;
		list[i].setup(&list[i].data, 1);
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
void renderqueueRadixSort(const renderqueue_t * queue){
	if(renderscratchsize < queue->place){
		renderscratch = realloc(renderscratch, queue->place * sizeof(renderlistitem_t));
		renderscratchsize = queue->place;
	}
	renderlistitem_t * list = queue->list;
	unsigned int place = queue->place;
	//todo use some preprocesor magics here
	renderqueue_radix(0, place, list, renderscratch);
	renderqueue_radix(1, place, renderscratch, list);
	renderqueue_radix(2, place, list, renderscratch);
	renderqueue_radix(3, place, renderscratch, list);
	renderqueue_radix(4, place, list, renderscratch);
	renderqueue_radix(5, place, renderscratch, list);
	renderqueue_radix(6, place, list, renderscratch);
	renderqueue_radix(7, place, renderscratch, list);
	renderqueue_radix(8, place, list, renderscratch);
	renderqueue_radix(9, place, renderscratch, list);
//	renderqueue_radix(10,place, list, renderscratch);
//	renderqueue_radix(11,place, renderscratch, list);
	//todo SHOULD BE RADIXSORTSIZE-1
}





char addRenderlistitem(renderqueue_t * queue, renderlistitem_t r){
	if((!r.setup || !r.draw)) return FALSE;
	//check if it needs a resize
	unsigned int renderqueuenewsize = queue->place + 1; //todo need TWO +1s?
	if(renderqueuenewsize > queue->size){
		queue->list = realloc(queue->list, renderqueuenewsize * sizeof(renderlistitem_t));
		queue->size = renderqueuenewsize;
	}

	//copyable data
	if(r.flags & 2){
		unsigned int dataplace = queue->dataplace;
		unsigned int queuedatanewsize = dataplace + r.datasize;
		if(queuedatanewsize > queue->datasize){
			queue->data = realloc(queue->data, queuedatanewsize);
			queue->datasize = queuedatanewsize;
		}
		memcpy(queue->data + dataplace, r.data, r.datasize);
		if(r.flags & 1) free(r.data);//shouldn't be both copyable and freeable, because that is silly, but i gotta check anyway...
		r.data = 0;
		r.dataoffset = dataplace;
		queue->dataplace += r.datasize;
	}
	memcpy(&queue->list[queue->place], &r, sizeof(renderlistitem_t));
	queue->place++;
	return TRUE;
}
char createAndAddRenderlistitem(renderqueue_t * queue, const void * data, const unsigned int datasize, const renderqueueCallback_t setup, const renderqueueCallback_t draw, const unsigned char flags, const unsigned char sort[RADIXSORTSIZE]){
	renderlistitem_t r;
	r.data = (void*)data;
	r.datasize = datasize;
	r.setup = setup;
	r.draw = draw;
	r.flags = flags;
	memcpy(r.sort, sort, RADIXSORTSIZE);
	return addRenderlistitem(queue, r);
}

char flushUBOCacheToBuffers(void){
	if(!ubodataplace) return FALSE;
	glBindBuffer(GL_ARRAY_BUFFER, renderqueueuboid);
	glBufferData(GL_ARRAY_BUFFER, ubodataplace, ubodata, GL_DYNAMIC_DRAW);
	ubodataplace = 0;
	return TRUE;
}

//returns the offset, in bytes
int pushDataToUBOCache(const unsigned int size, const void * data){
	if(!size || !data) return -1;
	unsigned int mysize = (size + uboalignment-1) & ~(uboalignment-1);
	//check if it needs a resize
	unsigned int ubodatanewsize = ubodataplace + mysize;
	if(ubodatanewsize > ubodatasize){
		ubodata = realloc(ubodata, ubodatanewsize);
		ubodatasize = ubodatanewsize;
	}
	memcpy(ubodata + ubodataplace, data, size);
	int ubodataoldplace = ubodataplace;
	ubodataplace += mysize;
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
	unsigned int facedatanewsize = facedataplace + faceinsize;
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
		unsigned int newvertposdatasize = (vertdataplace + vertcount) * 3;
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
		unsigned int newvertnormdatasize = (vertdataplace + vertcount) * 3;
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
		unsigned int newverttcdatasize = (vertdataplace + vertcount) * 2;
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
		unsigned int newverttangentdatasize = (vertdataplace + vertcount) * 3;
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
		unsigned int newvertblendidatasize = (vertdataplace + vertcount);
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
		unsigned int newvertblendwdatasize = (vertdataplace + vertcount);
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
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uboalignment);
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

	glGenBuffers(1, &renderqueueuboid);

	return TRUE;
}
