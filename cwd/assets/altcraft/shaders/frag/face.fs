#version 330 core
precision lowp float;

in vec2 UvPosition;
flat in uint Layer;

in VS_OUT {
	flat float Light;
	flat vec3 Color;
} fs_in;

uniform sampler2DArray textureAtlas;

void main() {
	vec4 color = texture(textureAtlas, vec3(UvPosition, Layer));
	if (color.a < 0.3)
		discard;

	color.rgb *= fs_in.Color * fs_in.Light;
	gl_FragColor = color;
}
