#version 150

uniform mat4 unimat40;


in vec3 posattrib;
in vec3 normattrib;
in vec2 tcattrib;
//in mat4 instanceattrib;

#define N 1024 //16 floats

layout (std140) uniform uniblock0 {
	mat4 ldata[N];
} uniblock0_t;
layout (std140) uniform uniblock1 {
	mat3x4 ldata[N];
} uniblock1_t;
out vec3 fragnormal;
out vec3 fragposition;
out vec2 fragtexCoord;


void main(){
	mat4 instanceattrib = uniblock0_t.ldata[gl_InstanceID];
	fragnormal = normattrib;
	fragtexCoord = tcattrib;
	fragposition = posattrib;
	gl_Position = instanceattrib * vec4(posattrib, 1.0);
}
