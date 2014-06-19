#version 150

//uniform mat4 unimat40;
//vec2(Far / (Far - Near), Far * Near / (Near - Far));
uniform vec2 uniscreentodepth;

in vec3 lpos; // worldspace of light
in vec3 mvpos;
in float lsize;
//in vec2 fragtexcoord;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

uniform vec2 uniscreensizefix;

out vec4 fragColor;

void main(){
		//calculate viewspace pixel position
		vec2 tc = gl_FragCoord.xy * uniscreensizefix;
		vec3 pos;
		//get the geometry information (depth, normal)
		vec4 normaldist = texture(texture1, tc);
		pos.z = normaldist.a;
		//pos.z = uniscreentodepth.y / normaldist.a + uniscreentodepth.x;
		pos.xy = mvpos.xy * (pos.z / mvpos.z);

		vec3 eyenormal = -normalize(pos);
		vec3 lightdelta = lpos-pos;
		float lightdist = length(lightdelta);
		vec3 lightnormal = normalize(lightdelta);
		vec3 surfnormal = normaldist.rgb;


		vec2 gloss = texture(texture2, tc).rg;
		vec3 diffuse = texture(texture0, tc).rgb;
//	float attenuation = 1.0/(1.0 + lsize * 2 *pow(lightdist, 2));
//	float attenuation = clamp(1.0 - lightdist/lsize, 0.0, 1.0); attenuation *= attenuation;
	float attenuation = clamp(1.0 - lightdist*lightdist/(lsize*lsize), 0.0, 1.0); attenuation *= attenuation;
//	attenuation *= 50.0;

	fragColor.rgb = clamp(dot(surfnormal, lightnormal), 0.0, 1.0) * diffuse *attenuation;
	fragColor.a = 1.0;
//	if(length(pos.xy) < 1)  fragColor.rgb += vec3(0.1);


}
