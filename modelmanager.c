#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>
#include <sys/stat.h> //todo filesys
#include <math.h>

#include "globaldefs.h"
#include "texturemanager.h"
#include "vbomanager.h"
#include "modelmanager.h"
#include "shadermanager.h"
#include "console.h"

int modelsOK = 0;
int modelnumber = 0;
model_t **modellist;

char *statictypes[] = {".obj"}; //todo filesys
char *animtypes[] = {".dpm"}; //todo filesys //todo


int initModelSystem(void){
	model_t none = {"default", findTextureGroupByName("default"), 0, 0};
	if(modellist) free(modellist);
	modellist = malloc(modelnumber * sizeof(model_t *));
	if(!modellist) memset(modellist, 0 , modelnumber * sizeof(model_t *));
	addModelToList(none);
	modelsOK = TRUE;
	return TRUE;
}
model_t * addModelToList(model_t model){
	model_t *pointmodel = malloc(sizeof(model_t));
	*pointmodel = model;
	int current = modelnumber;
	modelnumber++;
	modellist = realloc(modellist, modelnumber*sizeof(model_t*));
	modellist[current] = pointmodel;
//	modellist[current].name = malloc(sizeof(*model.name));
//	strcpy(modellist[current].name, model.name);
	return pointmodel;
}
model_t * findModelByName(char * name){
	int i;
	for (i=0; i<modelnumber; i++){
		if(!strcmp(name, modellist[i]->name)) return modellist[i];
	}
	return modellist[0];
}
model_t * createAndAddModel(char * name){
	return addModelToList(createAndLoadModel(name));
//	return &modellist[addModelToList(createAndLoadModel(name))];
}

int generateNormalsFromMesh(GLfloat * vertbuffer, GLfloat * normbuffer, GLuint * indices, GLuint indicecount, GLuint vertcount, int areaweighting){
	//used http://devmaster.net/posts/6065/calculating-normals-of-a-mesh and darkplaces model_shared.c as a ref
	int i;
	for(i = 0; i < indicecount; i += 3 ){
		float *p1 = &vertbuffer[indices[i+0]*3];
		float *p2 = &vertbuffer[indices[i+1]*3];
		float *p3 = &vertbuffer[indices[i+2]*3];
		float v1[3];
		float v2[3];
		int n;
		for(n = 0; n < 3; n++){
			v1[n] = p2[n] - p1[n];
			v2[n] = p3[n] - p1[n];
		}
		float normal[3];
		normal[0] = v1[1] * v2[2] - v2[1] * v1[2];
		normal[1] = v1[2] * v2[0] - v2[2] * v1[0];
		normal[2] = v1[0] * v2[1] - v2[0] * v1[1];
		//if dontwant to do area weighting, normalize now
		if(!areaweighting){
			float length;
			length = sqrt((normal[0] * normal[0]) + (normal[1] * normal[1]) + (normal[2] * normal[2]));
			normal[0] /= length;
			normal[1] /= length;
			normal[2] /= length;
		}

		float *n1 = &normbuffer[indices[i+0]*3];
		float *n2 = &normbuffer[indices[i+1]*3];
		float *n3 = &normbuffer[indices[i+2]*3];

		for(n = 0; n < 3; n++){
			n1[n] += normal[n];
			n2[n] += normal[n];
			n3[n] += normal[n];
		}
	}
	//go through and renormalize
	for(i = 0; i < vertcount; i++){
		float * normal = &normbuffer[i*3];
		float length;
		length = sqrt((normal[0] * normal[0]) + (normal[1] * normal[1]) + (normal[2] * normal[2]));
		normal[0] /= length;
		normal[1] /= length;
		normal[2] /= length;
	}
	return TRUE;
}
int generateNormalsFromInterleavedMesh(GLfloat * interleavedbuffer, GLuint * indices, GLuint indicecount, GLuint vertcount, int stride, int areaweighting){
	//used http://devmaster.net/posts/6065/calculating-normals-of-a-mesh and darkplaces model_shared.c as a ref
	if(stride < 5) return FALSE;
	int i;
	for(i = 0; i < indicecount; i += 3 ){
		//todo set up something to have a varialbe offset
		float *p1 = &interleavedbuffer[indices[i+0]*stride];
		float *p2 = &interleavedbuffer[indices[i+1]*stride];
		float *p3 = &interleavedbuffer[indices[i+2]*stride];
		float v1[3];
		float v2[3];
		int j;
		for(j = 0; j < 3; j++){
			v1[j] = p2[j] - p1[j];
			v2[j] = p3[j] - p1[j];
		}
		float n[3];
		n[0] = v1[1] * v2[2] - v2[1] * v1[2];
		n[1] = v1[2] * v2[0] - v2[2] * v1[0];
		n[2] = v1[0] * v2[1] - v2[0] * v1[1];
		//if dontwant to do area weighting, normalize now
		if(!areaweighting){
			float length;
			length = sqrt((n[0] * n[0]) + (n[1] * n[1]) + (n[2] * n[2]));
			n[0] /= length;
			n[1] /= length;
			n[2] /= length;
		}
		//todo set up something to have a varialbe offset
		float *n1 = p1 + 3;
		float *n2 = p2 + 3;
		float *n3 = p3 + 3;

		for(j = 0; j < 3; j++){
			n1[j] += n[j];
			n2[j] += n[j];
			n3[j] += n[j];
		}
	}
	//go through and renormalize
	for(i = 0; i < vertcount; i++){
		//todo set up something to have a varialbe offset
		float * normal = &interleavedbuffer[(i*stride)+3];
		float length;
		length = sqrt((normal[0] * normal[0]) + (normal[1] * normal[1]) + (normal[2] * normal[2]));
		normal[0] /= length;
		normal[1] /= length;
		normal[2] /= length;
	}
	return TRUE;
}

int loadModelOBJ(model_t * m, char * filename){//todo flags
	unsigned int vertcount = 0;
	unsigned int tccount = 0;
	unsigned int normcount = 0;
	unsigned int facecount = 0;

	FILE *f;
	if(!(f = fopen(filename, "r"))) return FALSE;
	char * line = malloc(300*sizeof(char)); //max size of 300;
//	char * testline;
	int over;
	while(fgets(line, 300, f)){
//		testline = line;
//		while((*testline == ' ' || *testline == '\t') && testline < line + 200) testline+=sizeof(char); //should be 1 //take off early white spaces
//		if(testline > 200 + line) continue;
//		if(!strncmp(testline, "v ", 2)) vertcount++;
//		else if(!strncmp(testline, "vt", 2)) tccount++;
//		else if(!strncmp(testline, "vp", 2)) normcount++;
//		else if(!strncmp(testline, "f ", 2)) facecount++;
		for(over = 0; (line[over] == ' ' || line[over] == '\t') && over < 200; over++); //should be 1 //take off early white spaces
		if(!strncmp(line+over, "v ", 2)) vertcount++;
		else if(!strncmp(line+over, "vt", 2)) tccount++;
		else if(!strncmp(line+over, "vp", 2)) normcount++;
		else if(!strncmp(line+over, "f ", 2)) facecount++;

	}

//	if(vertcount + tccount + normcount != vertcount*3) return FALSE;
	if(!vertcount) return FALSE; // no verts
	float 	* vertbuffer = malloc(3*vertcount*sizeof(float));
	float 	* tcbuffer = malloc(2*tccount*sizeof(float));
	float 	* normbuffer = malloc(3*normcount*sizeof(float));
	//GLuint  * facebuffer = malloc(9*facecount*sizeof(GLuint)); //one for verts, tc, and normals
	int 	* facebuffer = malloc(9*sizeof(int)); //one for verts, tc, and normals
	GLuint 	* indicebuffer = malloc(3*facecount*sizeof(GLuint));
	GLfloat * interleavedbuffer = malloc(8*facecount*sizeof(GLfloat));




	bzero(vertbuffer, 3*vertcount*sizeof(float));
	bzero(normbuffer, 3*normcount*sizeof(float));
	bzero(tcbuffer,   2*  tccount*sizeof(float));
	bzero(facebuffer, 9*sizeof(int));
	bzero(indicebuffer, 3*facecount*sizeof(GLuint));
	bzero(interleavedbuffer, 8*facecount*sizeof(GLfloat));
	rewind(f);


	unsigned int readvert = 0;
	unsigned int readnorm = 0;
	unsigned int readtc = 0;
	unsigned int readface = 0;
	while(fgets(line, 300, f)){
		for(over = 0; (line[over] == ' ' || line[over] == '\t') && over < 200; over++); //should be 1 //take off early white spaces


//		if(!strncmp(testline, "v ", 2)){
		if(!strncmp(line+over, "v ", 2)){
			if(readvert >= vertcount) break; //todo debug
			sscanf(line," v %f %f %f",&vertbuffer[(readvert*3)],&vertbuffer[(readvert*3)+1],&vertbuffer[(readvert*3)+2]);
			readvert++;
		}
		else if(!strncmp(line+over, "vt", 2)){
			if(readtc >= tccount) break; //todo debug
			sscanf(line," vt %f %f",&tcbuffer[(readtc*2)],&tcbuffer[(readtc*2)+1]);
			readtc++;
		}
		else if(!strncmp(line+over, "vp", 2)){
			if(readnorm >= normcount) break; //todo debug
			sscanf(line," vp %f %f %f",&normbuffer[(readnorm*3)],&normbuffer[(readnorm*3)+1], &normbuffer[(readnorm*3)+2]);
			readnorm++;
		}
		else if(!strncmp(line+over, "f ", 2)){
			if(readface >= facecount) break;
//			printf("%i\n",readface);
			if(sscanf(line," f %d/%d/%d %d/%d/%d %d/%d/%d",
				&facebuffer[0], &facebuffer[1], &facebuffer[2],
				&facebuffer[3], &facebuffer[4], &facebuffer[5],
				&facebuffer[6], &facebuffer[7], &facebuffer[8]
			) == 9);
			else if(sscanf(line," f %d/%d %d/%d %d/%d",
				&facebuffer[0], &facebuffer[1],
				&facebuffer[3], &facebuffer[4],
				&facebuffer[6], &facebuffer[7]
			) == 6 ) facebuffer[2] = facebuffer[5] = facebuffer[8] = 0; // make sure they are 0s
			else if(sscanf(line," f %d//%d %d//%d %d//%d",
				&facebuffer[0], &facebuffer[2],
				&facebuffer[3], &facebuffer[5],
				&facebuffer[6], &facebuffer[8]
			) == 6) facebuffer[1] = facebuffer[4] = facebuffer[7] = 0; // make sure they are 0s
			else if(sscanf(line," f %d %d %d",
				&facebuffer[0], &facebuffer[3], &facebuffer[6]
			) == 3) facebuffer[1] = facebuffer[2] = facebuffer[4] = facebuffer[5] = facebuffer[7] = facebuffer[8] = 0; // make sure they are 0s
			else{
				return 0; //todo debug... most likely case is that it has a quad
			}

			int n;
			for(n = 0; n < 3; n++){ //two or more, use a for
				int vindice = 0;
				int tcindice = 0;
				int normindice = 0;
				if(facebuffer[(n*3)+0] > 0) vindice = facebuffer[(n*3)+0];
				else if(facebuffer[(n*3)+0] < 0) vindice = readvert+facebuffer[(n*3)+0];
				else break; //0, todo debug
				if(facebuffer[(n*3)+1] >= 0) tcindice = facebuffer[(n*3)+1];
				else if(facebuffer[(n*3)+1] < 0) tcindice = readtc+facebuffer[(n*3)+1];

				if(facebuffer[(n*3)+2] >= 0) normindice = facebuffer[(n*3)+2];
				else if(facebuffer[(n*3)+2] < 0) normindice = readnorm+facebuffer[(n*3)+2];



				vindice--; //1 to 0
				tcindice--;
				normindice--;

				//may not be doing this properly...
				indicebuffer[(readface*3)+n] = vindice;
				interleavedbuffer[(vindice*8)+0] = vertbuffer[(vindice*3)+0];
				interleavedbuffer[(vindice*8)+1] = vertbuffer[(vindice*3)+1];
				interleavedbuffer[(vindice*8)+2] = vertbuffer[(vindice*3)+2];
				if(normindice>=0){
					//todo maybe check if there is already something there...
					interleavedbuffer[(vindice*8)+3] = vertbuffer[(normindice*3)+0];
					interleavedbuffer[(vindice*8)+4] = vertbuffer[(normindice*3)+1];
					interleavedbuffer[(vindice*8)+5] = vertbuffer[(normindice*3)+2];
				}
				if(tcindice>=0){
					//todo check if there is maybe something there
					interleavedbuffer[(vindice*8)+6] = tcbuffer[(tcindice*2)+0];
					interleavedbuffer[(vindice*8)+7] = tcbuffer[(tcindice*2)+1];
				}
			}
			readface++;
		}
	}


	free(line);
	fclose(f);
	free(vertbuffer);
	free(normbuffer);
	free(facebuffer);
	free(tcbuffer);

	consolePrintf("Model %s has %i faces and %i verts\n", filename, readface, readvert);
//	int print;
//	for(print = 0; print < facecount*3; printf("%i ", indicebuffer[print++]));
//	printf("\n\n\n\n\n\n\n\n\n\n%ix%i data: ", vertcount, readvert);
//	for(print = 0; print < vertcount*8; printf("%f ", interleavedbuffer[print++]));
//	printf("\n");

	if(readface != facecount) return 0; //todo actually debug and free crap
	if(readtc != tccount) return 0; //todo actually debug and whatnot
	if(readvert != vertcount) return 0; //todo actually debug and whatnot
	if(readnorm != normcount) return 0; //todo actually debug and whatnot






/*
	float muhverts[32] = {  -0.5, -0.5, 0.0,  0.0, 0.0, 0.0,  0.0, 0.0,
				-0.5,  0.5, 0.0,  0.0, 0.0, 0.0,  0.0, 1.0,
				 0.5,  0.5, 0.0,  0.0, 0.0, 0.0,  1.0, 1.0,
				 0.5, -0.5, 0.0,  0.0, 0.0, 0.0,  1.0, 0.0};
	int muhindices[6] = { 0, 1, 2, 0, 2, 3};
*/
	//todo actually set flags for if gen verts is needed, and if it should area weight
	if(!readnorm){
//		consolePrintf("Generating vertex normals for Model %s with area weighting\n", filename);
		consolePrintf("Generating vertex normals for Model %s\n", filename);
		generateNormalsFromInterleavedMesh(interleavedbuffer, indicebuffer, facecount*3, vertcount, 8 , 0);

	}



	m->vbo = createAndAddVBO(m->name, m->type);
//	printf("%i\n",m->vbo->type);
//	printf("%i\n",m->vbo->vaoid);
//	printf("%i\n",m->vbo->vboid);
//	printf("%i\n",m->vbo->indicesid);
//	printf("%s\n",m->name);
//	printf("%s\n",m->vbo->name);
	if(!m->vbo) return 0; // todo free and error handle
	//the correct vao should be bound at this point.
//	printf("%s vao: %i\n", m->name, m->vbo->vaoid);
//	glBindVertexArray(m->vbo->vaoid);
	glBindBuffer(GL_ARRAY_BUFFER,m->vbo->vboid);
	glBufferData(GL_ARRAY_BUFFER, vertcount * 8 * sizeof(GLfloat), interleavedbuffer, GL_STATIC_DRAW);
	m->vbo->numverts = vertcount;
	free(interleavedbuffer);

	shaderprogram_t * program = findProgramByName("staticmodel");//todo per model materials and permutations
//	printf("\n%i\n", program->id);
	if(!program->id) return FALSE; //todo debug
	glUseProgram(program->id);

	GLint posattrib = findShaderAttribPos(program, "position"); //todo per model materials?
//	printf("%i\n", posattrib);
	glEnableVertexAttribArray(posattrib);
	glVertexAttribPointer(posattrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 0);

	GLint normattrib = findShaderAttribPos(program, "normal"); //todo per model materials?
//	printf("%i\n", normattrib);
	glEnableVertexAttribArray(normattrib);
	glVertexAttribPointer(normattrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));

	GLint tcattrib = findShaderAttribPos(program, "texCoord"); //todo per model materials?
//	printf("%i\n", tcattrib);
	glEnableVertexAttribArray(tcattrib);
	glVertexAttribPointer(tcattrib, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)(5*sizeof(GLfloat)));



	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m->vbo->indicesid);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(*indicebuffer), indicebuffer, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,facecount * 3 *sizeof(GLint), indicebuffer, GL_STATIC_DRAW);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER,6 * sizeof(GLint), muhindices, GL_STATIC_DRAW);
	m->vbo->numfaces = facecount;

	free(indicebuffer);
	//maybe use material based shading

	//todo set flags in the model
	//todo curse more at obj for being stupid
	return TRUE;
}
model_t createAndLoadModel(char * name){

	model_t m;
	m.type = 0; // error
	m.name = malloc(strlen(name)+1);
	strcpy(m.name, name);

	char * filename = malloc(200); //todo filesys and define a size
	struct stat s;
	int n;
	for(n = 0; n < sizeof(statictypes) &&  statictypes[n]; n++){
		sprintf(filename, "%s%s", name, statictypes[n]);
		if(!stat(filename, &s)){ //if file exists... i guess
			if(!loadModelOBJ(&m, filename)){
				free(filename);
				 return m;
			}
			m.type = 1;
			free(filename);
			return m;
		}
	}
	for(n = 0; n < sizeof(animtypes) &&  animtypes[n]; n++){
		sprintf(filename, "%s%s", name, animtypes[n]);
		if(!stat(filename, &s)){ //if file exists... i guess
			//TODO CALL SOME SORTA LOADING FUNCTION... ANIMATED
			m.type = 2;
			free(filename);
			return m;
		}
	}
	if(!m.type)/*some sorta error*/;

	free(filename);
	return m;
}

//if i want to load a static iqm model forsay
model_t createAndLoadTypeModel(char * name, char type){
	model_t m;
	m.type = 0; // error
	m.name = malloc(strlen(name)+1);
	strcpy(m.name, name);

	char * filename = malloc(200); //todo filesys and define a size
	struct stat s;
	int n;
	for(n = 0; n < sizeof(statictypes) &&  statictypes[n]; n++){
		sprintf(filename, "%s%s", name, statictypes[n]);
		if(!stat(filename, &s)){ //if file exists... i guess
			if(!loadModelOBJ(&m, filename)){
				free(filename);
				return m;
			}

			m.type = type;
			free(filename);

			return m;
		}
	}
	for(n = 0; n < sizeof(animtypes) &&  animtypes[n]; n++){
		sprintf(filename, "%s%s", name, animtypes[n]);
		if(!stat(filename, &s)){ //if file exists... i guess
			//TODO CALL SOME SORTA LOADING FUNCTION... ANIMATED
			m.type = type;
			free(filename);
			return m;
		}
	}
	if(!m.type)/*some sorta error*/;

	free(filename);
	return m;
}
