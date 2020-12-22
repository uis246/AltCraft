#include "RendererEntity.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "Entity.hpp"
#include "GameState.hpp"
#include "Renderer.hpp"
#include "AssetManager.hpp"

static const GLfloat vertices[] = {
    -0.5f, 0.5f, 0.5f,//0
    -0.5f, -0.5f, 0.5f,//1
    0.5f, -0.5f, 0.5f,//2
    0.5f, 0.5f, 0.5f,//3
    -0.5f, 0.5f, -0.5f,//4
    -0.5f, -0.5f, -0.5f,//5
    0.5f, -0.5f, -0.5f,//6
    0.5f, 0.5f, -0.5f,//7
};

static const GLushort elements[] = {
	0, 1,
	1, 2,
	2, 3,
	3, 0,
	4, 5,
	5, 6,
	6, 7,
	7, 4,
	5, 1,
	0, 4,
	6, 2,
	3, 7
};

GLuint Vbo[3], Vao = 0;

GLuint RendererEntity::GetVao(){
	if (Vao == 0) {
		glGenVertexArrays(1, &Vao);
		glBindVertexArray(Vao);

		glGenBuffers(3, Vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Vbo[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, Vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, Vbo[1]);
		//Matrix
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, ((4 * 4) + 3) * sizeof(GLfloat), (GLvoid*)(0 * 4 * sizeof(GLfloat)));
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, ((4 * 4) + 3) * sizeof(GLfloat), (GLvoid*)(1 * 4 * sizeof(GLfloat)));
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, ((4 * 4) + 3) * sizeof(GLfloat), (GLvoid*)(2 * 4 * sizeof(GLfloat)));
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, ((4 * 4) + 3) * sizeof(GLfloat), (GLvoid*)(3 * 4 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glVertexAttribDivisor(1, 1);
		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);

		//Color
		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, ((4 * 4) + 3) * sizeof(GLfloat), (GLvoid*)(4 * 4 * sizeof(GLfloat)));
		glEnableVertexAttribArray(5);
		glVertexAttribDivisor(5, 1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    return Vao;
}

GLuint RendererEntity::GetColoMatVBO() {
	return Vbo[1];
}

RendererEntity::RendererEntity(unsigned int id)
{
    entityId = id;
}

RendererEntity::~RendererEntity() {
}

void RendererEntity::RenderBatch(const int count) noexcept {
	OPTICK_EVENT();

	glDrawElementsInstanced(GL_LINES, 24, GL_UNSIGNED_SHORT, 0, count);
}

void RendererEntity::AppendInstance(uint8_t *ptr, const World *world) noexcept {
	const Entity &entity = world->GetEntity(entityId);

	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, entity.pos.glm());
	model = glm::translate(model, glm::vec3(0, entity.height / 2.0, 0));
	model = glm::scale(model, glm::vec3(entity.width, entity.height, entity.width));
	memcpy(ptr, glm::value_ptr(model), sizeof(glm::mat4));

	memcpy(ptr + sizeof(glm::mat4), glm::value_ptr(entity.renderColor), sizeof(glm::vec3));
}

void RendererEntity::AppendStatic(uint8_t *ptr, glm::mat4 &matrix, glm::vec3 color) noexcept {
	memcpy(ptr, glm::value_ptr(matrix), sizeof(glm::mat4));

	memcpy(ptr + sizeof(glm::mat4), glm::value_ptr(color), sizeof(glm::vec3));
}
