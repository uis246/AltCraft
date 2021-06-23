#pragma once

#include <string>

#include "UIHelper.hpp"
#include "UI.hpp"

//Implementation of in-game ui
namespace GameUI {
	//Screen after start
	class MainMenu : public Menu {
		std::string address;
		std::string username;

		struct UIButton connect/*, close*/;
	public:
		MainMenu();

		bool onEvent(struct IOEvent state) noexcept override;
		void renderUpdate(struct RenderBuffer *buf) noexcept override;
	};

	//HP, selected item, etc
	namespace GameOverlay {
		void onEvent(struct IOState *state, void *custom) noexcept;
		void renderUpdate(struct RenderBuffer *buf, void *custom) noexcept;
	}

	namespace PauseMenu {
		void onEvent(struct IOState *state, void *custom) noexcept;
		void renderUpdate(struct RenderBuffer *buf, void *custom) noexcept;
	}

	namespace Inventory {
		void onEvent(struct IOState *state, void *custom) noexcept;
		void renderUpdate(struct RenderBuffer *buf, void *custom) noexcept;
	}

	void initLocalization() noexcept;
}
