#version 330 core
#extension GL_ARB_shader_image_load_store: enable

#ifdef GL_ARB_shader_image_load_store
layout(early_fragment_tests) in;
#endif

flat in vec3 color;

void main(){
    gl_FragColor = vec4(color, 1);
}
