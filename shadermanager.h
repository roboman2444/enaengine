#ifndef SHADERMANAGERHEADER
#define SHADERMANAGERHEADER

typedef struct shaderprogram_s
{
	GLuint id;
	char * name;
	GLuint vertexid;
	GLuint fragmentid;
} shaderprogram_t;
int initShaderSystem(void);
int createAndLoadShader(char * name);
int addProgramToList(char * name,GLuint id, GLuint vertexid, GLuint fragmentid);
int printProgramLogStatus(int id);
shaderprogram_t findShader(char * name);
#endif
