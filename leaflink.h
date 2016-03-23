#ifndef LEAFLINKHEADER
#define LEAFLINKHEADER



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


#endif
