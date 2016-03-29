#ifndef LEAFLINKHEADER
#define LEAFLINKHEADER




//CHANGE
typedef struct leaflinkitem_s {
	int self; //in indice if worldobject, in ID if other (to avoid multiple memory fetches)
	char  type;
	vec2_t bb;
	int unext; //in indice if worldobject, in ID if other (to avoid multiple memory fetches)
	char unextt;
	int uprev;
	char uprevt;
	int dnext;
	char dnextt;
	int dprev;
	char dprevt;
} leaflinkitem_t;


typedef struct leafarrlist_s {
	int * list; //indice into lists in tree
	char * type;
	size_t listsize;
	size_t listplace;
} leafarrlist_t;

	//maybe also try b tree, avl tree, or similar


#endif
