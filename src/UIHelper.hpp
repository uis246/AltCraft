#pragma once

#include "Platform.hpp"
#include "Vector.hpp"

//UI helper

typedef Vector2F Mat2x2F[2];

struct TextInput {
	const Vector3<float> background, foreground;
	const Vector2F drawPos, endPos;
	uint16_t *text;
	// Offset of visible text
	size_t windowOffset = 0;
	// Input offset
	size_t cursorPosition = 0;
	float scale = 1;
};

class AC_API UIHelper final {
	struct RenderBuffer *buffer;
	size_t element;
	float vto;//Vertical Text Offset
public:
	const struct RenderState *state;
	const Vector2F windowSize;//Float but in pixels

	AC_INTERNAL static int TextInput;
	AC_INTERNAL static void InitHelper() noexcept;

	UIHelper(struct RenderBuffer *buffer) noexcept;
	UIHelper(const struct RenderState *rs) noexcept;

	//Add rectangle filled with color
	void AddColoredRect(Vector2F from, Vector2F to, const Vector3<float> color) noexcept;
	void AddRect(Mat2x2F position, Mat2x2F uv, unsigned int layer, const Vector3<float> color, const float opacity) noexcept;

	//Text
	static const std::u16string ASCIIToU16(const std::string &str) noexcept;
	static const std::u16string UnicodeToU16(const std::string &str) noexcept;
	Vector2F GetTextSize(const std::u16string &string, const float scale) noexcept;
	unsigned int GetTextWidth(const std::u16string &string) noexcept;// Support multiline
	size_t GetMaxFitChars(const std::u16string &string, const size_t first, const unsigned int width) noexcept;
	void AddText(const Vector2F position, const std::u16string &string, const float scale, const Vector3<float> color) noexcept;
	void SetVerticalOffset(float offset) noexcept;
	void AddTextInput(struct TextInput *ti) noexcept;

	//Text IO
	static void StartTextEdit() noexcept;
	static void StopTextEdit() noexcept;

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


struct AC_API UIButton final {
	Vector3<float> background, foreground;
	Vector2F startPosBG, endPosBG, textPos;
	std::u16string text;

	float scale = 1;

	bool selected = false;

public:
	inline bool onClick(Vector2F pos) {
		if(pos > startPosBG && pos < endPosBG)
			selected = true;
		else
			selected = false;
		return selected;
	}

	inline bool onMove(Vector2F pos) {
		bool oldsel = selected;
		onClick(pos);

		return selected != oldsel;
	}

	inline void render(UIHelper &helper) {
		if(selected) {
			helper.AddColoredRect(startPosBG, endPosBG, foreground);
			helper.AddText(textPos, text, scale, background);
		} else {
			helper.AddColoredRect(startPosBG, endPosBG, background);
			helper.AddText(textPos, text, scale, foreground);
		}
	}
};

struct AC_API UITextInput final {
	Vector3<float> background, foreground;//Colors
	Vector2F startPosBG, endPosBG;
	Vector2I32 pixelSize;
	std::u16string text;

	//     view start pos(abs) cursor pos(rel to windowOffset)
	size_t windowOffset = 0,   cursorOffset = 0;

	float scale = 1;

	int selectionOffset = 0; //relative to cursor pos; Selection for copying/replacing

	bool selected = false;

public:
	inline bool onClick(Vector2F pos) {
		if(pos > startPosBG && pos < endPosBG)
			selected = true;
		else
			selected = false;
		return selected;
	}

	void render(UIHelper &helper);
};
