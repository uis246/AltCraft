#include "GameUI.hpp"

#include "Event.hpp"

static const std::u16string addrText = UIHelper::ASCIIToU16("Address");
static const std::u16string nameText = UIHelper::ASCIIToU16("Username");
static const std::u16string connectText = UIHelper::ASCIIToU16("Connect");
static const std::u16string exitText = UIHelper::ASCIIToU16("Exit");

static constexpr Vector3<float> buttonBG(.33f, .35f, .33f);
static constexpr Vector3<float> buttonFG(.8f, .8f, .8f);

namespace GameUI {
	MainMenu::MainMenu() {
		connect.background = buttonBG;
		connect.foreground = buttonFG;
		connect.text = connectText;
		connect.scale = scale;

		exit.background = buttonBG;
		exit.foreground = buttonFG;
		exit.text = exitText;
		exit.scale = scale;

		username.background = Vector3<float>(.1f, .1f, .1f);
		username.foreground = buttonFG;
		username.text = UIHelper::ASCIIToU16("test");
		username.cursorOffset = 4;
		username.selectionOffset = 1;
	}
	bool MainMenu::onEvent(struct IOEvent ev) noexcept {
		if(ev.type == IOEvent::MouseMoved) {
			MouseEvent *mev = reinterpret_cast<MouseEvent*>(ev.data);
			return connect.onMove(mev->NDCpos) || exit.onMove(mev->NDCpos) || username.onMove(mev->NDCpos);
		} else if(ev.type == IOEvent::MouseClicked) {
			MouseEvent *mev = reinterpret_cast<MouseEvent*>(ev.data);
			if(exit.onClick(mev->NDCpos)) {
				PUSH_EVENT("Exit", 0);
			} else if(connect.onClick(mev->NDCpos)) {
			} else
				return username.onClick(mev->NDCpos);
			return true;
		} else if(ev.type == IOEvent::MouseReleased) {
			username.onRelease();
		}
		return true;
	}
	void MainMenu::renderUpdate(struct RenderBuffer *buf) noexcept {
		UIHelper helper(buf);

		Vector2F addrSize = helper.GetTextSize(addrText, scale);
		Vector2F nameSize = helper.GetTextSize(nameText, scale);
		Vector2F connectSize = helper.GetTextSize(connectText, scale);
		Vector2F exitSize = helper.GetTextSize(exitText, scale);

		//FIXME: scaling
		float maxx = std::fmaxf(std::fmaxf(addrSize.x, nameSize.x), connectSize.x);
		Vector2F total;
		//4|input|4|name |4
		total.x = (4 + 16 * 16 + 8 + maxx + 4 + 4) * scale;
		total.z = 4 + addrSize.z + 4 + connectSize.z + 4 + nameSize.z + 4;
		total = total * 0.5f;

		{//Draw background
			Vector2F coord = helper.GetCoord(UIHelper::CENTER, total);
			helper.AddColoredRect(coord, -coord, Vector3<float>(0.4, 0.4, 0.4));
		}

		helper.AddText(helper.GetCoord(UIHelper::CENTER, (total - Vector2F(8 + maxx, 4 + nameSize.z))), nameText, 1, Vector3<float>(1, 1, 1));
		helper.AddText(helper.GetCoord(UIHelper::CENTER, (total * Vector2F(1, -1) - Vector2F(8 + maxx, -4))), addrText, 1, Vector3<float>(1, 1, 1));

		{//Will be progress bar
			//4/Exit /progressbar/Connect /4
			Vector2F coord = helper.GetCoord(UIHelper::CENTER, Vector2F(total.x - (4 + 8 + 4 + maxx), connectSize.z * 0.5f));
			helper.AddColoredRect(coord, -coord, Vector3<float>(.085f, .18f, .038f));
		}

		{//Buttons
			Vector2F A = helper.GetCoord(UIHelper::CENTER, Vector2F(total.x - 4, connectSize.z * 0.5f));
			Vector2F B = helper.GetCoord(UIHelper::CENTER, Vector2F(total.x - (4 + 8 + maxx), connectSize.z * -0.5f));

			connect.startPosBG = B;
			connect.endPosBG = A;
			exit.startPosBG = -A;
			exit.endPosBG = -B;

			connect.textPos = helper.GetCoord(UIHelper::CENTER, Vector2F(total.x - (8 + (maxx + connectSize.x)/2), connectSize.z * -0.5f));
			exit.textPos = helper.GetCoord(UIHelper::CENTER, Vector2F(8 + (maxx - exitSize.x)/2 - total.x, exitSize.z * -0.5f));

			connect.render(helper);
			exit.render(helper);
		}

		{//Input box
			username.startPosBG = helper.GetCoord(UIHelper::CENTER, Vector2F(4 - total.x, total.z - (4 + nameSize.z)));
			username.pixelSize = Vector2I32(16 * 16, 16);//FIXME: scaling. again.
			//Username
//			helper.AddTextBox(helper.GetCoord(UIHelper::CENTER, Vector2F(4 - total.x, total.z - (4 + nameSize.z))), Vector2F(16 * 16, nameSize.z), UIHelper::UnicodeToU16("Very, very, VEEEERY LOOOOOONG STRING! LONGER THAN LONG CAT"), 1, Vector3<float>(.1f, .1f, .1f));
			//Address
//			helper.AddTextBox(helper.GetCoord(UIHelper::CENTER, Vector2F(4, 4) - total), Vector2F(16 * 16, addrSize.z), UIHelper::ASCIIToU16("127.0.0.1"), 1, Vector3<float>(.1f, .1f, .1f));

			username.render(helper);
		}
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
