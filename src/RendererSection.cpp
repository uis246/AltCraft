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
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA16UI, buffers[BUFQUAD]);
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

void RendererSection::Render() {
	OPTICK_EVENT();
	//Bind quad verts texture
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_BUFFER, textures[TEXVERTS]);
	//Bind quad info texture
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_BUFFER, textures[TEXQUAD]);
	//Or we can use ARB_multi_bind
	glCheckError();

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
	glBufferData(GL_TEXTURE_BUFFER, bufsizes[BUFVERTS], NULL, GL_STATIC_DRAW);//Orphan buffer
	GLsizeiptr newSize = data.verts.size() * sizeof(glm::vec3);
	if (newSize > bufsizes[BUFVERTS]) {//Reallocate
		glBufferData(GL_TEXTURE_BUFFER, newSize, data.verts.data(), GL_STATIC_DRAW);
		bufsizes[BUFVERTS] = newSize;
	} else
		glBufferSubData(GL_TEXTURE_BUFFER, 0, newSize, data.verts.data());

	glBindBuffer(GL_TEXTURE_BUFFER, buffers[BUFQUAD]);
	glBufferData(GL_TEXTURE_BUFFER, bufsizes[BUFQUAD], NULL, GL_STATIC_DRAW);//Orphan buffer
	newSize = data.quadInfo.size() * sizeof(uint16_t) * 4;
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
