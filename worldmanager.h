#ifndef WORLDMANAGERHEADER
#define WORLDMANAGERHEADER

// 2^14
#define WORLDMAXSIZE 16384

//works out to be about smallest quadtree size is 64 meters by 64meters
#define WORLDTREEDEPTH 8

typedef struct worldleaf_s{
	float height;
	float negheight;
	vec2_t center;
	struct worldleaf_s *ne;
	struct worldleaf_s *se;
	struct worldleaf_s *sw;
	struct worldleaf_s *nw;
	int treedepth;
	vec6_t bbox;
	//todo actual list of shit and a vao
} worldleaf_t;

int worldOK;

#endif
