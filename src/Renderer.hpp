#pragma once

#include <GL/glew.h>

class RenderState {
	GLuint ActiveVao = -1;
	GLuint ActiveShader = -1;
public:
	void SetActiveVao(GLuint Vao) noexcept;
	void SetActiveShader(GLuint Shader);
    unsigned int WindowWidth, WindowHeight;
    long long TimeOfDay;
};
