#version 330 core
layout (location = 0) in vec2 Pos;
layout (location = 1) in vec2 TextureCoords;

out vec2 TexCoords;

//Intel SNB
//VS vec4 shader: 6 instructions. 0 loops. 22 cycles. 0:0 spills:fills, 1 sends. Compacted 96 to 96 bytes (0%)

void main()
{
    gl_Position = vec4(Pos.x, Pos.y, 0.0, 1.0); 
    TexCoords = TextureCoords;
}  
