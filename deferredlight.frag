#version 150

//uniform mat4 unimat40;

in vec3 fragposition;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

out vec4 fragColor;

void main(){
		fragColor = vec4(1.0, 0.7, 0.5, 1.0);
//		fragColor = vec4(fragposition, 1.0);
//		fragColor = texture2D(texture0, fragposition.xy);
}
