#ifndef UBOMANAGERHEADER
#define UBOMANAGERHEADER

#define UBOBLOCKLOC 0
typedef struct ubo_s {
	int myid;
	GLuint id;
	unsigned int place;
//	unsigned int oldplace;
	unsigned int size;
	GLubyte *data;
	char type;
} ubo_t;

GLint ubo_maxsize;
GLint ubo_alignment;

int ubo_ok;

ubo_t *ubo_list;
int ubo_count;
int ubo_arraysize;
int ubo_arraylasttaken;

int ubo_init(void);
char ubo_flushData(ubo_t *u);
int ubo_pushData(ubo_t *u, const unsigned int size, const void * data);

unsigned int ubo_halfData(ubo_t * u);
unsigned int ubo_sizeData(ubo_t * u, const unsigned int newsize);
unsigned int ubo_pruneData(ubo_t * u);

ubo_t ubo_create(void);
int ubo_delete(const int id);

void ubo_pruneList(void);

ubo_t * ubo_createAndAddRPOINT(void);
int ubo_createAndAddRINT(void);
ubo_t * ubo_returnById(int id);

#endif
