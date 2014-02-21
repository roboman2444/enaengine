#version 150

uniform mat4 unimat40;


in vec2 texCoord;
in vec3 position;
in vec3 normal;
out vec3 fragnormal;
out vec3 fragposition;
out vec2 fragtexCoord;


void main(){
	fragnormal = normal;
	fragposition = position;
	fragtexCoord = texCoord;
//	gl_Position = projectionMat *  viewMat * modelMat * vec4(fragposition, 1.0);
	gl_Position = mvpMat * vec4(fragposition, 1.0);
//	gl_Position = vec4(fragposition, 1.0) * mvpMat;
}
