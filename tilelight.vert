//todo put something in shadermanager to handle this
//something like a define instancesize 16, then have the shadermanager put in a define that does the devide?

uniform mat4 unimat40; //mvp
uniform mat4 unimat41; // mv

uniform vec2 univec20; //size of tile

//uniform vec2 uniscreentodepth;


in vec3 posattrib;
flat out unsigned int count;
struct lightdata {
	vec3 pos;
	float size;
};
struct tiledata {
	vec2 offset;
	unsigned int lcount;
	lightdata ldata[8];
};

#define N 256
layout (std140) uniform uniblock0 {
	tiledata tdata[N]; //todo
} uniblock0_t;


void main(){
	tiledata instanceattrib = uniblock0_t.tdata[gl_InstanceID];
//	vec2 scale = 1.0/univec20;
//	vec2 offset = instanceattrib.offset * scale;
//	gl_Position.xy = (((posattrib.xy * scale) + offset)*2.0) -1.0;
	gl_Position.xy = (posattrib.xy * univec20)+instanceattrib.offset;
	count = instanceattrib.lcount;
//	gl_Position.syz = posattrib.xyz;
//	gl_Position.yw =vec2(0.0);
}
