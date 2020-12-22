#pragma once

#include <GL/glew.h>
#include <glm/mat4x4.hpp>

//class RenderState;
class Entity;
class World;

class RendererEntity {
    unsigned int entityId;
public:
    RendererEntity(unsigned int id);
    ~RendererEntity();

    static void RenderBatch(const int count) noexcept;
    void AppendInstance(uint8_t *ptr, const World *world) noexcept;
    static void AppendStatic(uint8_t *ptr, glm::mat4 &matrix, glm::vec3 color) noexcept;

    static GLuint GetVao();
    static GLuint GetColoMatVBO();
    static constexpr inline size_t InstanceSize() noexcept {
	    return sizeof(glm::mat4) + sizeof(glm::vec3);
    }
};
