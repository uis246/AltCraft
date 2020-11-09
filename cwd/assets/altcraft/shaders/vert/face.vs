#version 330 core

uniform float GlobalTime;
uniform float DayTime;
uniform float MinLightLevel;
uniform mat4 projView;

//Section offset
//section.xyz*16
uniform vec3 sectionOffset;

//Texture pos format
//xxxx yyyy wwww hhhh
uniform samplerBuffer texturePos;//Texture 2
//Next texture pos format
//xxyy wwhh lfpp
//Or xx yy ww hh

//xxxx yyyy zzzz
uniform samplerBuffer pos;//Tex 3
//3*4*4=12*4=48

//Per quad info
//RR GG BB AA
//Lp tt lf uv
//L - Light (block<<4|sky)
//p - block horisontal pos (z<<4|x)
//tt- texture id
//l - layer
//f - frames
//u - uv start
//v - uv end
//(x<<4)|y
//Todo quad format
//Lp tt uv UV
//uv - uv start
//UV - uv end
uniform usamplerBuffer quadInfo;//Tex 4
//4*2*1=6
//48+8=56

//Indexed by horisontal position
//(z<<4|x)
// uniform usamplerBuffer biomes;

out vec2 UvPosition;

out VS_OUT {
	flat uint Layer;
	flat float Light;
	flat vec3 Color;
} vs_out;

void main()
{
	uint d = uint(gl_VertexID)/uint(3);
	uint quad = d>>1;
	uint m = d&uint(1);
	//If or sh+or
	uint vert = (uint(gl_VertexID)%uint(3)) ^ (m<<1 | m);
	vec2 mul = vec2(vert&uint(1), vert>>uint(1));//CW front
	gl_Position = projView * vec4(texelFetch(pos, int(vert+(quad*uint(4)))).rgb+sectionOffset, 1.0);

	uvec4 qinfo = texelFetch(quadInfo, int(quad)).rgba;
	//TODO: UV
// 	vec4 uv=vec4(uvec4(qinfo.a>>uint(12), qinfo.a>>uint(8), qinfo.a>>uint(4), (qinfo.a))&uint(0xF))/16.0;

	vec4 tex = texelFetch(texturePos, int(qinfo.g));
	float frames = float(qinfo.b&uint(0xFF));
	float frameHeight = tex.w / frames;
	float currentFrame = mod(GlobalTime * 4.0f, frames);
	currentFrame = trunc(currentFrame);
	tex.w = frameHeight;
	tex.y = tex.y + currentFrame * frameHeight;

// 	tex.xy += uv.xy * tex.zw;
// 	tex.zw = (uv.zw-uv.xy) * tex.zw;

	float blockLight = float((qinfo.r>>uint(12) & uint(0xF))) / 15.0;
	float skyLight = (float((qinfo.r>>uint(8)) & uint(0xF)) / 15.0) * DayTime;

	UvPosition = tex.xy + tex.zw*mul;
	vs_out.Light = clamp(blockLight + skyLight, MinLightLevel, 1.0);
	vs_out.Layer = qinfo.b >> uint(8);
// 	vs_out.biome = float(texelFetch(biomes, int(qinfo>>uint(16))&0xF).r) / 256.0;
}

//Per quad format
//Lptt
//L - Light
//p - block pos
//tt- texture id

//Quad pos in texture:
//(12*4)+(4*2)=48+8=56
