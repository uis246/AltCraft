#include "GameUI.hpp"
#include "RendererUI.hpp"
#include "UIHelper.hpp"

#include "Vector.hpp"

namespace GameUI {
	void MainScreen::onEvent(struct IOState *state, void*) noexcept {
	}
	void MainScreen::renderUpdate(struct RenderBuffer *buf, void*) noexcept {
		UIHelper helper(buf);
		helper.AddColoredRect(Vector2F(-0.9, -0.9), Vector2F(0.9, 0.9), Vector3<float>(0.9, 0.9, 0));

		helper.SetVerticalOffset(1.5);
		std::u16string text = UIHelper::ASCIIToU16("Works? WORKS!");
		Vector2F textSize = helper.GetTextSize(text, 1) / -2;
//		Vector2F textSize(50, 0);
		//border=input=sepa=name=border
		helper.AddText(helper.GetCoord(UIHelper::CENTER, textSize), text, 1, Vector3<float>(1, 1, 1));
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
