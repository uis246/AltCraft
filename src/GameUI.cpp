#include "GameUI.hpp"

#include "UIHelper.hpp"

namespace GameUI {
	void MainScreen::onEvent(struct IOState *state, void*) noexcept {
	}
	void MainScreen::renderUpdate(struct RenderBuffer *buf, void*) noexcept {
		UIHelper helper(buf);
//		helper.SetVerticalOffset(1.5);

		std::u16string addrText = UIHelper::ASCIIToU16("Address");
		std::u16string nameText = UIHelper::ASCIIToU16("Username");
		std::u16string connectText = UIHelper::ASCIIToU16("Connect");
		std::u16string exitText = UIHelper::ASCIIToU16("Exit");

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

		{//Buttons background
			Vector2F A = helper.GetCoord(UIHelper::CENTER, Vector2F(total.x - 4, connectSize.z * 0.5f));
			Vector2F B = helper.GetCoord(UIHelper::CENTER, Vector2F(total.x - (4 + 8 + maxx), connectSize.z * -0.5f));
			//Connect
			helper.AddColoredRect(A, B, Vector3<float>(.33f, .35f, .33f));
			//Exit
			helper.AddColoredRect(-A, -B, Vector3<float>(.33f, .35f, .33f));
		}

		{//Text
			helper.AddText(helper.GetCoord(UIHelper::CENTER, Vector2F(total.x - (8 + (maxx + connectSize.x)/2), connectSize.z * -0.5f)), connectText, 1, Vector3<float>(.74f, .9f, .83f));
			helper.AddText(helper.GetCoord(UIHelper::CENTER, Vector2F(8 + (maxx - exitSize.x)/2 - total.x, exitSize.z * -0.5f)), exitText, 1, Vector3<float>(.74f, .9f, .83f));
		}

		{//Input box
			//Username
			helper.AddTextBox(helper.GetCoord(UIHelper::CENTER, Vector2F(4 - total.x, total.z - (4 + nameSize.z))), Vector2F(16 * 16, nameSize.z), UIHelper::ASCIIToU16("Very, very, VEEEERY LOOOOOONG STRING! LONGER THAN LONG CAT!"), 1, Vector3<float>(.1f, .1f, .1f));
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
