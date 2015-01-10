struct lightdata {
	vec4 pos;
};
struct tiledata {
	vec2 offset;
	int lcount;
	int other;
	lightdata ldata[8];
};

uniform mat4 unimat40; //mvp
uniform mat4 unimat41; // mv

uniform vec2 univec20; //size of tile
uniform vec2 univec21; //for fixing viewstuff
uniform vec2 uniscreentodepth;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;


//flat in tiledata ddata;
in vec2 screenpos;
in vec2 tc;
flat in int instid;


#define N 256
layout (std140) uniform uniblock0 {
	tiledata tdata[N]; //todo
} uniblock0_t;


out vec4 fragColor;

void main(){
//	fragColor = vec4(abs(ddata.offset), ddata.lcount, 1.0f)) * 0.1;
	//calculate viewspace pixel pos
	tiledata ddata = uniblock0_t.tdata[instid];
	vec4 normaldist = texture(texture1, tc);
	float gloss = normaldist.a;

	vec3 pos;
	pos.z = normaldist.b;
	pos.xy = -screenpos * (pos.z * univec21);
	vec3 eyenormal = -normalize(pos);
	vec3 surfnormal = vec3(normaldist.rg, sqrt(1.0f-dot(normaldist.rg, normaldist.rg)));
	vec3 diffout = vec3(0.0);
	vec3 specout = vec3(0.0);
	int j = 0;
	for(int i = 0; i < ddata.lcount; i++){
		vec3 lightdelta = ddata.ldata[i].pos.rgb - pos;
		float lightdist = length(lightdelta);
		if(lightdist > ddata.ldata[i].pos.a) continue;
		j++;
		vec3 lightnormal = lightdelta / lightdist;
		vec3 vhalf = normalize(lightnormal + eyenormal);
		float attenuation = clamp(1.0f - lightdist * lightdist /(ddata.ldata[i].pos.a * ddata.ldata[i].pos.a), 0.0f, 1.0f); attenuation *= attenuation;

//		fragColor.rgb += ((clamp(dot(surfnormal, lightnormal), 0.0f, 1.0f) * diffuse)
//		 + vec3(clamp(pow(dot(surfnormal,vhalf), gloss.y), 0.0f, 1.0f) * gloss.x) )* attenuation;
		diffout += clamp(dot(surfnormal, lightnormal), 0.0f, 1.0f) * attenuation;
		specout += clamp(pow(dot(surfnormal,vhalf), gloss), 0.0f, 1.0f) * attenuation;

//		fragColor.xyz = diffuse;
//		fragColor.xyz = ddata.ldata[i].pos/60.0;

	}
	if(j == 0) discard;
	vec4 difftex = texture(texture0, tc);
	vec3 diffuse = difftex.rgb;
	float specmap = difftex.a;
	fragColor.rgb = (specout * specmap) + (diffout * diffuse);
	fragColor.a = 0.0;
}
