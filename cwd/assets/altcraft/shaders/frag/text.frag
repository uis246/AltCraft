#version 330
uniform sampler2DArray sampler;

in vec4 ColorMultiplier;
in vec2 UV;
flat in uint Layer;
void main() {
	gl_FragColor = texture(sampler, vec3(UV, Layer)) * ColorMultiplier;
}
