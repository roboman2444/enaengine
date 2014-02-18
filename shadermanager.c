//gloabl includes
#include <GL/glew.h>
#include <GL/gl.h>

//local includes
#include "globaldefs.h"
#include "hashtables.h"
#include "shadermanager.h"
#include "filemanager.h"
#include "console.h"

int shadercount = 0;
int shaderArrayFirstOpen = 0;
int shaderArrayLastTaken = 0;
int shaderArraySize = 0;
int shadersOK = 0;

shaderprogram_t *shaderlist;

hashbucket_t shaderhashtable[MAXHASHBUCKETS];

int initShaderSystem(void){
	memset(shaderhashtable, 0, MAXHASHBUCKETS * sizeof(hashbucket_t));

//	shaderprogram_t none = {"default", 0, 0, 0};
	if(shaderlist) free(shaderlist);
	shaderlist = 0;
//	shaderlist = malloc(0 * sizeof(shaderprogram_t));
//	defaultShader = addProgramToList(none);
	shadersOK = TRUE;
	return TRUE; // todo error check
}

shaderprogram_t * findShaderByNameRPOINT(char * name){
	return returnShaderById(findByNameRINT(name, shaderhashtable));
}
int findShaderByNameRINT(char * name){
	return findByNameRINT(name, shaderhashtable);
}
int deleteShader(int id){
	int shaderindex = (id & 0xFFFF);
	shaderprogram_t * shader = &shaderlist[shaderindex];
	if(shader->myid != id) return FALSE;
	if(!shader->name) return FALSE;
	deleteFromHashTable(shader->name, id, shaderhashtable);
	free(shader->name);

	//todo

	bzero(shader, sizeof(shaderprogram_t));
	if(shaderindex < shaderArrayFirstOpen) shaderArrayFirstOpen = shaderindex;
	for(; shaderArrayLastTaken > 0 && !shaderlist[shaderArrayLastTaken].type; shaderArrayLastTaken--);
	return TRUE;
}

shaderprogram_t * returnShaderById(int id){
	int shaderindex = (id & 0xFFFF);
	shaderprogram_t * shader = &shaderlist[shaderindex];
	if(!shader->type) return FALSE;
	if(shader->myid == id) return shader;
	return FALSE;
}

int addShaderRINT(shaderprogram_t shader){
	shadercount++;
	for(; shaderArrayFirstOpen < shaderArraySize && shaderlist[shaderArrayFirstOpen].type; shaderArrayFirstOpen++);
	if(shaderArrayFirstOpen == shaderArraySize){	//resize
		shaderArraySize++;
		shaderlist = realloc(shaderlist, shaderArraySize * sizeof(shaderprogram_t));
	}
	shaderlist[shaderArrayFirstOpen] = shader;
	int returnid = (shadercount << 16) | shaderArrayFirstOpen;
	shaderlist[shaderArrayFirstOpen].myid = returnid;

	addToHashTable(shaderlist[shaderArrayFirstOpen].name, returnid, shaderhashtable);
	if(shaderArrayLastTaken < shaderArrayFirstOpen) shaderArrayLastTaken = shaderArrayFirstOpen; //todo redo
	return returnid;
}
shaderprogram_t * addShaderRPOINT(shaderprogram_t shader){
	shadercount++;
	for(; shaderArrayFirstOpen < shaderArraySize && shaderlist[shaderArrayFirstOpen].type; shaderArrayFirstOpen++);
	if(shaderArrayFirstOpen == shaderArraySize){	//resize
		shaderArraySize++;
		shaderlist = realloc(shaderlist, shaderArraySize * sizeof(shaderprogram_t));
	}
	shaderlist[shaderArrayFirstOpen] = shader;
	int returnid = (shadercount << 16) | shaderArrayFirstOpen;
	shaderlist[shaderArrayFirstOpen].myid = returnid;

	addToHashTable(shaderlist[shaderArrayFirstOpen].name, returnid, shaderhashtable);
	//todo maybe have shader have a hash variable, so i dont have to calculate it again if i want to delete... maybe
	if(shaderArrayLastTaken < shaderArrayFirstOpen) shaderArrayLastTaken = shaderArrayFirstOpen;
//	printf("shaderarraysize = %i\n", shaderArraySize);
//	printf("shadercount = %i\n", shadercount);

	return &shaderlist[shaderArrayFirstOpen];

}


shaderprogram_t createAndReadyShader(char * name){
	shaderprogram_t shader;
	memset(&shader, 0 , sizeof(shaderprogram_t));
	char * vertname = malloc(strlen(name)+6);
	strcpy(vertname, name);strcat(vertname, ".vert");
	loadFileString(vertname, &shader.vertstring, &shader.vertlength, 2);
	free(vertname);
	if(shader.vertlength == 0){
		//todo
		if(shader.vertstring)free(shader.vertstring);
		shader.type = 0;
		return shader;
	}
	char * fragname = malloc(strlen(name)+6);
	strcpy(vertname, name);strcat(vertname, ".frag");
	loadFileString(fragname, &shader.fragstring, &shader.fraglength, 2);
	free(fragname);
	if(shader.fraglength == 0){
		//todo
		if(shader.vertstring)free(shader.vertstring);
		shader.vertlength =0;
		if(shader.fragstring)free(shader.fragstring);
		shader.type = 0;
		return shader;
	}
	shader.type = 1;
/*
	char * geomname = malloc(strlen(name)+6);
	strcpy(vertname, name);strcat(vertname, ".geom");
	loadFileString(geomname, &shader.geomstring, &shader.geomlength, 2);
	free(geomname);
	if(geomlength == 0){
		if(shader.geomstring)free(shader.geomstring);
		shader.type = 1;
	} else {
		shader.type = 2;
	}
*/
	//todo debugmodes
	shader.name = malloc(strlen(name)+1);
	strcpy(shader.name, name);

	return shader;
}
/*
shaderprogram_t createAndLoadShader(char * name){
	shaderprogram_t shader;
	shader.id = 0;
	shader.vertexid = 0;
	shader.fragmentid = 0;
	char * vertname = malloc(strlen(name) + 6);

	strcpy(vertname, name);strcat(vertname, ".vert");

	char * vertstring;
	int vertlength;

	loadFileString(vertname, &vertstring, &vertlength, 2);

	free(vertname);
	if(vertlength == 0){	//error
		free(vertstring);
		return shader;
	}
	char * fragname = malloc(strlen(name) + 6); //add on 5 extra characters for .frag
	strcpy(fragname,name); strcat(fragname, ".frag");
	char * fragstring;
	int fraglength;
	loadFileString(fragname, &fragstring, &fraglength, 2);
	free(fragname);
	if(fraglength == 0){	//error
		free(fragstring);
		return shader;
	}
	GLuint vertid = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragid = glCreateShader(GL_FRAGMENT_SHADER);
	shader.vertexid = vertid;
	shader.fragmentid = fragid;
	glShaderSource(vertid, 1, (const GLchar**) &vertstring, &vertlength);
	glShaderSource(fragid, 1, (const GLchar**) &fragstring, &fraglength);
	free(vertstring); free(fragstring);
	//TODO errorcheck
	glCompileShader(vertid);
	glCompileShader(fragid);
	//TODO errorcheck
	//TODO errorcheck for compilation

	GLuint programid = glCreateProgram();
	//TODO errorcheck
	glAttachShader(programid, vertid);
	glAttachShader(programid, fragid);
	glBindFragDataLocation(programid, 0, "fragColor"); //todo move this
	glLinkProgram(programid);
	//TODO errorcheck
	printProgramLogStatus(programid);
//	shaderprogram_t prog = {name, programid, vertid, fragid};
//	int id = addProgramToList(prog);
	consolePrintf("shader %s has id %d\n", name, programid);

	shader.id = programid;
	shader.name = malloc(strlen(name)+1);
	strcpy(shader.name, name);
//	return id; //so far i am assuming that it works
	return shader;
}
*/
int printProgramLogStatus(int id){
	GLint blen = 0;
	glGetProgramiv(id, GL_INFO_LOG_LENGTH, &blen);
	if(blen > 1){
		GLchar *log = (GLchar *) malloc(blen);
		glGetProgramInfoLog(id, blen, 0, log);
		consoleNPrintf(blen + 16, "program log: %s \n", log); //too much for the console
		free(log);
		return FALSE;
	}
	return TRUE;
}
//UNTESTED
int getProgramLogStatus(int id, char ** output){
	GLint length = 0;
	glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
	if(length > 1){
		*output = malloc(length);
		glGetProgramInfoLog(id, length, 0, *output);
	}
	return length;
}

int createAndAddShaderRINT(char * name){
	int s = findShaderByNameRINT(name);
	if(s) return s;
	return addShaderRINT(createAndReadyShader(name));
}
shaderprogram_t * createAndAddShaderRPOINT(char * name){
	shaderprogram_t * s = findShaderByNameRPOINT(name);
	if(s) return s;
	return addShaderRPOINT(createAndReadyShader(name));
}
/*
GLint findShaderAttribPos(shaderprogram_t * shader, char * name){
	GLint id = glGetAttribLocation(shader->id, name);
	return id;
}
*/
