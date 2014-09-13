#include "globaldefs.h"
#include <string.h>

#include "hashtables.h"
#include "cvarmanager.h"

int cvar_ok = 0;
int cvar_count = 0;
int cvar_arraysize = 0;
int cvar_arraylasttaken = 0;
int cvar_arrayfirstopen = 0;

hashbucket_t cvarhashtable[MAXHASHBUCKETS];

cvar_t **cvar_list;

int cvar_init(void){

	memset(cvarhashtable, 0, MAXHASHBUCKETS * sizeof(hashbucket_t));
	if(cvar_list) free(cvar_list); //todo this might require a special cleanup case
	cvar_list = 0;
	cvar_ok = TRUE;
	return TRUE;
}
cvar_t * cvar_findByNameRPOINT(char * name){
	return cvar_returnById(findByNameRINT(name, cvarhashtable));
}
int cvar_findByNameRINT(char * name){
	return findByNameRINT(name, cvarhashtable);
}

cvar_t * cvar_returnById(const int id){
	int index = (id & 0xFFFF);
	cvar_t * c = cvar_list[index]; //note the absence of &cvar_list[index];
	if(!c) return FALSE;
	if(c->myid == id) return c;
	return FALSE;
}


int cvar_register(cvar_t *c){
	cvar_count++;
	for(; cvar_arrayfirstopen < cvar_arraysize && cvar_list[cvar_arrayfirstopen]; cvar_arrayfirstopen++); //note absence of .type
	if(cvar_arrayfirstopen == cvar_arraysize){ //resize
		cvar_arraysize++;
		cvar_list = realloc(cvar_list, cvar_arraysize * sizeof(cvar_t));
	}
	cvar_list[cvar_arrayfirstopen] = c;
	int returnid = (cvar_count << 16) | cvar_arrayfirstopen;
	c->myid = returnid;

	addToHashTable(c->name, returnid, cvarhashtable);
	if(cvar_arraylasttaken < cvar_arrayfirstopen) cvar_arraylasttaken = cvar_arrayfirstopen; //todo redo
	return returnid;
}
int cvar_delete(const int id){
	int index = (id & 0xFFFF);
	cvar_t *c = cvar_list[index];
	if(!c) return FALSE;
	if(c->myid != id) return FALSE;
	if(!c->name) return FALSE;
	char flags = c->type;
	if(!(flags & CVAR_REGISTERED)) return FALSE;
	if(c->valuestring)free(c->valuestring);
	deleteFromHashTable(c->name, id, cvarhashtable);

	c->type = c->type & (~CVAR_REGISTERED);
	if(flags & CVAR_FREEABLE){
		if(c->name)free(c->name);
		if(c->helptext)free(c->helptext);
		free(c);
	}
	cvar_list[index] = 0;
	if(index < cvar_arrayfirstopen) cvar_arrayfirstopen = index;
	for(; cvar_arraylasttaken > 0 && !cvar_list[cvar_arraylasttaken]; cvar_arraylasttaken--);
	return TRUE;
}
