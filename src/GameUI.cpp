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

		helper.SetVerticalOffset(1);
		std::u16string text = UIHelper::ASCIIToU16("Works?\nWORKS!");
//		Vector2F textSize = helper.GetTextSize(text, 4) / -4;
		Vector2F textSize(100, 100);
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
