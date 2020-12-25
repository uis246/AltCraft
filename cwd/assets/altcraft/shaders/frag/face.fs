#version 330 core
precision lowp float;

in vec2 UvPosition;
flat in uint Layer;

flat in vec3 ColorMultiplier;

uniform sampler2DArray textureAtlas;

//Shader dispatch width limited to SIMD16: Fragment discard/demote not implemented in SIMD32 mode.
//Intel SNB: SIMD8 shader: 16 instructions. 0 loops. 856 cycles. 0:0 spills:fills, 2 sends, scheduled with mode top-down. Promoted 0 constants. Compacted 256 to 208 bytes (19%)
//Intel SNB: SIMD16 shader: 16 instructions. 0 loops. 868 cycles. 0:0 spills:fills, 2 sends, scheduled with mode top-down. Promoted 0 constants. Compacted 256 to 208 bytes (19%)

void main() {
	vec4 color = texture(textureAtlas, vec3(UvPosition, Layer));
	if (color.a < 0.3)
		discard;

	color.rgb *= ColorMultiplier;
	gl_FragColor = color;
}
