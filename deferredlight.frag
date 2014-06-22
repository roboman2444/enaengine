#version 150

//uniform mat4 unimat40;
//vec2(Far / (Far - Near), Far * Near / (Near - Far));
uniform vec2 uniscreentodepth;
in vec3 mvpos;
#ifdef DIRECTIONAL
in vec3 lightnormal;
#else
	in float lsize;
	in vec3 lpos; //viewspace of light
#endif
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
	pos.xy = mvpos.xy * (pos.z / mvpos.z);

	vec3 eyenormal = -normalize(pos);
	#ifndef DIRECTIONAL
		vec3 lightdelta = lpos-pos;
		float lightdist = length(lightdelta);
		vec3 lightnormal = lightdelta/lightdist;
	#endif
	vec3 surfnormal = normalize(normaldist.rgb);
	vec3 vhalf = normalize(lightnormal+eyenormal);

	vec2 gloss = texture(texture2, tc).rg;
	vec3 diffuse = texture(texture0, tc).rgb;

	//shading
//	float attenuation = 1.0/(1.0 + lsize * 2 *pow(lightdist, 2));
//	float attenuation = clamp(1.0 - lightdist/lsize, 0.0, 1.0); attenuation *= attenuation;

	#ifdef DIRECTIONAL
		float attenuation = 1.0;
	#else
		float attenuation = clamp(1.0 - lightdist*lightdist/(lsize*lsize), 0.0, 1.0); attenuation *= attenuation;
	#endif

	fragColor.rgb = clamp(dot(surfnormal, lightnormal), 0.0, 1.0) * diffuse * attenuation;
	fragColor.a = 1.0;

	fragColor.rgb += vec3(clamp(pow(dot(surfnormal,vhalf), gloss.y), 0.0, 1.0) * attenuation * gloss.x);
//	fragColor.rgb = diffuse;

}
