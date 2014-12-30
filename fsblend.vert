in vec3 posattrib;
in vec2 tcattrib;
out vec2 fragposition;
out vec2 fragtexcoord;


void main(){
	gl_Position = vec4(posattrib, 1.0);
	fragposition = posattrib.xy;
	fragtexcoord = tcattrib;
}
