#include "GameUI.hpp"

static const std::u16string addrText = UIHelper::ASCIIToU16("Address");
static const std::u16string nameText = UIHelper::ASCIIToU16("Username");
static const std::u16string connectText = UIHelper::ASCIIToU16("Connect");
static const std::u16string exitText = UIHelper::ASCIIToU16("Exit");

static constexpr Vector3<float> a(.33f, .35f, .33f);

namespace GameUI {
	MainMenu::MainMenu() {
		connect.background = a;
		connect.foreground = Vector3<float>(.8f, .8f, .8f);
		connect.text = connectText;
	}
	bool MainMenu::onEvent(struct IOEvent ev) noexcept {
		if(ev.type == IOEvent::MouseMoved) {
			MouseEvent *mev = reinterpret_cast<MouseEvent*>(ev.data);
			return connect.checkMouse(mev->pos);
		}
		return true;
	}
	void MainMenu::renderUpdate(struct RenderBuffer *buf) noexcept {
		UIHelper helper(buf);

		Vector2F addrSize = helper.GetTextSize(addrText, 1);
		Vector2F nameSize = helper.GetTextSize(nameText, 1);
		Vector2F connectSize = helper.GetTextSize(connectText, 1);
		Vector2F exitSize = helper.GetTextSize(exitText, 1);

		float maxx = std::fmaxf(std::fmaxf(addrSize.x, nameSize.x), connectSize.x);
		Vector2F total;
		//4|input|4|name |4
		total.x = 4 + 16 * 16 + 8 + maxx + 4 + 4;
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
			//Background
			Vector2F A = helper.GetCoord(UIHelper::CENTER, Vector2F(total.x - 4, connectSize.z * 0.5f));
			Vector2F B = helper.GetCoord(UIHelper::CENTER, Vector2F(total.x - (4 + 8 + maxx), connectSize.z * -0.5f));
			//Exit
			helper.AddColoredRect(-A, -B, Vector3<float>(.33f, .35f, .33f));

			//Text
			helper.AddText(helper.GetCoord(UIHelper::CENTER, Vector2F(8 + (maxx - exitSize.x)/2 - total.x, exitSize.z * -0.5f)), exitText, 1, Vector3<float>(.8f, .8f, .8f));

			connect.startPosBG = A;
			connect.endPosBG = B;
			connect.textPos = helper.GetCoord(UIHelper::CENTER, Vector2F(total.x - (8 + (maxx + connectSize.x)/2), connectSize.z * -0.5f));

			connect.render(helper);
		}

		{//Input box
			//Username
			helper.AddTextBox(helper.GetCoord(UIHelper::CENTER, Vector2F(4 - total.x, total.z - (4 + nameSize.z))), Vector2F(16 * 16, nameSize.z), UIHelper::UnicodeToU16("Very, very, VEEEERY LOOOOOONG STRING! LONGER THAN LONG CAT"), 1, Vector3<float>(.1f, .1f, .1f));
			//Address
			helper.AddTextBox(helper.GetCoord(UIHelper::CENTER, Vector2F(4, 4) - total), Vector2F(16 * 16, addrSize.z), UIHelper::ASCIIToU16("127.0.0.1"), 1, Vector3<float>(.1f, .1f, .1f));
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
