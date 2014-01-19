#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>
#include <sys/stat.h> //todo filesys

#include "globaldefs.h"
#include "texturemanager.h"
#include "vbomanager.h"
#include "modelmanager.h"
#include "shadermanager.h"

int modelnumber = 0;
model_t *modellist;

char *statictypes[] = {".obj"}; //todo filesys
char *animtypes[] = {".dpm"}; //todo filesys //todo


int initModelSystem(void){
	model_t none = {"default", findTextureGroupByName("default"), 0, 0};
	if(modellist) free(modellist);
	modellist = malloc(modelnumber * sizeof(model_t));
	if(!modellist) memset(modellist, 0 , modelnumber * sizeof(model_t));
	addModelToList(none);
	return TRUE;
}
int addModelToList(model_t model){
	int current = modelnumber;
	modelnumber++;
	modellist = realloc(modellist, modelnumber*sizeof(model_t));
	modellist[current] = model;
	modellist[current].name = malloc(sizeof(*model.name));
	strcpy(modellist[current].name, model.name);
	return current;
}
model_t * findModelByName(char * name){
	int i;
	for (i=0; i<modelnumber; i++){
		if(!strcmp(name, modellist[i].name)) return &modellist[i];
	}
	return &modellist[0];
}
model_t * createAndAddModel(char * name){
	return &modellist[addModelToList(createAndLoadModel(name))];
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
	int 	* indicebuffer = malloc(3*facecount*sizeof(int));
	GLfloat * interleavedbuffer = malloc(8*facecount*sizeof(GLfloat));




	bzero(vertbuffer, 3*vertcount*sizeof(float));
	bzero(normbuffer, 3*normcount*sizeof(float));
	bzero(tcbuffer,   2*  tccount*sizeof(float));
	bzero(facebuffer, 9*sizeof(int));
	bzero(indicebuffer, 3*facecount*sizeof(int));
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
				&facebuffer[0], &facebuffer[1], &facebuffer[2],
				&facebuffer[3], &facebuffer[4], &facebuffer[5]
			) == 6 ) facebuffer[6] = facebuffer[7] = facebuffer[8] = 0; // make sure they are 0s
			else if(sscanf(line," f %d//%d %d//%d %d//%d",
				&facebuffer[0], &facebuffer[1], &facebuffer[2],
				&facebuffer[6], &facebuffer[7], &facebuffer[8]
			) == 6) facebuffer[3] = facebuffer[4] = facebuffer[5] = 0; // make sure they are 0s
			else if(sscanf(line," f %d %d %d",
				&facebuffer[0], &facebuffer[1], &facebuffer[2]
			) == 3) facebuffer[3] = facebuffer[4] = facebuffer[5] = facebuffer[6] = facebuffer[7] = facebuffer[8] = 0; // make sure they are 0s
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
				indicebuffer[readface+n] = vindice;
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

//	int print;
	printf("Model %s has %i faces and %i verts\n", filename, readface, readvert);
//	for(print = 0; print < facecount*3; printf("%i ", indicebuffer[print++]));
//	printf("\n\n\n\n\n\n\n\n\n\n%ix%i data: ", vertcount, readvert);
//	for(print = 0; print < vertcount*8; printf("%f ", interleavedbuffer[print++]));
//	printf("\n");

	if(readface != facecount) return 0; //todo actually debug and free crap
	if(readtc != tccount) return 0; //todo actually debug and whatnot
	if(readvert != vertcount) return 0; //todo actually debug and whatnot
	if(readnorm != normcount) return 0; //todo actually debug and whatnot


	m->vbo = createAndAddVBO(m->name, m->type);
	if(!m->vbo) return 0; // todo free and error handle
	//the correct vao should be bound at this point.
	glBindBuffer(GL_ARRAY_BUFFER,m->vbo->vboid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(interleavedbuffer), interleavedbuffer, GL_STATIC_DRAW);
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indicebuffer), indicebuffer, GL_STATIC_DRAW);

	free(indicebuffer);
	//maybe use material based shading

	//todo set flags in the model
	//todo curse more at obj for being stupid
	return TRUE;
}
model_t createAndLoadModel(char * name){

	model_t m;
	m.type = 0; // error
	m.name = malloc(sizeof(*name));
	strcpy(m.name, name);

	char * filename = malloc(200); //todo filesys and define a size
	struct stat s;
	int n;
	for(n = 0; n < sizeof(statictypes) &&  statictypes[n]; n++){
		sprintf(filename, "%s%s", name, statictypes[n]);
		if(!stat(filename, &s)){ //if file exists... i guess
			if(!loadModelOBJ(&m, filename)) return m;
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
	m.name = malloc(sizeof(*name));
	strcpy(m.name, name);

	char * filename = malloc(200); //todo filesys and define a size
	struct stat s;
	int n;
	for(n = 0; n < sizeof(statictypes) &&  statictypes[n]; n++){
		sprintf(filename, "%s%s", name, statictypes[n]);
		if(!stat(filename, &s)){ //if file exists... i guess
			if(!loadModelOBJ(&m, filename)){
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
