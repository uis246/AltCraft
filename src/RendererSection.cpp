#include "RendererSection.hpp"

#include <easylogging++.h>

#include "Utility.hpp"
#include "Render.hpp"
#include "RendererSectionData.hpp"

static const GLfloat uv_coords[] = {
	0.f, 0.f,
	1.f, 0.f,
	0.f, 1.f,
	1.f, 1.f
};

GLuint RendererSection::VboUvs = 0;

RendererSection::RendererSection(const RendererSectionData &data) {
	OPTICK_EVENT();
	if (!VboUvs) {
		glGenBuffers(1, &VboUvs);

		glBindBuffer(GL_ARRAY_BUFFER, VboUvs);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uv_coords), uv_coords, GL_STATIC_DRAW);
	}
	glGenBuffers(BUFCOUNT, buffers);
	glGenTextures(TEXCOUNT, textures);
	glGenVertexArrays(1, &vertexarray);
	{
		glBindVertexArray(vertexarray);
		//Vertices
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_BUFFER, textures[TEXVERTS]);
		glBindBuffer(GL_TEXTURE_BUFFER, buffers[BUFVERTS]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, buffers[BUFVERTS]);

		GLuint QuadAttribPos = 0;
		glBindBuffer(GL_ARRAY_BUFFER, buffers[BUFQUAD]);
		glVertexAttribIPointer(QuadAttribPos, 4, GL_UNSIGNED_INT, 4 * 4, 0);
		glEnableVertexAttribArray(QuadAttribPos);
		glVertexAttribDivisor(QuadAttribPos, 1);

		GLuint UvAttribPos = 1;
		glBindBuffer(GL_ARRAY_BUFFER, VboUvs);
		glVertexAttribPointer(UvAttribPos, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(UvAttribPos);

		glBindVertexArray(0);
		glBindBuffer(GL_TEXTURE_BUFFER, 0);
	}
	glCheckError();
	
	UpdateData(data);
}

RendererSection::RendererSection(RendererSection && other) {
    using std::swap;
    swap(*this, other);
}

RendererSection::~RendererSection() {  
	if (buffers[0] != 0) {
		glBindBuffer(GL_TEXTURE_BUFFER, buffers[0]);
		glBufferData(GL_TEXTURE_BUFFER, 0, 0, GL_STATIC_DRAW);
		glDeleteBuffers(BUFCOUNT, buffers);
		glDeleteTextures(TEXCOUNT, textures);
		glDeleteVertexArrays(1, &vertexarray);
	}
}

void swap(RendererSection & lhs, RendererSection & rhs) {
	std::swap(lhs.buffers, rhs.buffers);
	std::swap(lhs.textures, rhs.textures);
	std::swap(lhs.vertexarray, rhs.vertexarray);
	std::swap(lhs.bufsizes, rhs.bufsizes);
    std::swap(lhs.hash, rhs.hash);
    std::swap(lhs.numOfFaces, rhs.numOfFaces);
    std::swap(lhs.sectionPos, rhs.sectionPos);
}

void RendererSection::Render() {
	OPTICK_EVENT();
	if (numOfFaces == 0)
		return;
	//Bind vertices texture
	glBindVertexArray(vertexarray);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_BUFFER, textures[TEXVERTS]);

//	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, numOfFaces);
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, numOfFaces);
}

Vector RendererSection::GetPosition() {
    return sectionPos;
}

size_t RendererSection::GetHash() {
    return hash;
}

void RendererSection::UpdateData(const RendererSectionData & data) {
	OPTICK_EVENT();
	glActiveTexture(GL_TEXTURE4);
	glBindBuffer(GL_TEXTURE_BUFFER, buffers[BUFVERTS]);
	glBufferData(GL_TEXTURE_BUFFER, bufsizes[BUFVERTS], NULL, GL_STATIC_DRAW);//Orphan buffer
	GLsizeiptr newSize = data.verts.size() * sizeof(glm::vec3);
	if (newSize > bufsizes[BUFVERTS]) {//Reallocate
		glBufferData(GL_TEXTURE_BUFFER, newSize, data.verts.data(), GL_STATIC_DRAW);
		bufsizes[BUFVERTS] = newSize;
	} else
		glBufferSubData(GL_TEXTURE_BUFFER, 0, newSize, data.verts.data());

	glBindBuffer(GL_ARRAY_BUFFER, buffers[BUFQUAD]);
	glBufferData(GL_ARRAY_BUFFER, bufsizes[BUFQUAD], NULL, GL_STATIC_DRAW);//Orphan buffer
	newSize = data.quadInfo.size() * sizeof(uint32_t);
	if (newSize > bufsizes[BUFQUAD]) {//Reallocate
		glBufferData(GL_ARRAY_BUFFER, newSize, data.quadInfo.data(), GL_STATIC_DRAW);
		bufsizes[BUFQUAD] = newSize;
	} else
		glBufferSubData(GL_ARRAY_BUFFER, 0, newSize, data.quadInfo.data());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	glCheckError();

	numOfFaces = data.verts.size()/4;
	sectionPos = data.sectionPos;
	hash = data.hash;
}
