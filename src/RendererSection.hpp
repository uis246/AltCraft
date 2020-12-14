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
	GLuint vertexarray = 0;
	GLuint VBO = 0;
	GLsizeiptr bufsize = 0;

	static GLuint VboUvs;

	size_t hash;
    Vector sectionPos;

    RendererSection(RendererSection &other) = delete;
public:
    RendererSection(RendererSectionData &data);

    RendererSection(RendererSection &&other);

	~RendererSection();

	void Render();

    Vector GetPosition();

    size_t GetHash();

	size_t numOfFaces = 0;

    friend void swap(RendererSection &lhs, RendererSection &rhs);

	void UpdateData(RendererSectionData &data);
};
