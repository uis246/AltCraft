#include "Renderer.hpp"
#include "Plugin.hpp"

void RenderState::SetActiveVao(GLuint Vao) noexcept {
	OPTICK_EVENT();
    glBindVertexArray(Vao);
    ActiveVao = Vao;
}

void RenderState::SetActiveShader(GLuint Shader) {
	if (Shader != ActiveShader) {
		glUseProgram(Shader);
		ActiveShader = Shader;
	}
}
