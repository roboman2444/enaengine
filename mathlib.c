#include "globaldefs.h"
#include "mathlib.h"
void getBBoxpFromBBox(vec_t * bbox, vec_t *bboxp){
	int i;
	for(i = 0; i < 8; i++){
		bboxp[(i*3)+0] = bbox[(i&1)+0];
		bboxp[(i*3)+1] = bbox[((i&2)>>1)+2];
		bboxp[(i*3)+2] = bbox[((i&4)>>2)+4];
	}
}
