#include "Dialog.h"


Dialog::Dialog(bool canMove) {
	movable = canMove;
}

Dialog::~Dialog() {

	controls.clear();
}

#include "../Controls/GUIFactory.h"
void Dialog::initialize(ComPtr<ID3D11ShaderResourceView> pixelTexture,
	const pugi::char_t* font) {


	frame.reset(new RectangleFrame(pixelTexture));
	bgSprite.reset(new RectangleSprite(pixelTexture));
	titleSprite.reset(new RectangleSprite(pixelTexture));
	titleSprite->setTint(Color(128, 128, 128));
	buttonFrameSprite.reset(new RectangleSprite(pixelTexture));
	buttonFrameSprite->setTint(Color(128, 128, 128));
	hitArea.reset(new HitArea(Vector2::Zero, Vector2::Zero));

	controls.resize(5);

	unique_ptr<GUIControl> titleText;

	unique_ptr<GUIControl> dialogText;
	dialogText.reset(new TextLabel(guiFactory->getFont(font)));
	dialogText->setTint(Color(0, 0, 0));
	controls[DialogText] = move(dialogText);

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

		//setTitleAreaDimensions(newSize);
		setDimensions(newPos, newSize);
		// not sure if this is necessary, but it was a fun excersize :O
	}
	Vector2 titlePos = Vector2(
		titleFramePosition.x + (titleFrameSize.x - titlesize.x) / 2,
		titleFramePosition.y + (titleFrameSize.y - titlesize.y) / 2);
	controls[TitleText]->setPosition(titlePos);
}


void Dialog::setTitle(wstring text, const Vector2& scale, const pugi::char_t* font) {

	controls[TitleText].release();
	controls[TitleText].reset(new TextLabel(guiFactory->getFont(font)));
	controls[TitleText]->setText(text);
	controls[TitleText]->setScale(scale);
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

void Dialog::setTitleAreaDimensions(const Vector2& newSize) {

	titleFrameSize = newSize;
	titleSprite->setDimensions(titleFramePosition, titleFrameSize);
}

void Dialog::setConfirmButton(unique_ptr<Button> okButton,
	bool autoPosition, bool autoSize) {

	if (autoPosition) {
		okButtonPosition.x = position.x + buttonMargin;
		if (calculateButtonPosition(okButtonPosition))
			okButtonPosition.y -= okButton->getHeight() / 2;
	} else {
		okButtonPosition = okButton->getPosition();
	}

	if (autoSize) {
		okButton->setDimensions(okButtonPosition, standardButtonSize, 3);
	} else {

		okButton->setPosition(okButtonPosition);
	}

	controls[ButtonOK].release();
	controls[ButtonOK] = move(okButton);
}


void Dialog::setConfirmButton(wstring text, const pugi::char_t* font) {

	unique_ptr<Button> okButton;
	okButton.reset(guiFactory->createButton(font));
	okButton->setDimensions(okButtonPosition, standardButtonSize, 3);

	controls[ButtonOK].release();
	controls[ButtonOK] = move(okButton);
	controls[ButtonOK]->setText(text);
	okButtonPosition.x = position.x + buttonMargin;
	if (calculateButtonPosition(okButtonPosition))
		okButtonPosition.y -= controls[ButtonOK]->getHeight() / 2;
	controls[ButtonOK]->setPosition(okButtonPosition);
}

void Dialog::setConfirmOnClickListener(Button::OnClickListener* iOnClickListener) {

	if (controls[ButtonOK].get() == NULL) {
		setConfirmButton(L"OK");
	}

	((Button*) controls[ButtonOK].get())->setOnClickListener(iOnClickListener);
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
	cancelButton.reset(guiFactory->createButton(font));
	cancelButton->setDimensions(cancelButtonPosition, standardButtonSize, 3);
	cancelButton->setOnClickListener(new OnClickListenerCancelButton(this));
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

void Dialog::setCancelOnClickListener(Button::OnClickListener* iOnClickListener) {

	if (controls[ButtonCancel].get() == NULL) {
		setCancelButton(L"OK");
	}

	((Button*) controls[ButtonCancel].get())->setOnClickListener(iOnClickListener);
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

	if (!isOpen)
		return;

	result = NONE;

	if (movable) {
		if ((isHover = titleSprite->getHitArea()->contains(mouse->getPosition()))) {

			if (mouse->pressed()) {

				pressedPosition = mouse->getPosition() - position;

				wostringstream ws;
				ws << mouse->getPosition().x << "," << mouse->getPosition().y << "\n";
				ws << pressedPosition.x << "," << pressedPosition.y << "\n";
				OutputDebugString(ws.str().c_str());

				isSelected = true;
			}

		}

		if (isSelected) {
			setPosition(mouse->getPosition() - pressedPosition);
		}

		if (!mouse->leftButtonDown())
			isSelected = false;

	}
	for (auto const& control : controls) {
		if (control == NULL)
			continue;
		control->update(deltaTime, mouse);
		/*if (control->clicked()) {
			switch (control->action) {
				case GUIControl::OK:
					result = CONFIRM;
					break;
				case GUIControl::CANCEL:
					result = ClickAction::CANCEL;
					break;
			}
		}*/
	}
}


void Dialog::draw(SpriteBatch* batch) {

	if (!isOpen)
		return;

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


void Dialog::setFont(const pugi::char_t* fontName) {

	controls[DialogText]->setFont(fontName);
}

void Dialog::setTint(const Color& color) {
	bgSprite->setTint(color);
}

const Vector2& Dialog::getPosition() const {

	return position;
}

const int Dialog::getWidth() const {
	return size.x;
}

const int Dialog::getHeight() const {
	return size.y;
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

void Dialog::setPosition(Vector2& newPosition) {

	if (newPosition.x < 0)
		newPosition.x = 0;
	if (newPosition.y < 0)
		newPosition.y = 0;
	if (newPosition.x + size.x > Globals::WINDOW_WIDTH)
		newPosition.x = Globals::WINDOW_WIDTH - size.x;
	if (newPosition.y + size.y > Globals::WINDOW_HEIGHT)
		newPosition.y = Globals::WINDOW_HEIGHT - size.y;
	Vector2 moveBy = newPosition - position;
	GUIControl::setPosition(newPosition);
	frame->moveBy(moveBy);
	bgSprite->moveBy(moveBy);
	titleSprite->moveBy(moveBy);
	buttonFrameSprite->moveBy(moveBy);

	for (auto const& control : controls) {
		if (control == NULL)
			continue;
		control->moveBy(moveBy);
	}
}

/** Used for dragging dialog around, if draggable set. */
void Dialog::movePosition(const Vector2& moveBy) {


	frame->moveBy(moveBy);
	bgSprite->moveBy(moveBy);
	titleSprite->moveBy(moveBy);
	buttonFrameSprite->moveBy(moveBy);

	for (auto const& control : controls) {
		if (control == NULL)
			continue;
		control->moveBy(moveBy);
	}
	GUIControl::moveBy(moveBy);
}


void Dialog::OnClickListenerCancelButton::onClick(Button* button) {

	dialog->close();
}
