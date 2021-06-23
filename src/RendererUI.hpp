#pragma once

#include "UI.hpp"

#include <vector>
#include <memory>

#include <GL/glew.h>

class RenderState;

enum ModKeysMask {
	SHIFT = 0x01,
	META = 0x02,
	CTRL = 0x04,
	ALT = 0x08,
};
enum Layer {
	GLOBAL_OVERLAY = 0,//Render always

	WORLD_HUD,//Render over world. Will not rendered when MENU_O* presented

	MENU_WORLD,//Same as WORLD_OVERLAY, but with mouse ungrab
	//For menu with current world on backgound. E.g. chat input

	MENU_ONLY//Stops world rendering, skips lower layer of menu
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

class RendererUI final {
	enum buffers {
		BUFVERTS = 0,
		BUFELEMENTS,
		BUFCOUNT
	};
	struct LayerStore final {
		std::shared_ptr<Menu> menu;
		Layer type;
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
	AC_API void PushLayer(std::shared_ptr<Menu> menu, Layer type, bool alwaysUpdate = false);
	//Remove UI layer
	AC_API void PopLayer() noexcept;
	//Mark geometry ditry
	AC_API void Redraw() noexcept;

	AC_INTERNAL void PushEvent(IOEvent ev) noexcept;
	AC_INTERNAL void Render(RenderState &state) noexcept;
	AC_INTERNAL void PrepareRender(RenderState &state) noexcept;
	AC_INTERNAL GLuint getVAO() noexcept;
};
