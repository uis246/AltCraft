#include "GameUI.hpp"
#include "RendererUI.hpp"
#include "UIHelper.hpp"

#include "Vector.hpp"

//vec2 - position
//vec3 - UvLayer
//vec4 - color multiplier

namespace GameUI {
	void MainScreen::onEvent(struct IOState *state, void*) noexcept {
	}
	void MainScreen::renderUpdate(struct RenderBuffer *buf, void*) noexcept {
		UIHelper helper(buf);
		helper.AddColoredRect(Vector2F(-0.9, -0.9), Vector2F(0.9, 0.9), Vector3<float>(0.9, 0.9, 0));
		helper.AddText(Vector2F(0), UIHelper::ASCIIToU16("sworld!"), Vector3<float>(1, 1, 1));
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
