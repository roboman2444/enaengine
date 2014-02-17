#ifndef SHADERMANAGERHEADER
#define SHADERMANAGERHEADER


typedef struct shaderprogram_s
{
	char * name;
	char type;
	unsigned int id;
	unsigned int vertexid;
	unsigned int fragmentid;
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
shaderprogram_t * defaultShader;

int initShaderSystem(void);
//loads a shader pair from files name.frag and name.vert, compiles, links, adds to the list
shaderprogram_t createAndLoadShader(char * name);
//resizes and adds the shader to the list
shaderprogram_t * addProgramToList(shaderprogram_t prog);
shaderprogram_t * createAndAddShader(char * name);

shaderprogram_t * returnShader(int id);
int printProgramLogStatus(int id);
int getProgramLogStatus(int id, char ** output);
shaderprogram_t * findProgramByName(char * name);
GLint findShaderAttribPos(shaderprogram_t * shader, char * name);
#endif
