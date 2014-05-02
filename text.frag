#version 150

in vec2 fragposition;
in vec2 fragtexcoord;
uniform sampler2D texture0;

out vec4 fragColor;

//vec2 uniscreensizefix = vec2(1.0/800.0, 1.0/600.0);

void main(){
//		fragColor = vec4(1.0);
//		fragColor = vec4(0.0);
		fragColor = texture2D(texture0, fragtexcoord);
//		fragColor.r = texture2D(texture0, fragtexcoord).b;
//		fragColor = texture2D(texture0, gl_FragCoord.xy * uniscreensizefix);
}
