#pragma once

#include <glm/mat4x4.hpp>
#include <GL/glew.h>

#include "Vector.hpp"

class RenderState;
class RendererSectionData;

class RendererSection {
	enum buffers {
		BUFVERTS = 0,
		BUFQUAD,
		BUFCOUNT
	};
	enum textures {
		TEXVERTS = 0,
		TEXQUAD,
		TEXCOUNT
	};
	GLuint vertexarray = 0;
	GLuint buffers[BUFCOUNT] = { 0 };
	GLuint textures[TEXCOUNT] = { 0 };
	GLsizeiptr bufsizes[BUFCOUNT] = { 0 };
	
	static GLuint VboVertices, VboUvs;

	size_t hash;
    Vector sectionPos;

    RendererSection(const RendererSection &other) = delete;
public:
    RendererSection(const RendererSectionData &data);

    RendererSection(RendererSection &&other);

	~RendererSection();

	void Render();

    Vector GetPosition();

    size_t GetHash();

	size_t numOfFaces = 0;

    friend void swap(RendererSection &lhs, RendererSection &rhs);

	void UpdateData(const RendererSectionData &data);
};
