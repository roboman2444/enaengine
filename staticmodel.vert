#version 150

uniform mat4 unimat40;


in vec3 posattrib;
in vec3 normattrib;
in vec2 tcattrib;
in mat4 instanceattrib;
out vec3 fragnormal;
out vec3 fragposition;
out vec2 fragtexCoord;


void main(){
	fragnormal = normattrib;
	fragtexCoord = tcattrib;
	fragposition = posattrib;
	gl_Position = instanceattrib * vec4(posattrib, 1.0);
}
