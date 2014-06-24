#ifndef GLMANAGERHEADER
#define GLMANAGERHEADER


int glInit(void);
int glMainDraw(void);
int glResizeViewports(int width, int height);

unsigned int screenHeight;
unsigned int screenWidth;
unsigned int maxMSAASamples;

#endif
