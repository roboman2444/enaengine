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
int shaderArrayLastTaken = -1;
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
	//much todo

	memset(shader, 0, sizeof(shaderprogram_t));
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
	if(shaderArrayLastTaken < shaderArrayFirstOpen) shaderArrayLastTaken = shaderArrayFirstOpen;
	return &shaderlist[shaderArrayFirstOpen];

}

//shaderpermutation_t compilePermutation(shaderprogram_t * shader, int permutation){
shaderpermutation_t createPermutation(shaderprogram_t * shader, int permutation){
//	consolePrintf("readying shader %s with permutation %i", shader->name, permutation);
	shaderpermutation_t perm;
	memset(&perm, 0 , sizeof(shaderpermutation_t));
	perm.permutation = permutation;

	if(!(shader->type & 2)) return perm;
	GLuint vertid = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragid = glCreateShader(GL_FRAGMENT_SHADER);
	//todo errorcheck

	char ** shaderstring = malloc((shader->numdefines+1) * sizeof(char *)); // if it has no defines, it will be 1 so its ok anyway
	memset(shaderstring, 0, (shader->numdefines +1) * sizeof(char *));
	if(shader->type & 1){
		int i;
		for(i = 0; i < shader->numdefines; i++){
			if(permutation & 1<<i){
				GLuint l = strlen(shader->defines[i]) + 10;
				shaderstring[i] = malloc(l); // 8 for the extra #define , 1 for the \n, 1 for the \0
				memset(shaderstring[i], 0, l);
				snprintf(shaderstring[i], l, "#define %s", shader->defines[i]); //no need for the \n because the defines has it in them
			} else {
				shaderstring[i] = malloc(2* sizeof(char));
				memset(shaderstring[i], 0, 2);
				strcpy(shaderstring[i], "\0");
			}
		}
	}

	shaderstring[shader->numdefines] = shader->vertstring;
	glShaderSource(vertid, shader->numdefines+1, (const GLchar **) shaderstring, 0);
	shaderstring[shader->numdefines] = shader->fragstring;
	glShaderSource(fragid, shader->numdefines+1, (const GLchar **) shaderstring, 0);
	//if i set shadersource length to null, it does it by null char looking
	//todo geom shader

	if(shader->type & 1){
		int i;
		for(i = 0; i < shader->numdefines; i++) if(shaderstring[i]) free(shaderstring[i]); // doesnt free the vertstring or fragstring
	}
	free(shaderstring);

	perm.compiled = 1;
	glCompileShader(vertid);
	glCompileShader(fragid);
	int fail = 0;
	int status;

	glGetShaderiv(fragid, GL_COMPILE_STATUS, &status);
	if(status == GL_FALSE){
//			consolePrintf("Shader %s compile failed\n", shader->name);
			fail = 1;
	}
	glGetShaderiv(vertid, GL_COMPILE_STATUS, &status);
	if(status == GL_FALSE){
//			consolePrintf("Shader %s compile failed\n", shader->name);
			fail = 1;
	}

	//todo geom shader
	GLuint programid = glCreateProgram();
	//TODO errorcheck
	glAttachShader(programid, vertid);
	glAttachShader(programid, fragid);
//	glUseProgram(programid);
	glBindFragDataLocation(programid, 0, "fragColor"); //todo move this
	glBindFragDataLocation(programid, 1, "normColor"); //todo move this
	glBindFragDataLocation(programid, 2, "specColor"); //todo move this


	glBindAttribLocation(programid, POSATTRIBLOC, "posattrib");
	glBindAttribLocation(programid, NORMATTRIBLOC, "normattrib");
	glBindAttribLocation(programid, TCATTRIBLOC, "tcattrib");



	glLinkProgram(programid);
	perm.id = programid;

	//TODO errorcheck
	glGetProgramiv(programid, GL_LINK_STATUS, &status);
	if(status == GL_FALSE){
//			consolePrintf("Shader %s compile failed\n", shader->name);
			fail = 1;
	}

	if(printProgramLogStatus(programid) || fail){
		fail = TRUE;
//		if(shader->type & 1){
			char * error = malloc((100* shader->numdefines) + 100);
			sprintf(error, "Shader %s compile failed. Permutations:\n", shader->name);
			int i, count = 0;
				for(i = 0; i < shader->numdefines; i++){
				if(permutation & 1<<i){
					sprintf(error, "%s%s", error, shader->defines[i]);
					count++;
				}
			}
			sprintf(error, "%sLine offset in file: %i\n", error, count);
			consoleNPrintf(strlen(error)+1,error);
			free(error);
/*		} else {
			consolePrintf("Shader %s compile failed\n", shader->name);
		}
*/
		perm.compiled = 1;
//		return perm;
	}
	if(fail) return perm;
	glUseProgram(programid);
//set up texture uniforms

	char * texstring = malloc(10);
	int i;
	for(i = 0; i < 16; i++){
		sprintf(texstring, "texture%i", i);
		perm.texturespos[i] = glGetUniformLocation(programid, texstring);
	}
	free(texstring);

//set up other uniforms
	perm.unimat40 = glGetUniformLocation(programid, "unimat40");
	perm.unimat41 = glGetUniformLocation(programid, "unimat41");
	perm.univec40 = glGetUniformLocation(programid, "univec40");
	perm.univec30 = glGetUniformLocation(programid, "univec30");
	perm.univec20 = glGetUniformLocation(programid, "univec20");
	perm.unifloat0 =glGetUniformLocation(programid, "unifloat0");

	perm.compiled = 2;
//	consolePrintf("Shader %s compile successful\n", shader->name);
	char * error = malloc((100* shader->numdefines) + 100);
	sprintf(error, "Shader %s compile success. Permutations:\n", shader->name);
	int count = 0;
	for(i = 0; i < shader->numdefines; i++){
		if(permutation & 1<<i){
			sprintf(error, "%s%s", error, shader->defines[i]);
			count++;
		}
	}
	consoleNPrintf(strlen(error)+1,error);
	free(error);

	return perm;
}


shaderprogram_t createAndReadyShader(char * name){
	shaderprogram_t shader;
	memset(&shader, 0 , sizeof(shaderprogram_t));


// populate define list
	char * definename = malloc(strlen(name)+8);
	sprintf(definename, "%s.define", name);
//	strcpy(definename, name);strcat(definename, ".define");

	FILE *f;
	if(!(f = fopen(definename, "r"))){
		//todo debug?
	} else {
		shader.defines = malloc(32 * sizeof(char *));
		int i;
		for(i = 0; i < 32; i++){
			shader.defines[i] = malloc(100);
			if(fgets(shader.defines[i], 100, f)){
				shader.defines[i] = realloc(shader.defines[i], strlen(shader.defines[i])+1);
				consolePrintf("%s\n",shader.defines[i]);
			} else {
//				consolePrintf("%s\n",shader.defines[i]);

				free(shader.defines[i]);
				shader.defines[i] = 0;
				break;
			}
		}
		if(i){
			shader.type = shader.type | 1; //shader has a definelist
			shader.defines = realloc(shader.defines, i * sizeof(char *)); // might need a +1
			shader.numdefines = i;
		} else {
			free(shader.defines);
			shader.defines = 0;
		}
	}
	if(f)fclose(f);
	free(definename);


	char * vertname = malloc(strlen(name)+6);
	sprintf(vertname, "%s.vert", name);
	loadFileString(vertname, &shader.vertstring, &shader.vertlength, 1);
	free(vertname);
	if(shader.vertlength == 0){
		//todo
		if(shader.vertstring)free(shader.vertstring);
		shader.type = 0;
		return shader;
	}
	char * fragname = malloc(strlen(name)+6);
	sprintf(fragname, "%s.frag", name);
//	strcpy(vertname, name);strcat(vertname, ".frag");
	loadFileString(fragname, &shader.fragstring, &shader.fraglength, 1);
	free(fragname);
	if(shader.fraglength == 0){
		//todo
		if(shader.vertstring)free(shader.vertstring);
		shader.vertlength =0;
		if(shader.fragstring)free(shader.fragstring);
		shader.type = 0;
		return shader;
	}
	shader.type = shader.type | 2;

	char * geomname = malloc(strlen(name)+6);
	sprintf(geomname, "%s.geom", name);
	loadFileString(geomname, &shader.geomstring, &shader.geomlength, 0);
	free(geomname);
	if(shader.geomlength < 1){
		if(shader.geomstring)free(shader.geomstring);
	} else {
		shader.type = shader.type | 4;
	}
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
		return blen;
	}
	return FALSE;
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
shaderpermutation_t * addPermutationToShader(shaderprogram_t * shader, int permutation){
	if(!shader) return FALSE;
	unsigned int hashindex = (permutation * 0x1021) & (PERMHASHSIZE - 1);
	if(!shader->permhashtable[hashindex].compiled){
		shader->permhashtable[hashindex] = createPermutation(shader, permutation);
		return &shader->permhashtable[hashindex];
	}
	shaderpermutation_t *p;
	for (p = &shader->permhashtable[hashindex]; p; p = p->next){
		if(p->permutation == permutation) return p; // already done
	}
	shaderpermutation_t *newp = malloc(sizeof(shaderpermutation_t));
	*newp = createPermutation(shader, permutation);
//	if(!newp->type){
//		free(newp);
//		return FALSE;
//	}
	p->next = newp;
	return newp;
}

shaderpermutation_t * findShaderPermutation(shaderprogram_t * shader, int permutation){
	if(!shader) return FALSE;
	unsigned int hashindex = (permutation * 0x1021) & (PERMHASHSIZE - 1);
	shaderpermutation_t *p;
	for (p = &shader->permhashtable[hashindex]; p; p = p->next){
		if(p->permutation == permutation) return p;
	}
	return FALSE;
}
/*
GLint findShaderAttribPos(shaderprogram_t * shader, char * name){
	GLint id = glGetAttribLocation(shader->id, name);
	return id;
}
*/


int bindShaderPerm(shaderpermutation_t * perm){
	if(!perm) return FALSE;
	GLuint id = perm->id;
	glUseProgram(id);
//set texture spaces
	int i;
	GLuint *tp = &perm->texturespos[0];
	for(i = 0; i < 16; i++){
		if(tp[i] > -1)
			glUniform1i(tp[i], i);
//		consolePrintf("texture space %i at uniform loc %i\n", i, perm->texturespos[i]);
	}
	//todo error check
	return TRUE;
}
