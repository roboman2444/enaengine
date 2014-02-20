#version 150

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projectionMat;
uniform mat4 mvpMat;


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
//	gl_Position = projectionMat *  viewMat * modelMat * vec4(fragposition, 1.0);
	gl_Position = mvpMat * vec4(fragposition, 1.0);
//	gl_Position = vec4(fragposition, 1.0) * mvpMat;
}
