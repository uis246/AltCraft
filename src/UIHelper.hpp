#pragma once


#include "Platform.hpp"
#include "Vector.hpp"

//UI helper

typedef Vector2F Mat2x2F[2];

class AC_API UIHelper final {
	size_t element;
	struct RenderBuffer *buffer;
public:
	AC_INTERNAL static void InitHelper() noexcept;

	UIHelper(struct RenderBuffer *buffer) noexcept;

	//Add rectangle filled with color
	void AddColoredRect(Vector2F from, Vector2F to, const Vector3<float> color);
	void AddRect(Mat2x2F position, Mat2x2F uv, unsigned int layer, const Vector3<float> color);

	//Text
	static const std::u16string ASCIIToU16(std::string str) noexcept;
	static const std::u16string UnicodeToU16(std::string &str) noexcept;
	void GetTextSize(const std::u16string &string, Mat2x2F *returnSize) noexcept;
	void AddText(Vector2F position, const std::u16string &string, const Vector3<float> color);
};
