#include "RendererSection.hpp"

#include <easylogging++.h>

#include "Utility.hpp"
#include "Render.hpp"
#include "RendererSectionData.hpp"

static constexpr GLsizei stride = (4 * sizeof(uint32_t)) + (4 * 3 * sizeof(GLfloat));

static const GLfloat uv_coords[] = {
	0.f, 1.f,
	1.f, 1.f,
	0.f, 0.f,
	1.f, 0.f
};

GLuint RendererSection::VboUvs = 0;

RendererSection::RendererSection(RendererSectionData &data) {
	OPTICK_EVENT();
	if (!VboUvs) {
		glGenBuffers(1, &VboUvs);

		glBindBuffer(GL_ARRAY_BUFFER, VboUvs);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uv_coords), uv_coords, GL_STATIC_DRAW);
	}
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &vertexarray);
	{
		glBindVertexArray(vertexarray);

		glBindBuffer(GL_ARRAY_BUFFER, VboUvs);
		GLuint UvAttribPos = 1;
		glVertexAttribPointer(UvAttribPos, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(UvAttribPos);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		GLuint TexAttribPos = 6;
		glVertexAttribIPointer(TexAttribPos, 4, GL_UNSIGNED_SHORT, stride, 0);
		glEnableVertexAttribArray(TexAttribPos);
		glVertexAttribDivisor(TexAttribPos, 1);

		GLuint QuadAttribPos = 0;
		glVertexAttribIPointer(QuadAttribPos, 2, GL_UNSIGNED_SHORT, stride, reinterpret_cast<void*>(3 * 4));
		glEnableVertexAttribArray(QuadAttribPos);
		glVertexAttribDivisor(QuadAttribPos, 1);

		GLuint PhlfAttribPos = 7;
		glVertexAttribIPointer(PhlfAttribPos, 4, GL_UNSIGNED_BYTE, stride, reinterpret_cast<void*>(2 * 4));
		glEnableVertexAttribArray(PhlfAttribPos);
		glVertexAttribDivisor(PhlfAttribPos, 1);

		//Vertices
		GLuint PositionsAttribPos = 2;
		glVertexAttribPointer(PositionsAttribPos+0, 3, GL_FLOAT, GL_FALSE, stride,
				      reinterpret_cast<void*>(4*4));

		glVertexAttribPointer(PositionsAttribPos+1, 3, GL_FLOAT, GL_FALSE, stride,
				      reinterpret_cast<void*>((4*4) + (3*sizeof(GLfloat))));

		glVertexAttribPointer(PositionsAttribPos+2, 3, GL_FLOAT, GL_FALSE, stride,
				      reinterpret_cast<void*>((4*4) + (2*3*sizeof(GLfloat))));

		glVertexAttribPointer(PositionsAttribPos+3, 3, GL_FLOAT, GL_FALSE, stride,
				      reinterpret_cast<void*>((4*4) + (3*3*sizeof(GLfloat))));

		glEnableVertexAttribArray(PositionsAttribPos+0);
		glEnableVertexAttribArray(PositionsAttribPos+1);
		glEnableVertexAttribArray(PositionsAttribPos+2);
		glEnableVertexAttribArray(PositionsAttribPos+3);
		glVertexAttribDivisor(PositionsAttribPos+0, 1);
		glVertexAttribDivisor(PositionsAttribPos+1, 1);
		glVertexAttribDivisor(PositionsAttribPos+2, 1);
		glVertexAttribDivisor(PositionsAttribPos+3, 1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glCheckError();
	
	UpdateData(data);
}

RendererSection::RendererSection(RendererSection && other) {
    using std::swap;
    swap(*this, other);
}

RendererSection::~RendererSection() {  
	if (vertexarray != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);//???
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &vertexarray);
	}
}

void swap(RendererSection & lhs, RendererSection & rhs) {
	std::swap(lhs.VBO, rhs.VBO);
	std::swap(lhs.vertexarray, rhs.vertexarray);
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

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, numOfFaces);
}

Vector RendererSection::GetPosition() {
    return sectionPos;
}

size_t RendererSection::GetHash() {
    return hash;
}

void RendererSection::UpdateData(RendererSectionData & data) {
	OPTICK_EVENT();

	numOfFaces = data.verts.size()/4;
	sectionPos = data.sectionPos;
	hash = data.hash;

	GLsizeiptr	vertsize = data.verts.size() * sizeof(glm::vec3),
			quadsize = data.quadInfo.size() * sizeof(uint32_t),
			newSize = vertsize + quadsize;

	static constexpr GLsizei stride32 = stride/4;

	//Reorder
	uint32_t *buffer = reinterpret_cast<uint32_t*>(alloca(newSize));
	const uint32_t *qinfo = data.quadInfo.data();
	const uint32_t *vert = reinterpret_cast<const uint32_t*>(data.verts.data());
	for(size_t i = 0; i < numOfFaces; i++) {
		buffer[i*stride32] = qinfo[i*4];
		buffer[i*stride32 + 1] = qinfo[i*4 + 1];
		buffer[i*stride32 + 2] = qinfo[i*4 + 2];
		buffer[i*stride32 + 3] = qinfo[i*4 + 3];
		memcpy(buffer+(i*stride32)+4, vert+(i*3*4), 3*4*4);
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, bufsize, NULL, GL_STATIC_DRAW);//Orphan buffer
	if (newSize > bufsize) {//Reallocate
		glBufferData(GL_ARRAY_BUFFER, newSize, buffer, GL_STATIC_DRAW);
		bufsize = newSize;
	} else
		glBufferSubData(GL_ARRAY_BUFFER, 0, newSize, buffer);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glCheckError();

	data.quadInfo.clear();
	data.quadInfo.shrink_to_fit();
	data.verts.clear();
	data.verts.shrink_to_fit();
}
