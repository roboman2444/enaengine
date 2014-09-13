#include "globaldefs.h"
#include "hashtables.h"

int getHash(const char * string){
	unsigned long rethash = 0;
	while(*string){
		rethash = rethash * 31 + *string;
		string++;
	}
	return rethash%MAXHASHBUCKETS;
}

int addToHashTable(const char * name, int id, hashbucket_t * ht){
	int hash = getHash(name);
	hashbucket_t * hb = &ht[hash];
	if(hb->id){
		for(; hb->next; hb = hb->next);
		hb->next = malloc(sizeof(hashbucket_t));
		hb = hb->next;
	}
	hb->name = (char *)name;
	hb->id = id;
	hb->next = 0;
	return hash;
}
int deleteFromHashTable(const char * name, int id, hashbucket_t *ht){
	int hash = getHash(name);
	hashbucket_t * hb = &ht[hash];
	if(hb->id == id){ // check first one
		if(hb->next){
			*hb = *hb->next;
			free(hb->next);
		}
		hb->id = 0;
		hb->name = 0;
		return TRUE;
	} //check linked list off of first
	hashbucket_t * oldb = hb;
        for(hb = hb->next; hb; oldb = hb, hb = hb->next){
		if(hb->id == id){
//			if(hb->name) free(hb->name);
			oldb->next = hb->next;
			free(hb);
			return TRUE;
		}
        }
	return FALSE;
}
int findByNameRINT(const char * name, hashbucket_t *ht){
	int hash = getHash(name);
	hashbucket_t * hb = &ht[hash];
	if(!hb->name) return 0;
        for(; hb; hb = hb->next){
		if(strcmp(hb->name, name)==0){
			return hb->id;
		}
        }
	//not found :(
	return 0;
}
