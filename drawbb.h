#ifndef DRAWBBHEADER
#define DRAWBBHEADER
	int drawbb_ok;
	int drawbb_init(void);
	unsigned int drawbb_addToRenderQueue(viewport_t *v, renderqueue_t *q, const vec_t *bboxp);

#endif
