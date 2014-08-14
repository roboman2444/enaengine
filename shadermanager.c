//gloabl includes
#include <GL/glew.h>
#include <GL/gl.h>

//local includes
#include "globaldefs.h"
#include "hashtables.h"
#include "shadermanager.h"
#include "filemanager.h"
#include "console.h"

#include "vbomanager.h" //for attriblocs
#include "ubomanager.h" //for ubo size

int shadercount = 0;
int shaderArrayFirstOpen = 0;
int shaderArrayLastTaken = -1;
int shaderArraySize = 0;
int shadersOK = 0;

shaderprogram_t *shaderlist;

shaderpermutation_t * shaderCurrentBound = 0;
GLuint currentprogram = 0;

hashbucket_t shaderhashtable[MAXHASHBUCKETS];

int initShaderSystem(void){
	memset(shaderhashtable, 0, MAXHASHBUCKETS * sizeof(hashbucket_t));

//	shaderprogram_t none = {"default", 0, 0, 0};
	if(shaderlist) free(shaderlist);
	shaderlist = 0;
//	shaderlist = malloc(0 * sizeof(shaderprogram_t));
//	defaultShader = addProgramToList(none);
	shaderCurrentBound = 0;
	shadersOK = TRUE;
	return TRUE; // todo error check
}

shaderprogram_t * findShaderByNameRPOINT(char * name){
	return returnShaderById(findByNameRINT(name, shaderhashtable));
}
int findShaderByNameRINT(char * name){
	return findByNameRINT(name, shaderhashtable);
}

int deleteShaderPermutation(shaderprogram_t * s, unsigned int permutation){
	if(!s) return FALSE;
	unsigned int hashindex = (permutation * 0x1021) & (PERMHASHSIZE - 1);
	shaderpermutation_t *p, *j = 0;
	for (p = &s->permhashtable[hashindex]; p; j = p, p = p->next){
		if(p->permutation == permutation){
			if(p->id) glDeleteProgram(p->id);
			if(!j){//we are at the first one... have to copy, not move
				j = p->next;
				memcpy(p, j, sizeof(shaderpermutation_t));
				memset(j, 0, sizeof(shaderpermutation_t));
				free(j);
			} else {
				j->next = p->next;
				memset(p, 0, sizeof(shaderpermutation_t));
				free(p);
			}
			return TRUE;
		}
	}
	return FALSE;
}


int deleteShaderProgram(int id){
	int shaderindex = (id & 0xFFFF);
	shaderprogram_t * shader = &shaderlist[shaderindex];
	if(shader->myid != id) return FALSE;
	if(!shader->name) return FALSE;
	deleteFromHashTable(shader->name, id, shaderhashtable);
	free(shader->name);

	int i;
	shaderpermutation_t *p, *j;
	for(i = 0; i < PERMHASHSIZE; i++){
		int del = 0;
		for (p = &shader->permhashtable[i]; p; p = j){
			j = p->next;
			//delete the program
			if(p->id) glDeleteProgram(p->id);
			//dont try to free the hashtable part, only the Linked list off of it
			if(del){
				memset(p, 0, sizeof(shaderpermutation_t));
				free(p);
			}
			del = 1;
		}
	}
	memset(shader->permhashtable, 0, PERMHASHSIZE * sizeof(shaderpermutation_t));
	if(shader->defines){
		for(i = 0; i < shader->numdefines; i++){
			if(shader->defines[i]) free(shader->defines[i]);
		}
		free(shader->defines); shader->defines = 0;
	}
	if(shader->fragstring) free(shader->fragstring); shader->fragstring = 0;
	if(shader->vertstring) free(shader->vertstring); shader->vertstring = 0;
	if(shader->geomstring) free(shader->geomstring); shader->geomstring = 0;




	memset(shader, 0, sizeof(shaderprogram_t));
	if(shaderindex < shaderArrayFirstOpen) shaderArrayFirstOpen = shaderindex;
	for(; shaderArrayLastTaken > 0 && !shaderlist[shaderArrayLastTaken].type; shaderArrayLastTaken--);
	return TRUE;
}

int deleteAllShaderPrograms(void){
	int count = 0;
	int i;
	for(i = 0; i <= shaderArrayLastTaken; i++){
		if(shaderlist[i].type){
			deleteShaderProgram(shaderlist[i].myid);
			count++;
		}
	}
	return count;
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

void pruneShaderList(void){
	if(shaderArraySize == shaderArrayLastTaken+1) return;
	shaderArraySize = shaderArrayLastTaken+1;
	shaderlist = realloc(shaderlist, shaderArraySize * sizeof(shaderprogram_t));
}



//shaderpermutation_t compilePermutation(shaderprogram_t * shader, int permutation){
shaderpermutation_t createPermutation(shaderprogram_t * shader, unsigned int permutation){
//	consolePrintf("readying shader %s with permutation %i", shader->name, permutation);
	shaderpermutation_t perm;
	memset(&perm, 0 , sizeof(shaderpermutation_t));
	perm.permutation = permutation;

	if(!(shader->type & 2)) return perm;
	GLuint vertid = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragid = glCreateShader(GL_FRAGMENT_SHADER);

	unsigned char linebounce = 0;
	unsigned char arraysize = shader->numdefines + 3;
	//todo errorcheck

	char ** shaderstring = malloc((arraysize) * sizeof(char *)); // if it has no defines, it will be 1 so its ok anyway
	memset(shaderstring, 0, (arraysize) * sizeof(char *));
	int i = 0;
	if(shader->type & 1){
		for(i = 0; i < shader->numdefines; i++){
			if(permutation & 1<<i){
				GLuint l = strlen(shader->defines[i]) + 10;
				shaderstring[i] = malloc(l); // 8 for the extra #define , 1 for the \n, 1 for the \0
				memset(shaderstring[i], 0, l);
				snprintf(shaderstring[i], l, "#define %s", shader->defines[i]); //no need for the \n because the defines has it in them
				linebounce++;
			} else {
				shaderstring[i] = malloc(2* sizeof(char));
				memset(shaderstring[i], 0, 2);
				strcpy(shaderstring[i], "\0");
			}
		}
	}
		if(permutation & 1<<30){
			shaderstring[i] = malloc(20); // 8 for the extra #define , 1 for the \n, 1 for the \0
			memset(shaderstring[i], 0, 20);
			snprintf(shaderstring[i], 20, "#define INSTANCED\n");
			linebounce++;
		} else {
			shaderstring[i] = malloc(2* sizeof(char));
			memset(shaderstring[i], 0, 2);
			strcpy(shaderstring[i], "\0");
		}
		i++;
		if(permutation & 1<<31){
			shaderstring[i] = malloc(18); // 8 for the extra #define , 1 for the \n, 1 for the \0
			memset(shaderstring[i], 0, 18);
			snprintf(shaderstring[i], 18, "#define UNUSED\n");
			linebounce++;
		} else {
			shaderstring[i] = malloc(2* sizeof(char));
			memset(shaderstring[i], 0, 2);
			strcpy(shaderstring[i], "\0");
		}
		i++;

	shaderstring[arraysize-1] = shader->vertstring;
	glShaderSource(vertid, arraysize, (const GLchar **) shaderstring, 0);
	shaderstring[arraysize-1] = shader->fragstring;
	glShaderSource(fragid, arraysize, (const GLchar **) shaderstring, 0);
	//if i set shadersource length to null, it does it by null char looking
	//todo geom shader

	if(shader->type & 1){
//		int i;
		for(i = 0; i < shader->numdefines+2; i++) if(shaderstring[i]) free(shaderstring[i]); // doesnt free the vertstring or fragstring
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
	glBindAttribLocation(programid, TANGENTATTRIBLOC, "tangentattrib");
	glBindAttribLocation(programid, BLENDIATTRIBLOC, "blendiattrib");
	glBindAttribLocation(programid, BLENDWATTRIBLOC, "blendwattrib");
	glBindAttribLocation(programid, INSTANCEATTRIBLOC, "instanceattrib");



	glLinkProgram(programid);
	perm.id = programid;




	glDeleteShader(vertid);
	glDeleteShader(fragid); //get rid of unused shader stuffs

	//TODO errorcheck
	glGetProgramiv(programid, GL_LINK_STATUS, &status);
	if(status == GL_FALSE){
//			consolePrintf("Shader %s compile failed\n", shader->name);
			fail = 1;
	}

	if(printProgramLogStatus(programid) || fail){
		fail = TRUE;
//		if(shader->type & 1){
			char * error = malloc((100*linebounce) + 100);
			sprintf(error, "Shader %s compile failed. Permutations:\n", shader->name);
			int i;
			for(i = 0; i < shader->numdefines; i++){
				if(permutation & 1<<i){
					sprintf(error, "%s%s", error, shader->defines[i]);
				}
			}
			if(permutation & 1<<30){
				sprintf(error, "%s%s", error, "INSTANCED\n");
			}
			if(permutation & 1<<31){
				sprintf(error, "%s%s", error, "UNUSED\n");
			}
			sprintf(error, "%sLine offset in file: %i\n", error, linebounce);
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
//	int i;
	for(i = 0; i < 16; i++){
		sprintf(texstring, "texture%i", i);
		perm.texturespos[i] = glGetUniformLocation(programid, texstring);
		glUniform1i(perm.texturespos[i], i);
//		consolePrintf("texture space %i at uniform loc %i\n", i, perm->texturespos[i]);
	}
	free(texstring);

//set up other uniforms
	perm.unimat40 = glGetUniformLocation(programid, "unimat40");
	perm.unimat41 = glGetUniformLocation(programid, "unimat41");
	perm.univec40 = glGetUniformLocation(programid, "univec40");
	perm.univec30 = glGetUniformLocation(programid, "univec30");
	perm.univec20 = glGetUniformLocation(programid, "univec20");
	perm.uniint0 = glGetUniformLocation(programid, "uniint0");
	perm.uniscreensizefix = glGetUniformLocation(programid, "uniscreensizefix");
	perm.uniscreentodepth = glGetUniformLocation(programid, "uniscreentodepth");
	perm.unifloat0 =glGetUniformLocation(programid, "unifloat0");
//uniform blocks
	perm.uniblock0 = glGetUniformBlockIndex(programid, "uniblock0"); //todo maybe i dont need to keep this around
	if(perm.uniblock0 > -1) glUniformBlockBinding(programid, perm.uniblock0, 0);	//todo make a ubo manager that i can just slap data in and out of, have each shader bind to this
	perm.uniblock1 = glGetUniformBlockIndex(programid, "uniblock1"); //todo maybe i dont need to keep this around
	if(perm.uniblock1 > -1) glUniformBlockBinding(programid, perm.uniblock1, 1);	//todo make a ubo manager that i can just slap data in and out of, have each shader bind to this
	//it will be very useful for some stuff i guess

	perm.compiled = 2;
	consolePrintf("Shader %s compile successful\n", shader->name);


/*
	char * error = malloc((100* shader->numdefines) + 100);
	sprintf(error, "Shader %s compile success. Permutations:\n", shader->name);
	if(shader->defines){
		for(i = 0; i < shader->numdefines; i++){
			if(permutation & 1<<i){
				sprintf(error, "%s%s", error, shader->defines[i]);
			}
		}
	}
	consoleNPrintf(strlen(error)+1,error);
	free(error);
*/

	return perm;
}


int readyShader(shaderprogram_t * shader){
	char * name = shader->name;
	char * definename = malloc(strlen(name)+8);
	sprintf(definename, "%s.define", name);
//	strcpy(definename, name);strcat(definename, ".define");

	FILE *f;
	if(!(f = fopen(definename, "r"))){
		consolePrintf("Count not find .define file for shader %s\n", name);
		shader->defines = 0;
		//todo debug?
	} else {
		shader->defines = malloc(32 * sizeof(char *));
		int i;
		for(i = 0; i < 32; i++){
			shader->defines[i] = malloc(100);
			if(fgets(shader->defines[i], 100, f)){
				shader->defines[i] = realloc(shader->defines[i], strlen(shader->defines[i])+1);
//				consolePrintf("%s\n",shader->defines[i]);
			} else {
//				consolePrintf("%s\n",shader.defines[i]);

				free(shader->defines[i]);
				shader->defines[i] = 0;
				break;
			}
		}
		if(i){
			shader->type = shader->type | 1; //shader has a definelist
			shader->defines = realloc(shader->defines, i * sizeof(char *)); // might need a +1
			shader->numdefines = i;
		} else {
			free(shader->defines);
			shader->defines = 0;
		}
	}
	if(f)fclose(f);
	free(definename);

	char * flagname = malloc(strlen(name)+7);
	sprintf(flagname, "%s.flags", name);
	if(!(f = fopen(flagname, "r"))){
		shader->sysflagssupport = 0;
		consolePrintf("Count not find .flags file for shader %s\n", name);
		//todo debug?
	} else {
		char * dfstring = malloc(30*sizeof(char));
		fgets(dfstring, 30, f);
//		shader.sysflags = atoi(dfstring);
		free(dfstring);
	}
	if(f)fclose(f);
	free(flagname);


	char * vertname = malloc(strlen(name)+6);
	sprintf(vertname, "%s.vert", name);
	loadFileString(vertname, &shader->vertstring, &shader->vertlength, 1);
	free(vertname);
	if(shader->vertlength == 0){
		//todo
		if(shader->vertstring)free(shader->vertstring);
		shader->type = 0;
		return FALSE;
	}
	char * fragname = malloc(strlen(name)+6);
	sprintf(fragname, "%s.frag", name);
//	strcpy(vertname, name);strcat(vertname, ".frag");
	loadFileString(fragname, &shader->fragstring, &shader->fraglength, 1);
	free(fragname);
	if(shader->fraglength == 0){
		//todo
		if(shader->vertstring)free(shader->vertstring);
		shader->vertlength =0;
		if(shader->fragstring)free(shader->fragstring);
		shader->type = 0;
		return FALSE;
	}
	shader->type = shader->type | 2;

	char * geomname = malloc(strlen(name)+6);
	sprintf(geomname, "%s.geom", name);
	loadFileString(geomname, &shader->geomstring, &shader->geomlength, 0);
	free(geomname);
	if(shader->geomlength < 1){
		if(shader->geomstring){
			free(shader->geomstring);
			shader->geomstring = 0;
		}
	} else {
		shader->type = shader->type | 4;
	}
	//todo debugmodes
	return TRUE;
}


shaderprogram_t createAndReadyShader(char * name){
	shaderprogram_t shader;
	memset(&shader, 0, sizeof(shaderprogram_t));
	//todo debugmodes
	shader.name = malloc(strlen(name)+1);
	strcpy(shader.name, name);
	readyShader(&shader);


	return shader;
}

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
shaderpermutation_t * addPermutationToShader(shaderprogram_t * shader, unsigned int permutation){
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

shaderpermutation_t * findShaderPermutation(shaderprogram_t * shader, unsigned int permutation){
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

void shaderUseProgram(GLuint program){
	if(program != currentprogram){
		currentprogram = program;
		glUseProgram(program);
	}
}


int bindShaderPerm(shaderpermutation_t * perm){
	if(!perm) return FALSE;
	if(perm->compiled < 2) return FALSE;
	GLuint program = perm->id;
	if(program != currentprogram){
		shaderCurrentBound = perm;
		currentprogram = program;
		glUseProgram(program);
		return 2;
	}
	return TRUE;
}

int reloadShaderProgram(int id){
	int shaderindex = (id & 0xFFFF);
	shaderprogram_t * shader = &shaderlist[shaderindex];
	if(shader->myid != id) return FALSE;
	if(!shader->name) return FALSE;

	int i;
	shaderpermutation_t *p, *j;
	for(i = 0; i < PERMHASHSIZE; i++){
		int del = 0;
		for (p = &shader->permhashtable[i]; p; p = j){
			j = p->next;
			//delete the program
			if(p->id) glDeleteProgram(p->id);
			//dont try to free the hashtable part, only the Linked list off of it
			if(del){
				memset(p, 0, sizeof(shaderpermutation_t));
				free(p);
			}
			del = 1;
		}
	}
	memset(shader->permhashtable, 0, PERMHASHSIZE * sizeof(shaderpermutation_t));

	if(shader->defines){
		for(i = 0; i < shader->numdefines; i++){
			if(shader->defines[i]) free(shader->defines[i]);
		}
		free(shader->defines); shader->defines = 0;
	}
	if(shader->fragstring) free(shader->fragstring); shader->fragstring = 0;
	if(shader->vertstring) free(shader->vertstring); shader->vertstring = 0;
	if(shader->geomstring) free(shader->geomstring); shader->geomstring = 0;

	shader->type = 0;
	shader->sysflagssupport = 0;
	shader->numdefines = 0;
	shader->fraglength = 0;
	shader->vertlength = 0;
	shader->geomlength = 0;
	//all deleted

	readyShader(shader);
	return TRUE;

}
int reloadAllShaderPrograms(void){
	int count = 0;
	int i;
	for(i = 0; i <= shaderArrayLastTaken; i++){
		if(shaderlist[i].type){
			reloadShaderProgram(shaderlist[i].myid);
			count++;
		}
	}
	return count;
}

