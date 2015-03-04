#ifndef RENDERMODELHEADER
#define RENDERMODELHEADER


/*
depth-only model drawing
*/
typedef struct renderModelDepthCallbackData_s {
	unsigned int modelid;
	unsigned int shaderid; //do i need this?
	GLuint shaderprogram;
	unsigned int shaderperm; //todo do i need these, or can i change to a pointer?
	unsigned int ubodataoffset;
	matrix4x4_t mvp;
//	matrix4x4_t mv; // unlikely that i need that
} renderModelDepthCallbackData_t; //todo rename

void rendermodel_drawMDCallback(renderlistitem_t * ilist, unsigned int count); // may need to change to void *
void rendermodel_setupMDCallback(renderlistitem_t * ilist, unsigned int count);// may need to change to void *

/*
textured model drawing
*/
typedef struct renderModelCallbackData_s {
	unsigned int modelid;
	unsigned int shaderid; //do i need this?
	GLuint shaderprogram;
	unsigned int shaderperm; //todo do i need these, or can i change to a pointer?
	unsigned int texturegroupid;
	unsigned int ubodataoffset;
	matrix4x4_t mvp;
	matrix4x4_t mv;
} renderModelCallbackData_t; //todo rename

void rendermodel_drawMCallback(renderlistitem_t * ilist, unsigned int count); // may need to change to void *
void rendermodel_setupMCallback(renderlistitem_t * ilist, unsigned int count);// may need to change to void *

/*
alpha blended textured model drawing
*/
typedef struct renderModelAlphaCallbackData_s {
	unsigned int modelid;
	unsigned int shaderid; //do i need this?
	GLuint shaderprogram;
	unsigned int shaderperm; //todo do i need these, or can i change to a pointer?
	unsigned int texturegroupid;
	unsigned int ubodataoffset;
	GLenum blendsource;
	GLenum blenddest;
	matrix4x4_t mvp;
	matrix4x4_t mv;
} renderModelAlphaCallbackData_t; //todo rename

void rendermodel_drawMACallback(renderlistitem_t * ilist, unsigned int count); // may need to change to void *
void rendermodel_setupMACallback(renderlistitem_t * ilist, unsigned int count);// may need to change to void *


/*
generic funcs
*/
void rendermodel_init(void);



#endif
