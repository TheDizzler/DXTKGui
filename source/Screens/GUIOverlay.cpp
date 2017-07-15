#include "GUIOverlay.h"
#include "../Engine/GameEngine.h"
#include "../DXTKGui/StringHelper.h"

const int TEXT_MARGIN = 5;
const int TEST_BOX_MARGIN = 16;

GUIOverlay::GUIOverlay() {

	fpsLabel.reset(guiFactory->createTextLabel(Vector2(Globals::WINDOW_WIDTH - 250, 20),
		L"", "Default Font", true));
	fpsLabel->setTint(Colors::Black);
	fpsLabel->setScale(Vector2(.5, .5));
	fpsLabel->setLayerDepth(1);
}


GUIOverlay::~GUIOverlay() {

	lostJoyDialogs.clear();
	displayingLostJoys.clear();
}

void GUIOverlay::reloadGraphicsAssets() {
	fpsLabel->reloadGraphicsAsset();
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
		fpsLabel->update(deltaTime);
		fpsUpdateTime = 0;
		frameCount = 0;
	}

}

void GUIOverlay::draw(SpriteBatch* batch) {


	fpsLabel->draw(batch);
}




ControllerDialog::ControllerDialog(GUIFactory* guiF)
	: PromptDialog(guiF, guiF->getMouseController(), guiF->getHWND(), false, true) {

	initialize();

}


void ControllerDialog::setDimensions(const Vector2& position, const Vector2& size,
	const int frameThickness) {

	PromptDialog::setDimensions(position, size, frameThickness);
}


bool ControllerDialog::update(double deltaTime) {

	if (!isShowing)
		return false;

	bool refreshed = false;
	dialogOpenTime += deltaTime;
	if (dialogOpenTime > CONTROLLER_WAIT_TIME) {
		dialogOpenTime = 0;
		if (ellipsisii++ > 5) {
			ellipsisii = 0;
			setText(defaultText);
			refreshed = true;
		} else {
			wstring text = dialogText->getText();
			text += L".";
			PromptDialog::setText(text);
			refreshed = true;
		}
	}
	if (PromptDialog::update(deltaTime))
		refreshed = true;

	return refreshed;
}

void ControllerDialog::setText(wstring text) {

	defaultText = text;
	PromptDialog::setText(text);
}
