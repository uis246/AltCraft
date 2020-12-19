#version 330 core
precision mediump float;
precision mediump int;

uniform float GlobalTime;
uniform float DayTime;
uniform float MinLightLevel;
uniform mat4 projView;

// uniform vec3 tint[256*256];

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

flat out vec4 ColorLight;

//Intel SNB: VS vec4 shader: 54 instructions. 0 loops. 180 cycles. 0:0 spills:fills, 1 sends. Compacted 864 to 848 bytes (2%)

void main() {
	gl_Position = projView * vec4(positions[gl_VertexID], 1.0);

	vec4 tex = vec4(utex) * 0.0009765625 /* /1024.0 */;
	float frames = float(bhlf.w);
	tex.w /= frames;

	vec4 subUV = vec4(
		uvec4(qinfo, qinfo >> uint(5)) & uint(0x1F)
		) * 0.0625 /* /16.0 */;

	UvPosition = tex.xy + tex.zw*(subUV.xy + uv*(subUV.zw-subUV.xy) + vec2(0, trunc(mod(GlobalTime * 4.0f, frames))));
	Layer = bhlf.z;

	vec2 light = vec2((qinfo >> uint(10)) & uint(0xF)) * (1 / 15.0) * vec2(1, DayTime);
	float Light = clamp(length(light), MinLightLevel, 1.0);
	ColorLight = vec4(clamp(vec3(0.514, 0.710, 0.576)+float(qinfo.x>>14), 0, 1), Light);
}
