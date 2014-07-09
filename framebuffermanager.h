#ifndef FRAMEBUFFERMANAGERHEADER
#define FRAMEBUFFERMANAGERHEADER


typedef struct framebuffer_s{
	char * name;
	unsigned char type;
	int myid;
	GLuint width;
	GLuint height;
	texture_t *textures;
	texture_t *multisampletextures;
	GLuint multisampleresolveid;
	GLuint rb;
	GLuint id;
	unsigned char count;
	unsigned char rbflags;
	//rbflags
	//first 3 bits are multisample count (128xmsaa ftw)
	//7th bit is weather or not to have a depth component
	//8th bit is weather or not to have a stencil component
	//if neither 8th or 7th bit is set, assume not renderbuffer, or renderbuffer is handled by a seperate framebuffer
//	int texturegroupid;
} framebuffer_t;

	#define FRAMEBUFFERRBFLAGSMSCOUNT 0b00001111
	//three free spots for flags
	#define FRAMEBUFFERRBFLAGSSTENCIL 0b10000000
	#define FRAMEBUFFERRBFLAGSDEPTH 0b01000000


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

char resolveMultisampleFramebuffer(framebuffer_t *fb);
char resolveMultisampleFramebufferSpecify(framebuffer_t *fb, unsigned int buffer);


void bindFramebuffer(framebuffer_t *fb);
#endif
