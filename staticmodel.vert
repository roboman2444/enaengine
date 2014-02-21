#version 150

uniform mat4 unimat40;


in vec3 posattrib;
in vec3 normattrib;
in vec2 tcattrib;
out vec3 fragnormal;
out vec3 fragposition;
out vec2 fragtexCoord;


void main(){
	fragnormal = normattrib;
	fragposition = posattrib;
	fragtexCoord = tcattrib;
	gl_Position = unimat40 * vec4(fragposition, 1.0);
}
