#version 330 core
precision mediump float;
precision mediump int;

uniform float GlobalTime;
uniform float DayTime;
uniform float MinLightLevel;
uniform mat4 projView;

uniform vec3 tint[256*256];

//Per quad info

//xx yy ww hh 4*2=8
//b h l f 4*1=4 //12
//T(L1){Uu} (L2){Vv} 2*2=4 //16

layout(location = 0) in uvec2 qinfo;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 positions[4];
//3
//4
//5
layout(location = 6) in uvec4 utex;
layout(location = 7) in uvec4 bhlf;//biome height layer frames

out vec2 UvPosition;
flat out uint Layer;

out VS_OUT {
	flat float Light;
	flat vec3 Color;
} vs_out;

//Intel SNB: VS vec4 shader: 56 instructions. 0 loops. 186 cycles. 0:0 spills:fills, 1 sends. Compacted 896 to 864 bytes (4%)

void main() {
	gl_Position = projView * vec4(positions[gl_VertexID], 1.0);

	vec4 tex = vec4(utex) * 0.0009765625 /* /1024.0 */;
	float frames = float(bhlf.w);
	tex.w /= frames;
	tex.y += trunc(mod(GlobalTime * 4.0f, frames)) * tex.w;

	vec4 subUV = vec4(
		uvec4(qinfo, qinfo >> uint(5)) & uint(0x1F)
		) * 0.0625 /* /16.0 */;

	UvPosition = tex.xy + tex.zw*(subUV.xy + uv*(subUV.zw-subUV.xy));
	Layer = bhlf.z;

	vec2 light = vec2((qinfo >> uint(10)) & uint(0xF)) * (1 / 15.0);
	vs_out.Light = clamp(length(vec2(light.x, (light.y * DayTime))), MinLightLevel, 1.0);
	vs_out.Color = vec3(1) + ((vec3(0.514, 0.710, 0.576)) - vec3(1))*(qinfo.x>>14);
}
