#ifndef HASHTABLESHEADER
#define HASHTABLESHEADER

typedef struct hashbucket_s {
	char * name;
	int id;
	struct hashbucket_s * next;
} hashbucket_t;

int getHash(char * string);
int addToHashTable(char * name, int id, hashbucket_t * ht);
int deleteFromHashTable(char * name, int id, hashbucket_t * ht);
int findByNameRINT(char * name, hashbucket_t * ht);
#endif
