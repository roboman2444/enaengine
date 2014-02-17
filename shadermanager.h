#ifndef SHADERMANAGERHEADER
#define SHADERMANAGERHEADER


typedef struct shaderprogram_s
{
	char * name;
	char type;
	int myid;
	char * fragstring; // quick if i need to compile a new permutation
	int fraglength;
	char * vertstring; // quick if i need to compile a new permutation
	int vertlength;
	char * geomstring; // quick if i need to compile a new permutation
	int geomlength;
//	shaderuniform_t * uniforms;
} shaderprogram_t;

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

shaderprogrm_t * addShaderRPOINT(shaderprogram_t shader);
int addShaderRINT(shaderprogram_t shader);

int deleteShader(int id);

shaderprogram_t * createAndAddShaderRPOINT(char * name);
int createAndAddShaderRINT(char * name);

int printProgramLogStatus(int id);
int getProgramLogStatus(int id, char ** output);
#endif
