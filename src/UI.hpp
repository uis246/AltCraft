#pragma once

#include "Platform.hpp"
#include "Vector.hpp"

struct IOState;
struct RenderBuffer;

enum MouseButtons {
	LEFT = 0,
	RIGHT,
	MIDDLE,
	AUX1,
	AUX2,
	BUTTONCOUNT
};

struct AC_API IOEvent {
	void *data;//Depends on type

	enum {
		MouseMoved,
		MouseClicked,
		MouseReleased,
	} type;
};

struct AC_API MouseEvent {//For MouseMoved, MouseClicked, MouseReleased
	Vector2F pos;
	float relativeX, relativeY;
	enum MouseButtons button;//on click/release
};

class AC_API Menu {
public:
//	virtual void initLocalization() noexcept;
//	Menu() = delete;
//	Menu() {}
//	virtual ~Menu() {};

	//Handle buttons, gamepad, other iteraction with player
	virtual bool onEvent(struct IOEvent event) noexcept = 0;

	//Update geometry
	virtual void renderUpdate(struct RenderBuffer *buf) noexcept = 0;
//	virtual size_t countIndicies(void *custom) noexcept;//Count indicies to be drawn
};
