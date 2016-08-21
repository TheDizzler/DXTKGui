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
	hitArea.reset(new HitArea(Vector2::Zero, Vector2::Zero));

	controls.resize(5);

	unique_ptr<GUIControl> titleText;
	titleText.reset(new TextLabel(GameManager::guiFactory->getFont("Arial")));
	titleText->setScale(Vector2(1.5, 1.5));
	titleText->setTint(Color(0, 0, 0));
	controls[TitleText] = move(titleText);
	unique_ptr<GUIControl> dialogText;
	dialogText.reset(new TextLabel(move(font)));
	dialogText->setTint(Color(0, 0, 0));
	controls[DialogText] = move(dialogText);



	/*unique_ptr<Button> junkOkButton;
	junkOkButton.reset(new Button());
	controls.push_back(move(junkOkButton));

	unique_ptr<Button> junkNeutralButton;
	junkNeutralButton.reset(new Button());
	controls.push_back(move(junkNeutralButton));

	unique_ptr<Button> junkCancelButton;
	junkCancelButton.reset(new Button());
	controls.push_back(move(junkCancelButton));*/


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
	const int frameThickness) {

	//position = pos;
	size = sz;
	/*position.x -= size.x / 2;
	position.y -= size.y / 2;*/
	setPosition(pos);

	bgSprite->setDimensions(position, size);
	frame->setDimensions(position, size, frameThickness);
	titleFrameSize.x = size.x;
	titleFramePosition = Vector2(position.x, position.y);
	titleSprite->setDimensions(titleFramePosition, titleFrameSize);
	dialogFramePosition = Vector2(position.x, position.y + titleFrameSize.y);
	dialogFrameSize = Vector2(size.x, size.y - titleFrameSize.y);
	buttonFramePosition =
		Vector2(position.x, dialogFramePosition.y + dialogFrameSize.y);


	Vector2 titlesize = controls[TitleText]->measureString();
	if (titlesize.x > 0) {
		calculateTitlePos();
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


void Dialog::setTitle(wstring text) {

	controls[TitleText]->setText(text);
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

/** Not used in DialogBox */
XMVECTOR XM_CALLCONV Dialog::measureString() const {
	return Vector2::Zero;
}


void Dialog::addItem(unique_ptr<GUIControl> control) {

	controls.push_back(move(control));
}

/* Not too sure how this will behave....*/
void Dialog::addItems(vector<unique_ptr<GUIControl>> newControls) {

	for (int i = 0; i < newControls.size(); ++i)
		controls.push_back(move(newControls[i]));
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
	frame->draw(batch);


	for (auto const& control : controls) {
		if (control == NULL)
			continue;
		control->draw(batch);
	}
}

void Dialog::setConfirmButton(unique_ptr<Button> okButton) {


	okButton->action = Button::OK;
	okButton->setDimensions(buttonFramePosition, standardButtonSize, 3);
	controls[ButtonOK].release();
	controls[ButtonOK] = move(okButton);

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

	//font.release();
	controls[DialogText]->setFont(move(newFont));
}

void Dialog::setTint(const Color& color) {
	bgSprite->setTint(color);
}

const Vector2& Dialog::getPosition() const {

	return position;
}

const int Dialog::getWidth() const {
	return 0;
}

const int Dialog::getHeight() const {
	return 0;
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



