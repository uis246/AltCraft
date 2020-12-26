#pragma once

struct IOState;
struct RenderBuffer;

//Implementation of in-game ui
namespace GameUI {
	//Screen after start
	namespace MainScreen {
		void onEvent(struct IOState *state, void *custom) noexcept;
		void renderUpdate(struct RenderBuffer *buf, void *custom) noexcept;
	}

	//HP, selected item, etc
	namespace GameOverlay {
		void onEvent(struct IOState *state, void *custom) noexcept;
		void renderUpdate(struct RenderBuffer *buf, void *custom) noexcept;
	}

	namespace Inventory {
		void onEvent(struct IOState *state, void *custom) noexcept;
		void renderUpdate(struct RenderBuffer *buf, void *custom) noexcept;
	}
}
