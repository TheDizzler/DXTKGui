#include "GUIOverlay.h"

const int TEXT_MARGIN = 5;
const int TEST_BOX_MARGIN = 16;
#include "../Engine/GameEngine.h"
GUIOverlay::GUIOverlay() {

	fpsLabel.reset(guiFactory->createTextLabel(Vector2(Globals::WINDOW_WIDTH - 250, 20),
		L"", "Default Font", true));
	fpsLabel->setTint(Colors::Black);
	fpsLabel->setScale(Vector2(.5, .5));
	fpsLabel->setLayerDepth(1);
}


GUIOverlay::~GUIOverlay() {

	fpsLabel.reset();
	lostJoyDialogs.clear();
	displayingLostJoys.clear();
}


int frameCount = 0;
double fpsUpdateTime = 5;
const double FPS_UPDATE_TIME = 1;
void GUIOverlay::update(double deltaTime) {

	fpsUpdateTime += deltaTime;
	++frameCount;
	if (fpsUpdateTime >= FPS_UPDATE_TIME) {

		wostringstream wss;
		wss << "frameCount: " << frameCount << " fpsUpdateTime: " << fpsUpdateTime << endl;
		wss << "fps: " << frameCount / fpsUpdateTime;
		fpsLabel->setText(wss.str());

		fpsUpdateTime = 0;
		frameCount = 0;
	}


	for (const auto& dialog : lostJoyDialogs) {
		dialog->update(deltaTime);
	}


}

void GUIOverlay::draw(SpriteBatch* batch) {


	for (const auto& dialog : lostJoyDialogs)
		dialog->draw(batch);


	fpsLabel->draw(batch);
}

void GUIOverlay::setDialogText(USHORT playerSlotNumber, wstring text) {


}

#include "../DXTKGui/StringHelper.h"
void GUIOverlay::reportLostJoystick(size_t playerSlotNumber) {

	//displayingLostJoys.push_back(joystickSocket);
	//shared_ptr<Joystick> lostJoy = joystickPorts[joystickSocket];

	//unique_ptr<ControllerDialog> joyLostDialog;
	//size_t numDialogs = lostJoyDialogs.size();
	//Vector2 dialogPos, dialogSize;
	//if (numDialogs <= 0)
	//	dialogSize = Vector2(Globals::WINDOW_WIDTH / 4, Globals::WINDOW_HEIGHT / 4);
	//else
	//	dialogSize = Vector2(lostJoyDialogs[0]->getWidth(), lostJoyDialogs[0]->getHeight());


	//dialogPos = Vector2(Globals::WINDOW_WIDTH / 2, Globals::WINDOW_HEIGHT / 2);
	//dialogPos.y -= dialogSize.y / 2;

	//if (numDialogs <= 0) {
	//	dialogPos.x -= dialogSize.x / 2;
	//} else {
	//	for (const auto& dialog : lostJoyDialogs)
	//		dialog->moveBy(Vector2(-dialogSize.x / 2, 0));
	//	dialogPos.x += (dialogSize.x / 2) * (numDialogs - 1);
	//}


	//joyLostDialog = make_unique<ControllerDialog>(guiFactory.get());
	//joyLostDialog->setDimensions(dialogPos, dialogSize);
	//wostringstream title;
	//title << L"Player " << lostJoy->socket;
	//title << L"  has dropped." << endl;
	//joyLostDialog->setTitle(title.str(), Vector2(1.2, 1.2));
	//wostringstream wss;
	////wss << StringHelper::convertCharStarToWCharT(lostJoy->pc->name.c_str()) << endl;
	//wss << L"Waiting for controller...\n";
	//joyLostDialog->setText(wss.str());
	//joyLostDialog->show();

	//lostJoyDialogs.push_back(move(joyLostDialog));
}








ControllerDialog::ControllerDialog(GUIFactory* guiF)
	: PromptDialog(guiF, guiF->getMouseController(), guiF->getHWND(), false, true) {

	initialize();

}


void ControllerDialog::setDimensions(const Vector2& position, const Vector2& size,
	const int frameThickness) {

	PromptDialog::setDimensions(position, size, frameThickness);
}


void ControllerDialog::update(double deltaTime) {

	if (!isShowing)
		return;


	dialogOpenTime += deltaTime;
	if (dialogOpenTime > CONTROLLER_WAIT_TIME) {
		dialogOpenTime = 0;
		if (ellipsisii++ > 5) {
			ellipsisii = 0;
			setText(defaultText);
		} else {
			wstring text = dialogText->getText();
			text += L".";
			PromptDialog::setText(text);
		}
	}
	PromptDialog::update(deltaTime);
}

void ControllerDialog::setText(wstring text) {

	defaultText = text;
	PromptDialog::setText(text);
}
