#pragma once


#include "Platform.hpp"
#include "Vector.hpp"

//UI helper

typedef Vector2F Mat2x2F[2];

class AC_API UIHelper final {
	struct RenderBuffer *buffer;
	size_t element;
	float vto;
public:
	AC_INTERNAL static void InitHelper() noexcept;

	UIHelper(struct RenderBuffer *buffer) noexcept;

	//Add rectangle filled with color
	void AddColoredRect(Vector2F from, Vector2F to, const Vector3<float> color);
	void AddRect(Mat2x2F position, Mat2x2F uv, unsigned int layer, const Vector3<float> color, const float opacity);

	//Text
	static const std::u16string ASCIIToU16(std::string str) noexcept;
	static const std::u16string UnicodeToU16(std::string &str) noexcept;
	Vector2F GetTextSize(const std::u16string &string, const float scale) noexcept;
	void AddText(const Vector2F position, const std::u16string &string, const float scale, const Vector3<float> color);
	void SetVerticalOffset(float offset);

	//Screen space converter
	enum origin {
		CENTER,
		UPLEFT,
		UPRIGHT,
		DOWNRIGHT,
		DOWNLEFT,
		ORIGINS
	};
	//Get NDC from pixel offset
	Vector2F GetCoord(const enum origin, Vector2F pixels) noexcept;
};
