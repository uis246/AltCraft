#pragma once


#include "Platform.hpp"
#include "Vector.hpp"

//UI helper

typedef Vector2F Mat2x2F[2];

class AC_API UIHelper final {
	size_t element;
	struct RenderBuffer *buffer;
public:
	static void InitHelper() noexcept;

	UIHelper(struct RenderBuffer *buffer) noexcept;

	//Add rectangle filled with color
	void AddColoredRect(Vector2F from, Vector2F to, const Vector3<float> color);
	void AddRect(Mat2x2F position, Mat2x2F uv, unsigned int layer, const Vector3<float> color);

	//Text
	void GetTextSize(std::u16string &string, Mat2x2F *returnSize) noexcept;
	void AddText(Vector2F position, std::u16string &string, const Vector3<float> color);
};
