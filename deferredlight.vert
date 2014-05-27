#version 150
//todo put something in shadermanager to handle this
//something like a define instancesize 16, then have the shadermanager put in a define that does the devide?

uniform mat4 unimat40;


in vec3 posattrib;
//in vec3 normattrib;
//in vec2 tcattrib;
//out vec3 fragnormal;
//in vec4 instanceattrib;
#define N 4096
layout (std140) uniform uniblock0 {
	vec4 ldata[N];
} uniblock0_t;
out vec3 fragposition;
out vec2 fragtexcoord;


void main(){
	vec4 instanceattrib = uniblock0_t.ldata[gl_InstanceID];
//	fragnormal = normattrib;
//	fragtexcoord = tcattrib;
//	fragposition = posattrib;
//	vec3 translated = (posattrib * block0.d[gl_InstanceID].a) + block0.d[gl_InstanceID].rgb;
	vec3 translated = (posattrib * instanceattrib.a) + instanceattrib.rgb;
	gl_Position = unimat40 * vec4(translated, 1.0);
	fragposition = gl_Position.xyz;
//	fragtexcoord = (gl_Position.xy /gl_Position.w );
//	fragtexcoord = vec2(gl_Position);
}
