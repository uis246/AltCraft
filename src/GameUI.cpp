#include "GameUI.hpp"
#include "RendererUI.hpp"

#include "Vector.hpp"

//vec2 - position
//vec4 - color multiplier
//vec3 - UvLayer

namespace GameUI {
	void MainScreen::onEvent(struct IOState *state, void*) noexcept {
	}
	void MainScreen::renderUpdate(struct RenderBuffer *buf, void*) noexcept {
		//Test code
		//Vert 0
		//Position
		buf->buffer.push_back(-1);
		buf->buffer.push_back(-1);

		//Color
		buf->buffer.push_back(1);
		buf->buffer.push_back(0);
		buf->buffer.push_back(1);
		buf->buffer.push_back(0.5f);

		//Uv
		buf->buffer.push_back(0);
		buf->buffer.push_back(0);
		buf->buffer.push_back(0);

		//Vert 1
		//Position
		buf->buffer.push_back(1);
		buf->buffer.push_back(-1);

		//Color
		buf->buffer.push_back(1);
		buf->buffer.push_back(0);
		buf->buffer.push_back(0);
		buf->buffer.push_back(0.5f);

		//Uv
		buf->buffer.push_back(1);
		buf->buffer.push_back(0);
		buf->buffer.push_back(0);

		//Vert 2
		//Position
		buf->buffer.push_back(-1);
		buf->buffer.push_back(1);

		//Color
		buf->buffer.push_back(0);
		buf->buffer.push_back(1);
		buf->buffer.push_back(0);
		buf->buffer.push_back(0.5f);

		//Uv
		buf->buffer.push_back(0);
		buf->buffer.push_back(1);
		buf->buffer.push_back(0);

		//Indexes
		buf->index.push_back(0);
		buf->index.push_back(1);
		buf->index.push_back(2);
	}



	void GameOverlay::onEvent(struct IOState *state, void*) noexcept {

	}
	void GameOverlay::renderUpdate(struct RenderBuffer *buf, void*) noexcept {

	}



	void Inventory::onEvent(struct IOState *state, void*) noexcept {

	}
	void Inventory::renderUpdate(struct RenderBuffer *buf, void*) noexcept {

	}
}
