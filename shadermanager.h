#ifndef SHADERMANAGERHEADER
#define SHADERMANAGERHEADER

#define PERMHASHSIZE 256


typedef struct shaderpermutation_s {
	struct shaderpermutation_s * next;
	GLuint id;
	char compiled;
	unsigned int permutation;
	int unimat40;
	int unimat41;
	//int unimat42;
	//int unimat43;
	int univec40;
	int univec30;
	int univec20;
	int univec21;
	int uniscreensizefix;
	int uniscreentodepth;
	int uniint0;
	int unifloat0;
	int uniblock0;
	int uniblock1;
	GLint texturespos[16];
} shaderpermutation_t;

typedef struct shaderprogram_s
{
	char * name;
	char type;
	char sysflagssupport; // 1 is instancing, 2 is unused
	int numdefines; // maybe change to unsigned char
	char **defines;
	int myid;
	char * fragstring; // quick if i need to compile a new permutation
	int fraglength;
	char * vertstring; // quick if i need to compile a new permutation
	int vertlength;
	char * geomstring; // quick if i need to compile a new permutation
	int geomlength;
	shaderpermutation_t permhashtable[PERMHASHSIZE];
//	shaderuniform_t * uniforms;
} shaderprogram_t;


//shaderpermutation_t * shaderCurrentBound;

int shader_ok;
//int shader_programnumber;

shaderprogram_t * shader_list;
int shader_count;
int shader_arraySize;
int shader_arrayLastTaken;

int shader_init(void);

shaderprogram_t * shader_findByNameRPOINT(const char * name);
int shader_findByNameRINT(const char * name);

shaderprogram_t * shader_returnById(const int id);

shaderprogram_t * shader_addRPOINT(shaderprogram_t shader);
int shader_addRINT(shaderprogram_t shader);

int shader_deletePermutation(shaderprogram_t * shader, const unsigned int permutation);
int shader_deleteProgram(const int id);
int shader_deleteAllPrograms(void);


int shader_reloadProgram(const int id);
int shader_reloadAllPrograms(void);

shaderprogram_t * shader_createAndAddRPOINT(const char * name);
int shader_createAndAddRINT(const char * name);

void shader_pruneList(void);



int shader_printShaderLogStatus(const int id);
int shader_printProgramLogStatus(const int id);
int shader_getProgramLogStatus(const int id, char ** output);
int shader_getShaderLogStatus(const int id, char ** output);
shaderpermutation_t * shader_findPermutation(shaderprogram_t * shader, unsigned int permutation);
shaderpermutation_t * shader_addPermutationToProgram(shaderprogram_t * shader, unsigned int permutation);


//int shaderUseProgram(const GLuint program);
char shader_bindPerm(shaderpermutation_t * perm);
#endif
