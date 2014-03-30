#ifndef FRAMEBUFFERMANAGERHEADER
#define FRAMEBUFFERMANAGERHEADER

typedef struct framebuffer_s
{
	char * name;
	char type;
	GLuint myid;
	GLuint width;
	GLuint height;
	int texturegroupid;
} framebuffer_t;

typedef struct framebufferlistpoint_s {
	//gotta free dis stuff after you use it
	framebuffer_t **list;
	int count;
} framebufferlistpoint_t;
typedef struct framebufferlistint_s {
	//gotta free dis stuff after you use it
	int *list;
	int count;
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

framebuffer_t * createAndAddFramebufferRPOINT(char * name, char type);
int createAndAddFramebufferRINT(char * name, char type);

framebuffer_t createFramebuffer(char * name, char type);
#endif
