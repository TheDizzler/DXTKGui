#include "Dialog.h"


Dialog::Dialog() {

}

Dialog::~Dialog() {
	/*for (Button* button : buttons)
		delete button;

	for (TextLabel* label : labels)
		delete label;*/

	/*for (GUIControl* control : controls)
		delete control;*/
}

#include "../Managers/GameManager.h"
void Dialog::initialize(unique_ptr<FontSet> font,
	ComPtr<ID3D11ShaderResourceView> pixelTexture) {

	/*if (!Sprite::load(device, Assets::uglyDialogBox))
		return false;

	setScale(Vector2(3, 1.5));*/

	//font = move(fnt);
	font->setTint(DirectX::Colors::White.v);

	frame.reset(new RectangleFrame(pixelTexture));
	bgSprite.reset(new RectangleSprite(pixelTexture));
	titleSprite.reset(new RectangleSprite(pixelTexture));
	titleSprite->setTint(Color(128, 128, 128));
	buttonFrameSprite.reset(new RectangleSprite(pixelTexture));
	buttonFrameSprite->setTint(Color(128, 128, 128));
	hitArea.reset(new HitArea(Vector2::Zero, Vector2::Zero));

	controls.resize(5);

	unique_ptr<GUIControl> titleText;
	//titleText.reset(new TextLabel(GameManager::guiFactory->getFont("Arial")));
	//titleText->setScale(Vector2(1.5, 1.5));
	//titleText->setTint(Color(0, 0, 0));
	//controls[TitleText] = move(titleText);

	unique_ptr<GUIControl> dialogText;
	dialogText.reset(new TextLabel(move(font)));
	dialogText->setTint(Color(0, 0, 0));
	controls[DialogText] = move(dialogText);


	/*
	Vector2 scaleFactor = Vector2(.75, 1);
	TextButton* button = new TextButton();
	if (!button->load(device, fontFile,
		Assets::buttonUpFile, Assets::buttonDownFile))
		return false;
	button->action = Button::OK;
	button->setScale(scaleFactor);
	button->setText("Quit");
	okBtn = Vector2(position.x - width + button->getWidth() / 3,
		position.y + height / 4);
	button->setPosition(okBtn);
	buttons.push_back(button);


	button = new TextButton();
	if (!button->load(device, fontFile,
		Assets::buttonUpFile, Assets::buttonDownFile))
		return false;
	button->action = Button::CANCEL;
	button->setScale(scaleFactor);
	button->setText("Keep Playing!");
	cancelBtn = Vector2(position.x + width - button->getWidth() / 3,
		position.y + height / 4);
	button->setPosition(cancelBtn);
	buttons.push_back(button);*/

}

void Dialog::setDimensions(const Vector2& pos, const Vector2& sz,
	const int frmThcknss) {

	frameThickness = frmThcknss;
	size = sz;
	GUIControl::setPosition(pos);

	bgSprite->setDimensions(position, size);
	frame->setDimensions(position, size, frameThickness);
	titleFrameSize.x = size.x;
	titleFramePosition = Vector2(position.x, position.y);
	titleSprite->setDimensions(titleFramePosition, titleFrameSize);
	dialogFramePosition = Vector2(position.x, position.y + titleFrameSize.y);
	dialogFrameSize = Vector2(size.x, size.y - titleFrameSize.y - buttonFrameSize.y);
	buttonFramePosition =
		Vector2(position.x, dialogFramePosition.y + dialogFrameSize.y);
	buttonFrameSize.x = size.x;
	buttonFrameSprite->setDimensions(buttonFramePosition, buttonFrameSize);

	if (controls[TitleText] != NULL) {
		Vector2 titlesize = controls[TitleText]->measureString();
		if (titlesize.x > 0) {
			calculateTitlePos();
		}
	}

	Vector2 dialogsize = controls[DialogText]->measureString();
	if (dialogsize.x > 0) {
		calculateDialogTextPos();
	}

}

int titleTextMargin = 10;
void Dialog::calculateTitlePos() {

	Vector2 titlesize = controls[TitleText]->measureString();

	if (titlesize.x > titleFrameSize.x
		|| titlesize.y > titleFrameSize.y) { // if title is bigger than dialog box
		//expand the box
		Vector2 newSize = size;
		Vector2 newPos = position;
		if (titlesize.x > titleFrameSize.x) {
			newSize.x = titlesize.x + titleTextMargin;
			newPos.x -= (newSize.x - size.x) / 2;
		}
		if (titlesize.y > titleFrameSize.y) {
			titleFrameSize.y += titleTextMargin;
			// might have to do somethin here
			//newPos.y -= titleTextMargin;
		}


		setDimensions(newPos, newSize);
		// not sure if this is necessary, but it was a fun excersize :O
	}
	Vector2 titlePos = Vector2(
		titleFramePosition.x + (titleFrameSize.x - titlesize.x) / 2,
		titleFramePosition.y + (titleFrameSize.y - titlesize.y) / 2);
	controls[TitleText]->setPosition(titlePos);
}


void Dialog::setTitle(wstring text, const pugi::char_t* font) {

	controls[TitleText].release();
	controls[TitleText].reset(new TextLabel(GameManager::guiFactory->getFont(font)));
	controls[TitleText]->setText(text);
	controls[TitleText]->setScale(Vector2(1.5, 1.5));
	controls[TitleText]->setTint(Color(0, 0, 0));
	calculateTitlePos();
}

void Dialog::calculateDialogTextPos() {

	Vector2 dialogtextsize = controls[DialogText]->measureString();
	if (dialogtextsize.x > dialogFrameSize.x) { // if the text is longer than the dialog box
	//	TODO:
	//		break the text down into multiple lines

	}
	Vector2 dialogpos = Vector2(
		dialogFramePosition.x + (dialogFrameSize.x - dialogtextsize.x) / 2,
		dialogFramePosition.y + (dialogFrameSize.y - dialogtextsize.y) / 2);
	controls[DialogText]->setPosition(dialogpos);
}

void Dialog::setText(wstring text) {

	controls[DialogText]->setText(text);
	calculateDialogTextPos();
}

void Dialog::setConfirmButton(unique_ptr<Button> okButton) {

	okButtonPosition.x = position.x + buttonMargin;
	if (calculateButtonPosition(okButtonPosition))
		okButtonPosition.y -= okButton->getHeight() / 2;
	okButton->setPosition(okButtonPosition);
	controls[ButtonOK].release();
	controls[ButtonOK] = move(okButton);

}

void Dialog::setConfirmButton(wstring text, const pugi::char_t* font) {

	unique_ptr<Button> okButton;
	okButton.reset(GameManager::guiFactory->createButton(font));
	okButton->setDimensions(okButtonPosition, standardButtonSize, 3);
	controls[ButtonOK].release();
	controls[ButtonOK] = move(okButton);
	controls[ButtonOK]->action = ClickAction::OK;
	controls[ButtonOK]->setText(text);
	okButtonPosition.x = position.x + buttonMargin;
	if (calculateButtonPosition(okButtonPosition))
		okButtonPosition.y -= controls[ButtonOK]->getHeight() / 2;
	controls[ButtonOK]->setPosition(okButtonPosition);
}

void Dialog::setCancelButton(unique_ptr<Button> cancelButton) {

	cancelButtonPosition.x =
		position.x + size.x - cancelButton->getWidth() - buttonMargin;
	if (calculateButtonPosition(cancelButtonPosition))
		cancelButtonPosition.y -= cancelButton->getHeight() / 2;
	cancelButton->setPosition(cancelButtonPosition);
	controls[ButtonCancel].release();
	controls[ButtonCancel] = move(cancelButton);
}

void Dialog::setCancelButton(wstring text, const pugi::char_t * font) {

	unique_ptr<Button> cancelButton;
	cancelButton.reset(GameManager::guiFactory->createButton(font));
	cancelButton->setDimensions(cancelButtonPosition, standardButtonSize, 3);
	controls[ButtonCancel].release();
	controls[ButtonCancel] = move(cancelButton);
	controls[ButtonCancel]->action = ClickAction::CANCEL;
	controls[ButtonCancel]->setText(text);
	cancelButtonPosition.x =
		position.x + size.x - controls[ButtonCancel]->getWidth() - buttonMargin;
	if (calculateButtonPosition(cancelButtonPosition))
	cancelButtonPosition.y -= controls[ButtonCancel]->getHeight() / 2;
	controls[ButtonCancel]->setPosition(cancelButtonPosition);
}

bool Dialog::calculateButtonPosition(Vector2& buttonPos) {

	int buttonheight = getMaxButtonHeight();
	if (buttonheight + buttonMargin * 2 > buttonFrameSize.y) {
		// this will shrink the dialog text
		buttonFrameSize.y = buttonheight + buttonMargin * 2;
		setDimensions(position, size, frameThickness);

		// recalculate all button y positions
		if (controls[ButtonOK] != NULL) {
			okButtonPosition.y = buttonFramePosition.y
				+ (buttonFrameSize.y - controls[ButtonOK]->getHeight()) / 2;
			controls[ButtonOK]->setPosition(okButtonPosition);
		}
		if (controls[ButtonCancel] != NULL) {
			cancelButtonPosition.y = buttonFramePosition.y
				+ (buttonFrameSize.y - controls[ButtonCancel]->getHeight()) / 2;
			controls[ButtonCancel]->setPosition(cancelButtonPosition);

		}
		if (controls[ButtonNeutral] != NULL) {
			neutralButtonPosition.y = buttonFramePosition.y
				+ (buttonFrameSize.y - controls[ButtonNeutral]->getHeight()) / 2;
			controls[ButtonNeutral]->setPosition(neutralButtonPosition);
		}
		return false;
	} else
		buttonPos.y = buttonFramePosition.y
		+ buttonFrameSize.y / 2;

	return true;
}




void Dialog::update(double deltaTime, MouseController* mouse) {

	result = NONE;

	for (auto const& control : controls) {
		if (control == NULL)
			continue;
		control->update(deltaTime, mouse);
		if (control->clicked()) {
			switch (control->action) {
				case GUIControl::OK:
					result = CONFIRM;
					break;
				case GUIControl::CANCEL:
					result = ClickAction::CANCEL;
					break;
			}
		}
	}
}


void Dialog::draw(SpriteBatch* batch) {

	bgSprite->draw(batch);
	titleSprite->draw(batch);
	buttonFrameSprite->draw(batch);
	frame->draw(batch);


	for (auto const& control : controls) {
		if (control == NULL)
			continue;
		control->draw(batch);
	}
}



void Dialog::addItem(unique_ptr<GUIControl> control) {

	controls.push_back(move(control));
}

/* Not too sure how this will behave....*/
void Dialog::addItems(vector<unique_ptr<GUIControl>> newControls) {

	for (int i = 0; i < newControls.size(); ++i)
		controls.push_back(move(newControls[i]));
}

/** Not used in DialogBox */
XMVECTOR XM_CALLCONV Dialog::measureString() const {
	return Vector2::Zero;
}

void Dialog::open() {
	isOpen = true;
}

void Dialog::close() {
	isOpen = false;
}

GUIControl::ClickAction Dialog::getResult() {
	return result;
}


void Dialog::setFont(unique_ptr<FontSet> newFont) {

	controls[DialogText]->setFont(move(newFont));
}

void Dialog::setTint(const Color& color) {
	bgSprite->setTint(color);
}

const Vector2& Dialog::getPosition() const {

	return position;
}

const int Dialog::getWidth() const {
	return hitArea->size.x;
}

const int Dialog::getHeight() const {
	return hitArea->size.y;
}

bool Dialog::clicked() {
	return isClicked;
}

bool Dialog::selected() {
	return isSelected;
}

bool Dialog::hovering() {
	return isHover;
}


int Dialog::getMaxButtonHeight() {

	int maxHeight = 0;
	if (controls[ButtonOK] != NULL)
		maxHeight = controls[ButtonOK]->getHeight();

	if (controls[ButtonCancel] != NULL
		&& controls[ButtonCancel]->getHeight() > maxHeight)
		maxHeight = controls[ButtonCancel]->getHeight();

	if (controls[ButtonNeutral] != NULL
		&& controls[ButtonNeutral]->getHeight() > maxHeight)
		maxHeight = controls[ButtonNeutral]->getHeight();

	return maxHeight;
}

/** Used for dragging dialog around, if draggable set. */
void Dialog::setPosition(const Vector2& position) {
}

