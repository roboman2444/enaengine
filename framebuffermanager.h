#ifndef FRAMEBUFFERMANAGERHEADER
#define FRAMEBUFFERMANAGERHEADER


typedef struct framebuffer_s{
	char * name;
	unsigned char type;
	int myid;
	GLuint width;
	GLuint height;
	texture_t *textures;
	GLuint rb;
	GLuint id;
	unsigned char count;
	unsigned char rbflags;
//	int texturegroupid;
} framebuffer_t;


typedef struct framebufferlistpoint_s {
	//gotta free dis stuff after you use it
	framebuffer_t **list;
	unsigned int count;
} framebufferlistpoint_t;
typedef struct framebufferlistint_s {
	//gotta free dis stuff after you use it
	int *list;
	unsigned int count;
} framebufferlistint_t;

int initFramebufferSystem(void);

framebuffer_t * framebufferlist;
int vpnumber;
int framebuffersOK;
int framebuffercount;
int framebufferArraySize;
int framebufferArrayLastTaken;

framebuffer_t * findFramebufferByNameRPOINT(char * name);
int findFramebufferByNameRINT(char * name);

framebufferlistpoint_t findFramebuffersByNameRPOINT(char * name);
framebufferlistint_t findFramebuffersByNameRINT(char * name);

framebuffer_t * returnFramebufferById(int id);

framebuffer_t * createAndAddFramebufferRPOINT(char * name, unsigned char count, unsigned char rbflags, unsigned char * perflags);
int createAndAddFramebufferRINT(char * name, unsigned char count, unsigned char rbflags, unsigned char *perflags);

framebuffer_t createFramebuffer(char * name, unsigned char count, unsigned char rbflags, unsigned char *perflags);
int resizeFramebuffer(framebuffer_t *fb, int width, int height);
#endif
