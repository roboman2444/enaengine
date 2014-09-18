#ifndef CVARHEADER
#define CVARHEDER

struct cvar_s;
typedef void (* cvarcallback_t)(struct cvar_s * c);

typedef struct cvar_s {
	char * name;
	int myid;
	char * helptext;

	int valueint;
	float valuefloat;
	float valuevector[3]; //todo write something for this...
	char * valuestring; //ALWAYS freeable
	unsigned int valuestringlength;


	cvarcallback_t onchange;

//	int defaultint; //todo
//	float defaultfloat;
//	float defaultvector[3];
//	char * defaultstring;
//	char isdefault;

	char type;
} cvar_t;

#define CVAR_REGISTERED 1
#define CVAR_FREEABLE 2


cvar_t **cvar_list;
int cvar_ok;
int cvar_count;
int cvar_arraysize;
int cvar_arraylasttaken;


int cvar_init(void);

cvar_t * cvar_returnById(const int id);
cvar_t * cvar_findByNameRPOINT(char * name);
int cvar_findByNameRINT(char * name);

int cvar_register(cvar_t *c);

int cvar_unregister(const int id);

void cvar_pset(cvar_t * c, const char *value);
void cvar_idset(const int id, const char *value);
void cvar_nameset(char * name, const char *value);

#endif
