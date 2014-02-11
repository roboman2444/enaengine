#include "globaldefs.h"
#include "hashtables.h"

int getHash(char * string){
	unsigned long rethash = 0;
	while(*string){
		rethash = rethash * 31 + *string;
		string++;
	}
	return rethash%MAXHASHBUCKETS;
}
