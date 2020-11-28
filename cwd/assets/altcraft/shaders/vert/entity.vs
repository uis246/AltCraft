#version 330 core

uniform mat4 projView;
uniform mat4 model;
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uvPosition;
out vec2 uvPos;

//Intel SNB
//VS vec4 shader: 23 instructions. 0 loops. 176 cycles. 0:0 spills:fills, 1 sends. Compacted 368 to 320 bytes (13%)
//Opt: VS vec4 shader: 16 instructions. 0 loops. 116 cycles. 0:0 spills:fills, 1 sends. Compacted 256 to 224 bytes (12%)

void main(){
    uvPos = uvPosition;
    gl_Position = projView*model*vec4(position,1);
}
