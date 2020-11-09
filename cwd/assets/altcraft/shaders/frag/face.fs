#version 330 core

in vec2 UvPosition;

in VS_OUT {
//     vec2 UvPosition;
//     vec3 Texture;
//     vec3 Color;
//     vec2 Light;
// 	flat float biome;//debug only
	flat uint Layer;
	flat float Light;
	flat vec3 Color;
} fs_in;

uniform sampler2DArray textureAtlas;


vec3 rgb2hsv(vec3 c)
{
	vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
	vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
	vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

	float d = q.x - min(q.w, q.y);
	float e = 1.0e-10;
	return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
	vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
	return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
	vec4 color = texture(textureAtlas, vec3(UvPosition, fs_in.Layer));
//// 	if (color.a < 0.3)
//// 		discard;

// 	vec3 hsvColor = rgb2hsv(color.xyz);
// 	hsvColor+=fs_in.Color;
//	color = vec4(hsv2rgb(hsvColor),1);

	color.rgb *= fs_in.Light;
	gl_FragColor = color;

// 	gl_FragColor = vec4(fs_in.UvPosition, texture(textureAtlas, vec3(fs_in.UvPosition, fs_in.Layer)).r, fs_in.Light);
// 	gl_FragColor = vec4(texture(textureAtlas, vec3(UvPosition, fs_in.Layer)).rgb, (fs_in.Light*0.001)+float(fs_in.Layer));
}
