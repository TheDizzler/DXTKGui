#include "Dialog.h"


Dialog::Dialog(HWND h, bool canMove) {
	hwnd = h;
	movable = canMove;
}

Dialog::~Dialog() {

	controls.clear();

	if (openTransition != NULL)
		delete openTransition;
	if (closeTransition != NULL)
		delete closeTransition;
}

#include "GUIFactory.h"
void Dialog::initialize(GraphicsAsset* pixelAsset,
	const pugi::char_t* font) {

	panel.reset(guiFactory->createPanel());
	panel->setTint(Color(0, 1, 1, 1));
	frame.reset(new RectangleFrame(pixelAsset));
	bgSprite.reset(new RectangleSprite(pixelAsset));
	titleSprite.reset(new RectangleSprite(pixelAsset));
	titleSprite->setTint(Color(1, 1, 1, 1));
	buttonFrameSprite.reset(new RectangleSprite(pixelAsset));
	buttonFrameSprite->setTint(Color(1, 1, 1, 1));
	hitArea.reset(new HitArea(Vector2::Zero, Vector2::Zero));

	controls.resize(5);

	unique_ptr<GUIControl> titleText;

	dialogText.reset(new TextLabel(guiFactory->getFont(font)));
	dialogText->setTint(Color(0, 0, 0, 1));


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

	panel->setDimensions(dialogFramePosition, dialogFrameSize);

	if (controls[TitleText] != NULL) {
		Vector2 titlesize = controls[TitleText]->measureString();
		if (titlesize.x > 0) {
			calculateTitlePos();
		}
	}

	calculateDialogTextPos();
}

void Dialog::setOpenTransition(TransitionEffects::TransitionEffect* effect) {
	if (openTransition != NULL)
		delete openTransition;
	else {
		runTransition = &TransitionEffects::TransitionEffect::run;
		resetTransition = &TransitionEffects::TransitionEffect::reset;
		drawTransition = &TransitionEffects::TransitionEffect::draw;
	}
	openTransition = effect;
}

void Dialog::setCloseTransition(TransitionEffects::TransitionEffect* effect) {

	if (closeTransition != NULL)
		delete closeTransition;
	else {
		runTransition = &TransitionEffects::TransitionEffect::run;
		resetTransition = &TransitionEffects::TransitionEffect::reset;
		drawTransition = &TransitionEffects::TransitionEffect::draw;
	}
	closeTransition = effect;
}


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

void Dialog::setTitle(wstring text, const Vector2& scale,
	const pugi::char_t* font, Color color) {

	controls[TitleText].release();
	controls[TitleText].reset(new TextLabel(guiFactory->getFont(font)));
	controls[TitleText]->setText(text);
	controls[TitleText]->setScale(scale);
	controls[TitleText]->setTint(color);
	calculateTitlePos();
}

void Dialog::calculateDialogTextPos() {

	Vector2 dialogtextsize = dialogText->measureString();
	if (dialogtextsize.x <= 0)
		return;

	TextLabel formattedText(Vector2::Zero, dialogText->getText(), dialogText->getFont());
	int scrollBarBuffer = 0;
	if (dialogtextsize.x + dialogTextMargin.x * 2 > dialogFrameSize.x) {
	// if the text is longer than the dialog box
	//		break the text down into multiple lines
		wstring newText = L"";

		int i = 0;
		int textLength = wcslen(dialogText->getText());
		bool scrollbarAdded = false;
		bool done = false;
		while (i < textLength) {
			wstring currentLine = L"";
			while (dialogText->measureString(currentLine).x + (dialogTextMargin.x * 2)
				< dialogFrameSize.x - scrollBarBuffer) {

				currentLine += dialogText->getText()[i++];
				if (i >= textLength) {
					done = true;
					break;
				}
			}
			if (!done) {
				wchar_t ch = currentLine[currentLine.length() - 1];
				int back = 0;
				while (!isspace(ch)) {

					++back;
					--i;
					ch = currentLine[currentLine.length() - back - 1];
				}
				currentLine.erase(currentLine.end() - back, currentLine.end());
			}

			newText += currentLine + L"\n";

			// If text is getting too long, restart and adjust for scrollbar
			if (!scrollbarAdded
				&& dialogText->measureString(newText).y + dialogTextMargin.y * 2
				> dialogFrameSize.y) {
				scrollBarBuffer = panel->getScrollBarSize().x;
				i = 0;
				newText = L"";
				scrollbarAdded = true;
				done = false;
			}
		}

		formattedText.setText(newText);
		dialogtextsize = formattedText.measureString();
	}

	Vector2 dialogpos =
		Vector2(dialogFramePosition.x +
		(dialogFrameSize.x - dialogtextsize.x - scrollBarBuffer) / 2, 0);

	if (dialogtextsize.y < dialogFrameSize.y)
		dialogpos.y = dialogFramePosition.y + (dialogFrameSize.y - dialogtextsize.y) / 2;
	else
		dialogpos.y = dialogFramePosition.y;
	dialogText->setPosition(dialogpos);
	formattedText.setPosition(dialogpos);

	panel->setDimensions(dialogFramePosition, dialogFrameSize);
	panel->setTexture(
		guiFactory->createTextureFromIElement2D(
			&formattedText, panel->getTint()));

	panel->setTexturePosition(dialogpos);
}

void Dialog::setText(wstring text) {

	dialogText->setText(text);
	calculateDialogTextPos();
}

void Dialog::setTitleAreaDimensions(const Vector2& newSize) {

	titleFrameSize = newSize;
	titleSprite->setDimensions(titleFramePosition, titleFrameSize);
}

void Dialog::setConfirmButton(unique_ptr<Button> okButton,
	bool autoPosition, bool autoSize) {

	if (autoSize)
		okButton->setDimensions(okButtonPosition, standardButtonSize, 3);

	if (autoPosition) {
		okButtonPosition.x = position.x + buttonMargin;
		if (calculateButtonPosition(okButtonPosition))
			okButtonPosition.y -= okButton->getHeight() / 2;
	} else {
		okButtonPosition = okButton->getPosition();
	}


	okButton->setPosition(okButtonPosition);

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

void Dialog::setCancelButton(unique_ptr<Button> cancelButton,
	bool autoPosition, bool autoSize) {

	if (autoPosition) {
		cancelButtonPosition.x =
			position.x + size.x - cancelButton->getWidth() - buttonMargin;
		if (calculateButtonPosition(cancelButtonPosition))
			cancelButtonPosition.y -= cancelButton->getHeight() / 2;
	} else {
		cancelButtonPosition = cancelButton->getPosition();
	}
	if (autoSize) {
		cancelButton->setDimensions(cancelButtonPosition, standardButtonSize, 3);
	} else
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
		buttonPos.y = buttonFramePosition.y + buttonFrameSize.y / 2;

	return true;
}


void Dialog::update(double deltaTime) {

	if (!isOpen)
		return;

	if (isOpening) {
		isOpening = !(openTransition->*runTransition)(deltaTime, this);
	} else if (isClosing) {
		isClosing = !(closeTransition->*runTransition)(deltaTime, this);
		isOpen = isClosing;
		if (!isOpen)
			(closeTransition->*resetTransition)(this);
	}

	if (movable && !isOpening && !isClosing) {
		if ((isHover = titleSprite->getHitArea()->contains(mouse->getPosition()))) {

			if (mouse->pressed()) {
				pressedPosition = mouse->getPosition() - position;
				isPressed = true;
			}
		}

		if (!mouse->leftButton()) {
			isPressed = false;
			pressedPosition = Vector2::Zero;
		}

		if (isPressed) {
			setDraggedPosition(mouse->getPosition() - pressedPosition);
		}



	}
	panel->update(deltaTime);
	for (auto const& control : controls) {
		if (control == NULL)
			continue;
		control->update(deltaTime);

	}
}


void Dialog::draw(SpriteBatch* batch) {

	if (!isOpen)
		return;

	if (isOpening && (openTransition->*drawTransition)(batch)) {
		//OutputDebugString(L"opening\n");
	} else if (isClosing && (closeTransition->*drawTransition)(batch)) {
		//OutputDebugString(L"Closing\n");
	} else {
		bgSprite->draw(batch);
		panel->draw(batch);
		titleSprite->draw(batch);
		buttonFrameSprite->draw(batch);

		for (auto const& control : controls) {
			if (control == NULL)
				continue;
			control->draw(batch);
		}


		frame->draw(batch);
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
const Vector2& XM_CALLCONV Dialog::measureString() const {
	return Vector2::Zero;
}


void Dialog::open() {
	isOpen = true;
	isClosing = false;
	if (openTransition != NULL) {
		isOpening = true;
		(openTransition->*resetTransition)(this);
	}
}

void Dialog::close() {

	isOpening = false;
	if (closeTransition != NULL) {
		if (isClosing) {
			open();
		} else {
			isClosing = true;
			(closeTransition->*resetTransition)(this);
		}
	} else
		isOpen = false;
}


void Dialog::setFont(const pugi::char_t* fontName) {

	dialogText->setFont(fontName);
	calculateDialogTextPos();
}

void Dialog::setTextTint(const XMFLOAT4 color) {

	dialogText->setTint(color);
	calculateDialogTextPos();
}

void Dialog::setTint(const XMFLOAT4 color) {
	bgSprite->setTint(color);
	panel->setTint(color);
	calculateDialogTextPos();
}

void Dialog::setScale(const Vector2& newScale) {

	GUIControl::setScale(newScale);
	frame->setScale(newScale);
	bgSprite->setScale(newScale);
	titleSprite->setScale(newScale);
	buttonFrameSprite->setScale(newScale);
	panel->setScale(newScale);

	for (auto const& control : controls) {
		if (control == NULL)
			continue;
		control->setScale(newScale);
	}
}

void Dialog::setPosition(const Vector2& newPosition) {

	Vector2 moveBy = newPosition - position;
	GUIControl::setPosition(newPosition);
	frame->moveBy(moveBy);
	bgSprite->moveBy(moveBy);
	titleSprite->moveBy(moveBy);
	buttonFrameSprite->moveBy(moveBy);
	panel->moveBy(moveBy);

	for (auto const& control : controls) {
		if (control == NULL)
			continue;
		control->moveBy(moveBy);
	}
}

const Color& Dialog::getPanelTint() const {
	return panel->getTint();
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

const vector<IElement2D*> Dialog::getElements() const {

	vector<IElement2D*> elements;
	elements.push_back(frame.get());
	elements.push_back(bgSprite.get());
	elements.push_back(titleSprite.get());
	elements.push_back(buttonFrameSprite.get());
	elements.push_back(panel.get());

	for (auto const& control : controls) {
		if (control == NULL)
			continue;
		elements.push_back(control.get());
	}
	return elements;
}

bool Dialog::clicked() {
	return isClicked;
}

bool Dialog::pressed() {
	return isPressed;
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

void Dialog::setDraggedPosition(Vector2& newPosition) {

	RECT rect;
	GetClientRect(hwnd, &rect);

	int screenWidth = rect.right - rect.left;
	int screenHeight = rect.bottom - rect.top;


	if (newPosition.x < 0) {
		pressedPosition.x += newPosition.x;
		newPosition.x = 0;
	} else if (newPosition.x + size.x > screenWidth) {
		pressedPosition.x += newPosition.x + size.x - screenWidth;
		newPosition.x = screenWidth - size.x;
	}
	if (newPosition.y < 0) {
		pressedPosition.y += newPosition.y;
		newPosition.y = 0;
	} else if (newPosition.y + size.y > screenHeight) {
		pressedPosition.y += newPosition.y + size.y - screenHeight;
		newPosition.y = screenHeight - size.y;
	}

	Vector2 moveBy = newPosition - position;
	GUIControl::setPosition(newPosition);
	frame->moveBy(moveBy);
	bgSprite->moveBy(moveBy);
	titleSprite->moveBy(moveBy);
	buttonFrameSprite->moveBy(moveBy);
	panel->moveBy(moveBy);

	for (auto const& control : controls) {
		if (control == NULL)
			continue;
		control->moveBy(moveBy);
	}
}

/** NOT USED. */
void Dialog::movePosition(const Vector2& moveBy) {


	frame->moveBy(moveBy);
	bgSprite->moveBy(moveBy);
	titleSprite->moveBy(moveBy);
	buttonFrameSprite->moveBy(moveBy);
	panel->moveBy(moveBy);

	for (auto const& control : controls) {
		if (control == NULL)
			continue;
		control->moveBy(moveBy);
	}
	GUIControl::moveBy(moveBy);
}

void Dialog::setScrollBar(ScrollBarDesc& scrollBarDesc) {
	panel->setScrollBar(scrollBarDesc);
}

void Dialog::alwaysShowScrollBar(bool alwaysShow) {
	panel->alwaysShowScrollBar(alwaysShow);
}


void Dialog::OnClickListenerCancelButton::onClick(Button* button) {

	dialog->close();
}
