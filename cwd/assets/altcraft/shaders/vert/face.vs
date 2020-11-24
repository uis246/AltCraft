#version 330 core
#extension GL_ARB_shader_draw_parameters : require
precision lowp float;

uniform float GlobalTime;
uniform float DayTime;
uniform float MinLightLevel;
uniform mat4 projView;

//Section offset
//section.xyz*16
uniform vec3 sectionOffset;

//xxxx yyyy zzzz
uniform samplerBuffer pos;
//3*4*4=12*4=48

//Per quad info
//RRRR GGGG BBBB AAAA
//xxww yyhh pT(L1)f{Uu} h(L2)l{Vv}
//{
//Uu - 10 bit
//f - 6 bit
//L1 - 4 bit
//T - 1 bit
//p -  8 bit
//}3 free
//{
//Vv - 10 bit
//l - 6 bit
//L2 - 4 bit
//h - 8 bit
//}4 free

//xx yy ww hh 4*2=8
//p h l f 4*1=4 //12
//T(L1){Uu} (L2){Vv} 2*2=4 //16

layout(location = 0) in uvec4 qinfo;
layout(location = 1) in vec2 uv;

out vec2 UvPosition;
flat out uint Layer;

out VS_OUT {
	flat float Light;
	flat vec3 Color;
} vs_out;

void main()
{
// 	uint quad = uint(gl_InstanceID+gl_BaseInstanceARB)+uint(gl_VertexID)/uint(4);
// 	uint vert = uint(gl_VertexID)%uint(4);
// 	vec2 mul = vec2(vert&uint(1), vert>>uint(1));
	uint quad = uint(gl_InstanceID+gl_BaseInstanceARB);
	uint vert = uint(gl_VertexID);
	gl_Position = projView * vec4(texelFetch(pos, int(vert+(quad*uint(4)))).rgb, 1.0);

	vec2 uv_start = vec2(qinfo.zw & uint(0x1F)) / 16.0;
	vec2 uv_end = vec2((qinfo.zw >> uint(5)) & uint(0x1F)) / 16.0;
	vec2 light = vec2((qinfo.zw >> uint(16)) & uint(0xF)) / 15.0;

	vec4 tex = vec4((qinfo.xy >> uint(16)) & uint(0xFFFF), (qinfo.xy) & uint(0xFFFF)) / 1024.0;
	uvec2 lf = uvec2((qinfo.zw >> uint(10)) & uint(0x3F));
	float frames = float(lf.x);
	tex.w = tex.w / frames;
	tex.y += trunc(mod(GlobalTime * 4.0f, frames)) * tex.w;

	tex.xy += uv_start * tex.zw;
	tex.zw = (uv_end-uv_start) * tex.zw;

	UvPosition = tex.xy + tex.zw*uv;
	vs_out.Light = clamp(light.x + (light.y * DayTime), MinLightLevel, 1.0);
	Layer = lf.y;
	vs_out.Color = vec3(0.275, 0.63, 0.1) * ((qinfo.z>>20)&uint(1));
}
