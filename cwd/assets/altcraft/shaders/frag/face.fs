#version 330 core
precision lowp float;

in vec2 UvPosition;
flat in uint Layer;

flat in vec4 ColorLight;

uniform sampler2DArray textureAtlas;

//Intel SNB:  SIMD8 shader: 19 instructions. 0 loops. 870 cycles. 0:0 spills:fills, 2 sends, scheduled with mode top-down. Promoted 0 constants. Compacted 304 to 256 bytes (16%)
//Intel SNB: SIMD16 shader: 19 instructions. 0 loops. 884 cycles. 0:0 spills:fills, 2 sends, scheduled with mode top-down. Promoted 0 constants. Compacted 304 to 256 bytes (16%)

void main() {
	vec4 color = texture(textureAtlas, vec3(UvPosition, Layer));
	if (color.a < 0.3)
		discard;

	color.rgb *= ColorLight.rgb * ColorLight.a;
	gl_FragColor = color;
}
