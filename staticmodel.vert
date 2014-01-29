#version 150

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projectionMat;

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
	gl_Position = modelMat *viewMat * projectionMat * vec4(fragposition, 1.0);
//	gl_PointSize = 5.0;
}
