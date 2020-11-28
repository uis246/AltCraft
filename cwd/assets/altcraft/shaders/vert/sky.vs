#version 330 core

uniform mat4 projView;
uniform mat4 model;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uvPosition;

out vec2 uvPos;
out vec3 pos;

//Intel SNB
//VS vec4 shader: 24 instructions. 0 loops. 178 cycles. 0:0 spills:fills, 1 sends. Compacted 384 to 336 bytes (12%)
//Opt: VS vec4 shader: 17 instructions. 0 loops. 116 cycles. 0:0 spills:fills, 1 sends. Compacted 272 to 240 bytes (12%)

void main(){
    uvPos = uvPosition;
    pos = position;
    gl_Position = projView*model*vec4(position,1);
}
