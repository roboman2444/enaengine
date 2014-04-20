#version 150

in vec2 fragposition;
in vec2 fragtexcoord;
uniform sampler2D texture0;

out vec4 fragColor;

void main(){
		fragColor = vec4(1.0);
//		fragColor += texture2D(texture0, fragtexcoord);
}
