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

unsigned int rendervertdatacount;
unsigned int rendervertdatasize;
rendervertdata_t * rendervertdatalist;

unsigned int rendervbodatacount;
unsigned int rendervbodatasize;
rendervbodata_t * rendervbodatalist;



//unsigned int renderqueuevboid = 0;
//vboseperate_t * renderqueuevbo;
vboseperate_t renderqueuevbo;

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







// will make it so i can have a no-malloc system, and so the rendervertdata can easily be found and uploaded to one buffer
rendervertdata_t * addVertDataToList(renderlist_t * list, rendervertdata_t data){
	unsigned int pos = list->count;
	list->count++;
	list->types = realloc(list->types, list->count * sizeof(renderlisttype));
	list->types[pos] = RENDERVERTDATA;
	list->renderlist = realloc(list->renderlist, list->count * sizeof(int));

	unsigned int vpos = rendervertdatacount;
	rendervertdatacount++;
	if(rendervertdatasize <= rendervertdatacount){
		rendervertdatasize +=1; // todo may change
		rendervertdatalist = realloc(rendervertdatalist, rendervertdatasize * sizeof(rendervertdata_t));
	}
	rendervertdatalist[vpos] = data;
	return &rendervertdatalist[vpos];
}
rendervbodata_t * addVBODataToList(renderlist_t * list, rendervbodata_t data){
	unsigned int pos = list->count;
	list->count++;
	list->types = realloc(list->types, list->count * sizeof(renderlisttype));
	list->types[pos] = RENDERVBO;
	list->renderlist = realloc(list->renderlist, list->count * sizeof(int));

	unsigned int vpos = rendervbodatacount;
	rendervbodatacount++;
	if(rendervbodatasize <= rendervbodatacount){
		rendervbodatasize +=1; // todo may change
		rendervbodatalist = realloc(rendervbodatalist, rendervbodatasize * sizeof(rendervbodata_t));
	}
	rendervbodatalist[vpos] = data;
	return &rendervbodatalist[vpos];
}
void cleanUpRenderlist(renderlist_t *list){
	free(list->renderlist);
	free(list->types);
	memset(list, 0, sizeof(renderlist_t));
//	free(list);
}


//todo might not use
void halfRenderListData(void){
	rendervertdatasize = rendervertdatasize/2;
	rendervertdatalist = realloc(rendervertdatalist, rendervertdatasize * sizeof(rendervertdata_t));
	rendervbodatasize = rendervbodatasize/2;
	rendervbodatalist = realloc(rendervbodatalist, rendervbodatasize * sizeof(rendervbodata_t));
}


void drawVertData(rendervertdata_t * d){
	//bind vbo
	//bind texture
	//bind shader
	//do UBO data or whatever, idk
	glDrawElements(GL_TRIANGLES, d->numfaces * 3, GL_UNSIGNED_INT, (void *)(d->facestart * 3 * sizeof(GLuint)));
}


int runThroughVertList(void){
	//todo move these to "pool" style mem allocs

	unsigned int i;
	unsigned int vertdatasize = 0;
	GLfloat * posvertdata = 0;
	GLfloat * normvertdata = 0;
	GLfloat * tcvertdata = 0;
	GLfloat * tangentvertdata = 0;
	GLfloat * blendivertdata = 0;
	GLfloat * blendwvertdata = 0;
	GLuint * facedata =0;
	unsigned int facedatasize = 0;
	for(i = 0; i < rendervertdatacount; i++){
		rendervertdata_t * data = &rendervertdatalist[i];

		if(!data->facedata) continue;
		unsigned int oldsize = vertdatasize;
		unsigned int numverts = data->numverts;
		unsigned int numfaces = data->numfaces;
		if(!numverts || !numfaces) continue;
		vertdatasize += numverts;
		posvertdata = 	realloc(posvertdata,	vertdatasize * 3 * sizeof(GLfloat));
		normvertdata = 	realloc(normvertdata,	vertdatasize * 3 * sizeof(GLfloat));
		tcvertdata = 	realloc(tcvertdata,	vertdatasize * 2 * sizeof(GLfloat));
		tangentvertdata=realloc(tangentvertdata,vertdatasize * 3 * sizeof(GLfloat));
		blendivertdata =realloc(blendivertdata,	vertdatasize * 1 * sizeof(GLfloat));
		blendwvertdata =realloc(blendwvertdata,	vertdatasize * 1 * sizeof(GLfloat));


		unsigned int oldfacesize = facedatasize;
		facedatasize += numfaces;
		facedata = realloc(facedata, facedatasize * 3 * sizeof(GLuint));

		if(data->posvertdata){
			memcpy(&posvertdata[oldsize * 3], data->posvertdata, 3 * numverts * sizeof(GLfloat));
		} else {
			memset(&posvertdata[oldsize * 3], 0, 3 * numverts * sizeof(GLfloat));
		}
		if(data->normvertdata){
			memcpy(&normvertdata[oldsize * 3], data->normvertdata, 3 * numverts * sizeof(GLfloat));
		} else {
			memset(&normvertdata[oldsize * 3], 0, 3 * numverts * sizeof(GLfloat));
		}
		if(data->tcvertdata){
			memcpy(&tcvertdata[oldsize * 2], data->tcvertdata, 2 * numverts * sizeof(GLfloat));
		} else {
			memset(&tcvertdata[oldsize * 2], 0, 2 * numverts * sizeof(GLfloat));
		}
		if(data->tangentvertdata){
			memcpy(&tangentvertdata[oldsize * 3], data->tangentvertdata, 3 * numverts * sizeof(GLfloat));
		} else {
			memset(&tangentvertdata[oldsize * 3], 0, 3 * numverts * sizeof(GLfloat));
		}
		if(data->blendivertdata){
			memcpy(&blendivertdata[oldsize], data->blendivertdata, numverts * sizeof(GLfloat));
		} else {
			memset(&blendivertdata[oldsize], 0, numverts * sizeof(GLfloat));
		}
		if(data->blendwvertdata){
			memcpy(&blendwvertdata[oldsize], data->blendwvertdata, numverts * sizeof(GLfloat));
		} else {
			memset(&blendwvertdata[oldsize], 0, numverts * sizeof(GLfloat));
		}


		unsigned int j;
		unsigned int dataput = oldfacesize*3;
		unsigned int dataread = 0;
		GLuint * indata = data->facedata;
		for(j = 0; j < numfaces; j++){
			facedata[dataput++] = indata[dataread++] + oldsize;
			facedata[dataput++] = indata[dataread++] + oldsize;
			facedata[dataput++] = indata[dataread++] + oldsize;
		}
		data->facestart = oldfacesize;



	}

	glBindBuffer(GL_ARRAY_BUFFER, renderqueuevbo.vboposid);
	glBufferData(GL_ARRAY_BUFFER, 3 * vertdatasize * sizeof(GLfloat), posvertdata, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, renderqueuevbo.vbonormid);
	glBufferData(GL_ARRAY_BUFFER, 3 * vertdatasize * sizeof(GLfloat), normvertdata, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, renderqueuevbo.vbotcid);
	glBufferData(GL_ARRAY_BUFFER, 2 * vertdatasize * sizeof(GLfloat), tcvertdata, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, renderqueuevbo.vbotangentid);
	glBufferData(GL_ARRAY_BUFFER, 3 * vertdatasize * sizeof(GLfloat), posvertdata, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, renderqueuevbo.vboblendiid);
	glBufferData(GL_ARRAY_BUFFER, vertdatasize * sizeof(GLfloat), blendivertdata, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, renderqueuevbo.vboblendwid);
	glBufferData(GL_ARRAY_BUFFER, vertdatasize * sizeof(GLfloat), blendwvertdata, GL_DYNAMIC_DRAW);



	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderqueuevbo.indicesid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, facedatasize * 3 * sizeof(GLuint), facedata, GL_DYNAMIC_DRAW);

	return TRUE;
}



int readyRenderQueueVBO(void){
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

	return TRUE;
}
