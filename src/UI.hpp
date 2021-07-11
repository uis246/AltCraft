#pragma once

#include "Platform.hpp"
#include "Vector.hpp"

struct IOState;
struct RenderBuffer;

enum MouseButtons {
	LEFT = 0,
	MIDDLE,
	RIGHT,
	AUX1,
	AUX2,
	BUTTONCOUNT
};

struct AC_API IOEvent {
	union {//8 byte
		void *data;//Depends on type
		struct { uint16_t inlined1, inlined2, inlined3, inlined4; };
		struct { uint32_t inlined32, inlined32_2; };
	};

	enum {
		MouseMoved,//Pointer to MouseEvent
		MouseClicked,//same
		MouseReleased,//same
//		Resize,//Nothing?
		KeyPressed,//USB scancode in inlined1, repeat in inlined2 and modifiers in inlined3
		KeyReleased,//same
		TextInput,//Pointer to null-terminated line
	} type;
};

struct AC_API MouseEvent {//For MouseMoved, MouseClicked, MouseReleased
	Vector2F NDCpos, NDCrelative;
//	Vector2I32 ABSpos;//Right-handed. As in vulkan. With origin in center.
	enum MouseButtons button;//on click/release
};

class AC_API Menu {
public:
//	virtual void reinitLocalization() noexcept;

	//Handle buttons, gamepad, other iteraction with player
	virtual bool onEvent(struct IOEvent event) noexcept = 0;

	//Update geometry
	virtual void renderUpdate(struct RenderBuffer *buf) noexcept = 0;
//	virtual size_t countIndicies(void *custom) noexcept;//Count indicies to be drawn
};
