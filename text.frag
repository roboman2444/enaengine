#version 150

in vec2 fragposition;
in vec2 fragtexcoord;
uniform sampler2D texture0;

out vec4 fragColor;

void main(){
		fragColor = texture2D(texture0, fragtexcoord);
//		fragColor = vec4(1.0);
}
