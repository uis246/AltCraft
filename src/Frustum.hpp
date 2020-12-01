#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class Frustum {
    enum FrustumSide {
        RIGHT,
        LEFT,
        BOTTOM,
        TOP,
	FAR,
        NEAR,
	SIDE_COUNT,
    };

	glm::vec4 planes[SIDE_COUNT];

public:    
	Frustum(const glm::mat4 &vpMat);

	~Frustum() = default;

	bool TestPoint(const glm::vec3 &pos);

	bool TestSphere(const glm::vec3 &pos, float radius);
};
