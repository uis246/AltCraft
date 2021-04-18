#pragma once

#include "Platform.hpp"

#include <vector>

#include <GL/glew.h>

class RenderState;

enum MouseButtons {
	LEFT = 0,
	RIGHT,
	MIDDLE,
	AUX1,
	AUX2,
	BUTTONCOUNT
};
enum ModKeysMask {
	SHIFT = 0x01,
	META = 0x02,
	CTRL = 0x04,
	ALT = 0x08,
};
enum Layer {
	GLOBAL_OVERLAY = 0,
	//Render always

	WORLD_OVERLAY,//Render over world. Will not rendered when MENU_O* presented
	//For HUD

	MENU_WORLD,//Same as WORLD_OVERLAY, but with mouse ungrab
	//For menu with current world on backgound

	MENU_OVERLAY,//Stops world rendering, doesn't skip lower layer of menu
	//For menu without world rendering

	//NOTE: rename or merge with WORLD_OVERLAY to just OVERLAY
	MENU_ONLY//Stops world rendering, skips lower layer of menu

	//TODO: skip MENU_OVERLAY when no MENU_ONLY or MENU_WORLD
};

struct AC_API IOState {
	size_t pointercnt;
	struct
	{
		float x, y;
		float relativeX, relativeY;
		float pressure;
	} *pointers;
	bool mouse[BUTTONCOUNT];
	uint8_t modifierKeys;
};

struct AC_API RenderBuffer {//TODO
	const RenderState *renderState;
	std::vector<GLushort> index;
	std::vector<GLfloat> buffer;
	//vec2 - position
	//vec3 - UvLayer
	//vec4 - color multiplier
	//9 floats = 36 bytes
};

struct AC_API LayerInfo {
	//Handle buttons, gamepad, other iteraction with player
	void (*onEvent)(struct IOState *state, void *custom);
	void (*onTextInput)(char *string, size_t stringlen, void *custom);
	//Update geometry
	void (*renderUpdate)(struct RenderBuffer *buf, void *custom);
	enum Layer layer;
};

class RendererUI final {
	enum buffers {
		BUFVERTS = 0,
		BUFELEMENTS,
		BUFCOUNT
	};
	struct LayerStore final {
		struct LayerInfo info;
		void *argument;
		bool permadirt;
	};
	//Some GL info
	GLuint VAO, VBOs[BUFCOUNT];
	GLsizei elements;
	GLenum elementType;

	//Layers
	std::vector<LayerStore> layers;

	//Redraw info
	bool dirtry = true;
	bool permanentDirty = false;
	size_t min = 0, max = 0;
	enum Layer currentLayer = GLOBAL_OVERLAY;

	AC_INTERNAL void UpdateRenderInfo() noexcept;
public:
	RendererUI();
	~RendererUI();

	//Add UI layer
	AC_API void PushLayer(struct LayerInfo &layer, void *customArg = nullptr, bool alwaysUpdate = false);
	//Remove UI layer
	AC_API void PopLayer() noexcept;
	//Mark geometry ditry
	AC_API void Redraw() noexcept;
	AC_INTERNAL void Render(RenderState &state) noexcept;
	AC_INTERNAL void PrepareRender(RenderState &state) noexcept;
	AC_INTERNAL GLuint getVAO() noexcept;
};
