#pragma once

#include "Platform.hpp"
#include "Vector.hpp"

//UI helper

class AC_API UIHelper final {
	size_t element;
	struct RenderBuffer *buffer;
public:
	static void InitHelper() noexcept;

	UIHelper(struct RenderBuffer *buffer) noexcept;

	//Add rectangle filled with color
	void AddColoredRect(Vector2F from, Vector2F to, Vector3<float> color);
};
