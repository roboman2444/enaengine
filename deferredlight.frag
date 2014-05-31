#version 150

//uniform mat4 unimat40;
//vec2(Far / (Far - Near), Far * Near / (Near - Far));
uniform vec2 uniscreentodepth;

in vec3 lpos;
in vec3 mvpos;
//in vec2 fragtexcoord;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

uniform vec2 uniscreensizefix;

out vec4 fragColor;

void main(){
		vec2 tc = gl_FragCoord.xy * uniscreensizefix;
		vec3 pos;
		vec4 normaldist = texture2D(texture0, tc);
//		pos.z = normaldist.a;
		pos.z = uniscreentodepth.y / normaldist.a + uniscreentodepth.x;
		pos.xy = mvpos.xy * (pos.z / mvpos.z);

		vec3 eyenormal = -normalize(pos);
		vec3 lightnormal = normalize(lpos - pos);
		vec3 surfnormal = normaldist.rgb;

		float diffuse = clamp(dot(surfnormal, lightnormal * 2.0), 0.0, 1.0);
//		fragColor = vec4(0.01)+diffuse*texture2D(texture0, tc);
		fragColor = texture2D(texture1, tc);
//		fragColor = vec4(0.1);
//		fragColor += texture2D(texture0, tc);

}
