#version 150

in vec2 texCoord;
in vec3 position;
in vec3 normal;
vec3 newpos;


void main(){
	newpos = position*1000.0;
	newpos.z = -0.5;
	gl_Position = vec4(newpos, 1.0);
}
