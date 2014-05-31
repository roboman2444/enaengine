#ifndef SHADERMANAGERHEADER
#define SHADERMANAGERHEADER

#define PERMHASHSIZE 256


typedef struct shaderpermutation_s {
	struct shaderpermutation_s * next;
	GLuint id;
	char compiled;
	int permutation;
	int unimat40;
	int unimat41;
	//int unimat42;
	//int unimat43;
	int univec40;
	int univec30;
	int univec20;
	int uniscreensizefix;
	int uniscreentodepth;
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


shaderpermutation_t * shaderCurrentBound;

int shadersOK;
int programnumber;

shaderprogram_t * shaderlist;
int shadercount;
int shaderArraySize;
int shaderArrayLastTaken;

int initShaderSystem(void);

shaderprogram_t * findShaderByNameRPOINT(char * name);
int findShaderByNameRINT(char * name);

shaderprogram_t * returnShaderById(int id);

shaderprogram_t * addShaderRPOINT(shaderprogram_t shader);
int addShaderRINT(shaderprogram_t shader);

int deleteShader(int id);

shaderprogram_t * createAndAddShaderRPOINT(char * name);
int createAndAddShaderRINT(char * name);

int printProgramLogStatus(int id);
int getProgramLogStatus(int id, char ** output);
shaderpermutation_t * findShaderPermutation(shaderprogram_t * shader, int permutation);
shaderpermutation_t * addPermutationToShader(shaderprogram_t * shader, int permutation);


int bindShaderPerm(shaderpermutation_t * perm);
#endif
