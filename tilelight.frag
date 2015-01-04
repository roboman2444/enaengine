uniform vec2 uniscreentodepth;

out vec4 fragColor;
flat in unsigned int count;

void main(){
	fragColor = (vec4(0.1) * count) + vec4(0.1, 0.0, 0.0, 0.0);
}
