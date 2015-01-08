//todo put something in shadermanager to handle this
//something like a define instancesize 16, then have the shadermanager put in a define that does the devide?

struct lightdata {
	vec3 pos;
	float size;
};
struct tiledata {
	vec2 offset;
	int lcount;
	lightdata ldata[8];
};

#define N 256
layout (std140) uniform uniblock0 {
	tiledata tdata[N]; //todo
} uniblock0_t;

uniform mat4 unimat40; //mvp
uniform mat4 unimat41; // mv

uniform vec2 univec20; //size of tile
uniform vec2 univec21; //for calc worldpos stuff

//uniform vec2 uniscreentodepth;


in vec3 posattrib;
flat out tiledata ddata;

out vec2 screenpos;
out vec2 tc;

void main(){
	tiledata instanceattrib = uniblock0_t.tdata[gl_InstanceID];
	gl_Position.xy = (posattrib.xy * univec20)+instanceattrib.offset;
//	gl_Position.zw = vec2(0.0);
	screenpos = gl_Position.xy;
	tc = (gl_Position.xy +1.0)/2.0;
	ddata = instanceattrib;
}
