#version 330 core

uniform mat4 projView;
layout (location = 0) in vec3 position;
layout (location = 1) in mat4 model;
//2
//3
//4
layout (location = 5) in vec3 colorIn;

flat out vec3 color;

//Intel SNB
//VS vec4 shader: 23 instructions. 0 loops. 176 cycles. 0:0 spills:fills, 1 sends. Compacted 368 to 320 bytes (13%)
//Opt: VS vec4 shader: 16 instructions. 0 loops. 116 cycles. 0:0 spills:fills, 1 sends. Compacted 256 to 224 bytes (12%)

void main(){
    color = colorIn;
    gl_Position = projView*model*vec4(position, 1);
}
