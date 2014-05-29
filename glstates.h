#ifndef GLSTATESHEADER
#define GLSTATESHEADER


#define FLAG_BLEND	1
#define FLAG_DEPTHTEST 	2
#define FLAG_DEPTHMASK 	4
#define FLAG_CULLFACE	8
#define FLAG_CULLSIDE	16


char glcurrentstate;
char glcurrentactivetexture; //todo

void glStatesSetFlag(char flag); //best way to do it... all at once

//todo all of these
void glStatesBlend(GLboolean flag);
void glStatesDepthTest(GLboolean flag);
void glStatesDepthMask(GLboolean flag);
void glStatesCullFace(GLboolean flag);
void glStatesCullFaceSide(GLboolean flag);



#endif
