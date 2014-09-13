#ifndef HASHTABLESHEADER
#define HASHTABLESHEADER

typedef struct hashbucket_s {
	char * name;
	int id;
	struct hashbucket_s * next;
} hashbucket_t;

int getHash(const char * string);
int addToHashTable(const char * name, int id, hashbucket_t * ht);
int deleteFromHashTable(const char * name, int id, hashbucket_t * ht);
int findByNameRINT(const char * name, hashbucket_t * ht);
#endif
