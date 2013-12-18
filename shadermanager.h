#ifndef SHADERMANAGERHEADER
#define SHADERMANAGERHEADER


typedef struct shaderuniform_s
{
	char * name;
	GLuint position;
	//todo maybe type and other stuffs?
} shaderuniform_t;
typedef struct shaderprogram_s
{
	GLuint id;
	char * name;
	GLuint vertexid;
	GLuint fragmentid;
	shaderuniform_t * uniforms;
} shaderprogram_t;
int initShaderSystem(void);
//loads a shader pair from files name.frag and name.vert, compiles, links, adds to the list
int createAndLoadShader(char * name);
//resizes and adds the shader to the list
int addProgramToList(char * name,GLuint id, GLuint vertexid, GLuint fragmentid);

shaderprogram_t * returnShader(int id);
int printProgramLogStatus(int id);
int getProgramLogStatus(int id, char ** output);
shaderprogram_t * findProgramByName(char * name);
#endif
