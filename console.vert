#version 150

in vec2 texCoord;
in vec2 position;
vec3 newpos;


void main(){
	gl_Position = vec4(position, 1.0, 1.0);
}
