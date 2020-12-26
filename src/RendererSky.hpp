#pragma once

#include <GL/glew.h>

class RenderState;

class RendererSky {
    GLuint VBOs[2], Vao;
public:
    RendererSky();
    ~RendererSky();
    void Render(RenderState &renderState) noexcept;
};
