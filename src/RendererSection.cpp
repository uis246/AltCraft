#include "RendererSection.hpp"

#include <easylogging++.h>

#include "Utility.hpp"
#include "Render.hpp"
#include "RendererSectionData.hpp"

RendererSection::RendererSection(const RendererSectionData &data) {
	OPTICK_EVENT();
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

//		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_BUFFER, textures[TEXQUAD]);
		glBindBuffer(GL_TEXTURE_BUFFER, buffers[BUFQUAD]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32UI, buffers[BUFQUAD]);

//		glActiveTexture(GL_TEXTURE0);
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
	for (int i = 0; i < BUFCOUNT; i++) {
		if (buffers[i] != 0) {
			glBindBuffer(GL_TEXTURE_BUFFER, buffers[i]);
			glBufferData(GL_TEXTURE_BUFFER, 0, 0, GL_STATIC_DRAW);
			glDeleteBuffers(1, &buffers[i]);
		}
		if (textures[i] != 0)
			glDeleteTextures(1, &textures[i]);
	}
}

void swap(RendererSection & lhs, RendererSection & rhs) {
	std::swap(lhs.buffers, rhs.buffers);
	std::swap(lhs.textures, rhs.textures);
    std::swap(lhs.hash, rhs.hash);
    std::swap(lhs.numOfFaces, rhs.numOfFaces);
    std::swap(lhs.sectionPos, rhs.sectionPos);
}

void RendererSection::Render() {
	OPTICK_EVENT();
	//Bind quad verts texture
	if (numOfFaces == 0)
		return;
	if (GLEW_ARB_multi_bind) {
		glBindTextures(3, TEXCOUNT, textures);
	} else
	{
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_BUFFER, textures[TEXVERTS]);
		//Bind quad info texture
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_BUFFER, textures[TEXQUAD]);
	}

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
	glActiveTexture(GL_TEXTURE3);
	glBindBuffer(GL_TEXTURE_BUFFER, buffers[BUFVERTS]);
	glBufferData(GL_TEXTURE_BUFFER, bufsizes[BUFVERTS], NULL, GL_STATIC_DRAW);//Orphan buffer
	GLsizeiptr newSize = data.verts.size() * sizeof(glm::vec3);
	if (newSize > bufsizes[BUFVERTS]) {//Reallocate
		glBufferData(GL_TEXTURE_BUFFER, newSize, data.verts.data(), GL_STATIC_DRAW);
		bufsizes[BUFVERTS] = newSize;
	} else
		glBufferSubData(GL_TEXTURE_BUFFER, 0, newSize, data.verts.data());

	glBindBuffer(GL_TEXTURE_BUFFER, buffers[BUFQUAD]);
	glBufferData(GL_TEXTURE_BUFFER, bufsizes[BUFQUAD], NULL, GL_STATIC_DRAW);//Orphan buffer
	newSize = data.quadInfo.size() * sizeof(uint32_t);
	if (newSize > bufsizes[BUFQUAD]) {//Reallocate
		glBufferData(GL_TEXTURE_BUFFER, newSize, data.quadInfo.data(), GL_STATIC_DRAW);
		bufsizes[BUFQUAD] = newSize;
	} else
		glBufferSubData(GL_TEXTURE_BUFFER, 0, newSize, data.quadInfo.data());

	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	glCheckError();

	numOfFaces = data.verts.size()/4;
	sectionPos = data.sectionPos;
	hash = data.hash;
}
