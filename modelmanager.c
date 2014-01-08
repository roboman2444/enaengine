#include <GL/glew.h> //may be unneeded
#include <GL/gl.h>
#include <sys/stat.h> //todo filesys

#include "globaldefs.h"
#include "texturemanager.h"
#include "modelmanager.h"
//#include "vbomanager.h"

int modelnumber = 0;
model_t *modellist;

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
	char * testline;
	while(fgets(line, 300, f)){
		testline = line;
		while(*testline == ' ' || *testline == '\t') testline+=sizeof(char); //should be 1 //take off early white spaces
		if(testline > 200 + line) continue;
		if(!strncmp(testline, "v ", 2)) vertcount++;
		else if(!strncmp(testline, "vt", 2)) tccount++;
		else if(!strncmp(testline, "vp", 2)) normcount++;
		else if(!strncmp(testline, "f ", 2)) facecount++;
	}
//	if(vertcount + tccount + normcount != vertcount*3) return FALSE;
	if(!vertcount) return FALSE; // no verts
	float * vertbuffer = malloc(3*vertcount*sizeof(GLfloat));
	float * tcbuffer = malloc(2*tccount*sizeof(GLfloat));
	float * normbuffer = malloc(3*normcount*sizeof(GLfloat));
	//GLuint  * facebuffer = malloc(9*facecount*sizeof(GLuint)); //one for verts, tc, and normals
	int  * facebuffer = malloc(9*facecount*sizeof(GLuint)); //one for verts, tc, and normals
	rewind(f);


	unsigned int readvert = 0;
	unsigned int readnorm = 0;
	unsigned int readtc = 0;
	unsigned int readface = 0;
	while(fgets(line, 300, f)){
		testline = line;
		while(*testline == ' ' || *testline == '\t') testline+=sizeof(char); //should be 1 //take off early white spaces
		if(testline > 200 + line) continue;
		if(!strncmp(testline, "v ", 2)){
			if(readvert >= vertcount) break; //todo debug
//			sscanf(line," v %f %f %f",&vertbuffer[readvert++],&vertbuffer[readvert++],&vertbuffer[readvert++]);
			sscanf(line," v %f %f %f",&vertbuffer[(readvert*3)],&vertbuffer[(readvert*3)+1],&vertbuffer[(readvert*3)+2]);
			readvert++;
		}
		else if(!strncmp(testline, "vt", 2)){
			if(readtc >= tccount) break; //todo debug
//			sscanf(line," vt %f %f",&tcbuffer[readtc++],&tcbuffer[readtc++]);
			sscanf(line," vt %f %f",&tcbuffer[(readtc*2)],&tcbuffer[(readtc*2)+1]);
			readtc++;
		}
		else if(!strncmp(testline, "vp", 2)){
			if(readnorm >= normcount) break; //todo debug
			sscanf(line," vp %f %f %f",&normbuffer[(readnorm*3)],&normbuffer[(readnorm*3)+1], &normbuffer[(readnorm*3)+2]);
			readnorm++;
		}
		else if(!strncmp(testline, "f ", 2)){
			if(readface >=facecount) break;
			//sscanf(line," vp %d %d %d", &facebuffer[readface++], &facebuffer[readface++], &facebuffer[readface++]);
			if(sscanf(line," f %d/%d/%d %d/%d/%d %d/%d/%d",
				&facebuffer[(readface*9)], &facebuffer[(readface*9)+1], &facebuffer[(readface*9)+2],
				&facebuffer[(readface*9)+3], &facebuffer[(readface*9)+4], &facebuffer[(readface*9)+5],
				&facebuffer[(readface*9)+6], &facebuffer[(readface*9)+7], &facebuffer[(readface*9)+8]
			))readface++;
			else if(sscanf(line," f %d/%d %d/%d %d/%d",
				&facebuffer[(readface*9)], &facebuffer[(readface*9)+1], &facebuffer[(readface*9)+2],
				&facebuffer[(readface*9)+3], &facebuffer[(readface*9)+4], &facebuffer[(readface*9)+5]
			))readface++;
			else if(sscanf(line," f %d//%d %d//%d %d//%d",
				&facebuffer[(readface*9)], &facebuffer[(readface*9)+1], &facebuffer[(readface*9)+2],
				&facebuffer[(readface*9)+6], &facebuffer[(readface*9)+7], &facebuffer[(readface*9)+8]
			))readface++;
			else return 0; //todo debug... most likely case is that it has a quad
		}
	}
	free(line);
	fclose(f);
	if(readface != facecount) return 0; //todo actually debug and whatnot
	if(readtc != tccount) return 0; //todo actually debug and whatnot
	if(readtc != vertcount) return 0; //todo actually debug and whatnot
	if(readnorm != normcount) return 0; //todo actually debug and whatnot
	//todo sort texcoords and normals to verts... in each face thing, organize verts into a new buffer
	//todo create VAO
	//todo create VBOs and load crap into them
	//todo set flags in the model
	//todo curse more at obj for being stupid
	free(vertbuffer);
	free(normbuffer);
	free(facebuffer);
	free(tcbuffer);
	return TRUE;
}
model_t createAndLoadModel(char * name){
	char *statictypes[] = {".obj"}; //todo filesys
	char *animtypes[] = {".dpm"}; //todo filesys //todo

	model_t m;
	m.type = 0; // error
	char * filename = malloc(200);
	struct stat s;
	int n;
	for(n = 0; n < sizeof(statictypes) &&  statictypes[n]; n++){
		sprintf(filename, "%s%s", name, statictypes[n]);
		if(!stat(filename, &s)){ //if file exists... i guess
			//TODO CALL SOME SORTA LOADING FUNCTION
			if(!loadModelOBJ(&m, filename)) return m;
			m.type = 1;
			m.name = malloc(sizeof(name));
			strcpy(m.name, name);
			free(filename);
			return m;
		}
	}
	for(n = 0; n < sizeof(animtypes) &&  animtypes[n]; n++){
		sprintf(filename, "%s%s", name, animtypes[n]);
		if(!stat(filename, &s)){ //if file exists... i guess
			//TODO CALL SOME SORTA LOADING FUNCTION... ANIMATED
			m.type = 2;
			m.name = malloc(sizeof(name));
			strcpy(m.name, name);
			free(filename);
			return m;
		}
	}
	if(!m.type)/*some sorta error*/;

	free(filename);
	return m;
}
