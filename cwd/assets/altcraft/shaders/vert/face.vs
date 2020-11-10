#version 330 core
precision lowp float;

uniform float GlobalTime;
uniform float DayTime;
uniform float MinLightLevel;
uniform mat4 projView;

//Section offset
//section.xyz*16
uniform vec3 sectionOffset;

//Texture pos format
//xxxx yyyy wwww hhhh
//uniform samplerBuffer texturePos;//Texture 2
//Next texture pos format
uniform usamplerBuffer texturePos;
//xxww yyhh plpf
//p - padding
//Should be zero

//xxxx yyyy zzzz
uniform samplerBuffer pos;//Tex 3
//3*4*4=12*4=48

//Per quad info
//RR GG BB AA
//ph tt {T0lUv} {LVv}
//p - block horisontal pos (z<<4|x)
//h - block vertical pos
//tt- texture id
//lUu - uv with block light
//LVv - uv with sky light
//T - tint flag

uniform usamplerBuffer quadInfo;//Tex 4

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
	uint vert = (uint(gl_VertexID)%uint(3)) ^ (m<<1 | m);
	vec2 mul = vec2(vert&uint(1), vert>>uint(1));//CW front
	gl_Position = projView * vec4(texelFetch(pos, int(vert+(quad*uint(4)))).rgb+sectionOffset, 1.0);

	uvec4 qinfo = texelFetch(quadInfo, int(quad)).rgba;

	vec2 uv_start = vec2(qinfo.zw & uint(0x1F)) / 16.0;
	vec2 uv_end = vec2((qinfo.zw >> uint(5)) & uint(0x1F)) / 16.0;
	vec2 light = vec2((qinfo.zw >> uint(10)) & uint(0xF)) / 15.0;

	uvec3 texf = texelFetch(texturePos, int(qinfo.y)).xyz;
	uvec3 up = texf >> uint(16);
	uvec3 low = texf & uint(0xFFFF);
	vec4 tex = vec4(up.xy, low.xy) / 1024.0;
	float frames = float(low.z);
	tex.w = tex.w / frames;
	tex.y += trunc(mod(GlobalTime * 4.0f, frames)) * tex.w;

	tex.xy += uv_start * tex.zw;
	tex.zw = (uv_end-uv_start) * tex.zw;

	UvPosition = tex.xy + tex.zw*mul;
	vs_out.Light = clamp(light.x + (light.y * DayTime), MinLightLevel, 1.0);
	vs_out.Layer = up.z;
	vs_out.Color = vec3(0.275, 0.63, 0.1) * (qinfo.z>>15);
}
