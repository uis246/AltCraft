#version 330 core

flat in vec3 color;

void main(){
    gl_FragColor = vec4(color, 1);
}
