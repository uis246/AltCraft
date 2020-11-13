#version 330 core
precision lowp float;

uniform float GlobalTime;
uniform float DayTime;
uniform float MinLightLevel;
uniform mat4 projView;

//Section offset
//section.xyz*16
uniform vec3 sectionOffset;

//xxxx yyyy zzzz
uniform samplerBuffer pos;//Tex 3
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

uniform usamplerBuffer quadInfo;//Tex 4

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
	uint vert = (uint(gl_VertexID)%uint(3)) ^ (m<<1 | m);
	vec2 mul = vec2(vert&uint(1), vert>>uint(1));//CW front
	gl_Position = projView * vec4(texelFetch(pos, int(vert+(quad*uint(4)))).rgb+sectionOffset, 1.0);

	uvec4 qinfo = texelFetch(quadInfo, int(quad)).rgba;

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

	UvPosition = tex.xy + tex.zw*mul;
	vs_out.Light = clamp(light.x + (light.y * DayTime), MinLightLevel, 1.0);
	vs_out.Layer = lf.y;
	vs_out.Color = vec3(0.275, 0.63, 0.1) * ((qinfo.z>>20)&uint(1));
}
