#version 330
uniform mat4 projection;

layout(location = 0) in vec2 Position;
layout(location = 1) in vec3 UVLayered;
layout(location = 2) in vec4 Color;

out vec4 ColorMultiplier;
out vec2 UV;
flat out uint Layer;
void main() {
	ColorMultiplier = Color;
	UV = UVLayered.xy;
	Layer = uint(UVLayered.z);
	gl_Position = /* projection * */ vec4(Position, 0, 1);
}
