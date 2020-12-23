#include "RendererSky.hpp"

#include "Renderer.hpp"
#include "Utility.hpp"
#include "Platform.hpp"

const GLfloat vertices[] = {
    //Z+ edge
    -0.5f, -0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,    
    0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,    
    0.5f, 0.5f, 0.5f,

    //Z- edge
    -0.5f, 0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,    
    0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,    
    0.5f, 0.5f, -0.5f,

    //X+ edge    
    -0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f,    
    -0.5f, -0.5f, 0.5f,
    -0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, 0.5f,

    //X- edge    
    0.5f, 0.5f, -0.5f,
    0.5f, -0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,    
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, 0.5f,
    0.5f, 0.5f, -0.5f,

    //Y+ edge    
    -0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, 0.5f,    
    -0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, 0.5f,

    //Y- edge    
    0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,    
    0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f,
};

const GLfloat uv_coords[] = {
    //Z+
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,

    //Z-
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,

    //X+
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,

    //X-
    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,

    //Y+
    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,

    //Y-
    1.0f, 0.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
};

RendererSky::RendererSky() {
	glGenVertexArrays(1, &Vao);
	glGenBuffers(2, VBOs);

	glBindVertexArray(Vao);
	{
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uv_coords), uv_coords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	}
	glBindVertexArray(0);
	glCheckError();
}

RendererSky::~RendererSky() {
    glDeleteBuffers(2, VBOs);
    glDeleteVertexArrays(1, &Vao);
    //glCheckError();
}

void RendererSky::Render(RenderState &renderState) {
	OPTICK_EVENT();
    renderState.SetActiveVao(Vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glCheckError();
}
