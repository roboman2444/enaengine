#ifndef FILEMANAGERHEADER
#define FILENANAGERHEADER

int loadFileString(const char * filename, char ** output, int * length, int debugmode);
int loadFileStringNoLength(const char *filename, char ** output, int debugmode);

#endif
