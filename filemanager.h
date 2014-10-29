#ifndef FILEMANAGERHEADER
#define FILENANAGERHEADER

int file_loadString(const char * filename, char ** output, int * length, const int debugmode);
int file_loadStringNoLength(const char *filename, char ** output, const int debugmode);

#endif
