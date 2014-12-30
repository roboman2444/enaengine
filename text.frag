in vec2 fragposition;
in vec2 fragtexcoord;
uniform sampler2D texture0;

out vec4 fragColor;

void main(){
		fragColor = vec4(1.0, 1.0, 1.0, texture(texture0, fragtexcoord).a);
//		fragColor = texture(texture0, fragtexcoord);

//		fragColor += vec4(1.0, 0.0, 0.0, 1.0);
}
