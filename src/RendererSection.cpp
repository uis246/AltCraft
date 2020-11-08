#include "RendererSection.hpp"

#include <easylogging++.h>

#include "Utility.hpp"
#include "Renderer.hpp"
#include "RendererSectionData.hpp"

RendererSection::RendererSection(const RendererSectionData &data) {
	OPTICK_EVENT();
	glGenBuffers(BUFCOUNT, buffers);
	glGenTextures(TEXCOUNT, textures);
    {
		//Vertices
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_BUFFER, textures[TEXVERTS]);
		glBindBuffer(GL_TEXTURE_BUFFER, buffers[BUFVERTS]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, buffers[BUFVERTS]);

//		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_BUFFER, textures[TEXQUAD]);
		glBindBuffer(GL_TEXTURE_BUFFER, buffers[BUFQUAD]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB16UI, buffers[BUFQUAD]);
//		glActiveTexture(GL_TEXTURE0);

		glBindBuffer(GL_TEXTURE_BUFFER, 0);
    }
    glBindVertexArray(0);
    glCheckError();
	
	UpdateData(data);
}

RendererSection::RendererSection(RendererSection && other) {
    using std::swap;
    swap(*this, other);
}

RendererSection::~RendererSection() {  
	for (int i = 0; i < BUFCOUNT; i++)
		if (buffers[i] != 0) {
			glBindBuffer(GL_TEXTURE_BUFFER, buffers[i]);
			glBufferData(GL_TEXTURE_BUFFER, 0, 0, GL_STATIC_DRAW);
        }

	glDeleteBuffers(BUFCOUNT, buffers);
	glDeleteTextures(TEXCOUNT, textures);
}

void swap(RendererSection & lhs, RendererSection & rhs) {
	std::swap(lhs.buffers, rhs.buffers);
	std::swap(lhs.textures, rhs.textures);
    std::swap(lhs.hash, rhs.hash);
    std::swap(lhs.numOfFaces, rhs.numOfFaces);
    std::swap(lhs.sectionPos, rhs.sectionPos);
}

void RendererSection::Render(RenderState &renderState) {
	OPTICK_EVENT();
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_BUFFER, textures[TEXVERTS]);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_BUFFER, textures[TEXQUAD]);
	glDrawArrays(GL_TRIANGLES, 0, 6 * numOfFaces);
	glCheckError();
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
	glBufferData(GL_TEXTURE_BUFFER, data.verts.size() * sizeof(glm::vec3), data.verts.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_TEXTURE_BUFFER, buffers[BUFQUAD]);
	glBufferData(GL_TEXTURE_BUFFER, data.quadInfo.size() * sizeof(uint16_t) * 3, data.quadInfo.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	glCheckError();

	numOfFaces = data.quadInfo.size();
	sectionPos = data.sectionPos;
	hash = data.hash;
}
