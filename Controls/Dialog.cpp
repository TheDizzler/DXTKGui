#include "Dialog.h"
#include "../GUIFactory.h"

Dialog::Dialog(GUIFactory* factory, MouseController* mouseController)
	: GUIControlBox(factory, mouseController) {
}

Dialog::~Dialog() {

	if (openTransition)
		delete openTransition;
	if (closeTransition)
		delete closeTransition;
}

void Dialog::show() {
	isShowing = true;
	isClosing = false;
	if (openTransition != NULL) {
		isOpening = true;
		(openTransition->*resetTransition)();
	}
}


void Dialog::hide() {

	isOpening = false;
	if (closeTransition != NULL) {
		if (isClosing) {
			show();
		} else {
			isClosing = true;
			(closeTransition->*resetTransition)();
		}
	} else
		isShowing = false;
}

bool Dialog::isOpen() {
	return isShowing;
}

void Dialog::setText(wstring text) {

	dialogText->setText(text);
}


const wchar_t* Dialog::getText() {
	return dialogText->getText();
}

void Dialog::setFont(const pugi::char_t* font) {

	dialogText->setFont(font);
}

void Dialog::setTextTint(const XMFLOAT4 color) {

	dialogText->setTint(color);
}

const Vector2& Dialog::getPosition() const {
	return position;
}

const int Dialog::getWidth() const {
	return (int) size.x;
}

const int Dialog::getHeight() const {
	return (int) size.y;
}


/** Not used in DialogBox */
const Vector2 XM_CALLCONV Dialog::measureString() const {
	return Vector2::Zero;
}


size_t Dialog::addControl(unique_ptr<GUIControl> control) {
	return size_t();
}

void Dialog::addControls(vector<unique_ptr<GUIControl>> controls) {
}

GUIControl* Dialog::getControl(size_t controlPosition) const {
	return nullptr;
}

const vector<IElement2D*> Dialog::getElements() const {
	return vector<IElement2D*>();
}


void Dialog::setOpenTransition(TransitionEffects::TransitionEffect* effect) {

	if (openTransition != NULL)
		delete openTransition;
	else {
		runTransition = &TransitionEffects::TransitionEffect::run;
		resetTransition = &TransitionEffects::TransitionEffect::reset;
		drawTransition = &TransitionEffects::TransitionEffect::draw;
	}

	effect->initializeEffect(this);

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

	effect->initializeEffect(this);

	closeTransition = effect;
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



void Dialog::OnClickListenerCancelButton::onClick(Button* button) {
	dialog->hide();
}

void Dialog::OnClickListenerCancelButton::onPress(Button * button) {
}

void Dialog::OnClickListenerCancelButton::onHover(Button * button) {
}

void Dialog::OnClickListenerCancelButton::resetState(Button * button) {
}




/** ******* PromptDialog START ******** **/
PromptDialog::PromptDialog(GUIFactory* factory,
	MouseController* mouseController, HWND h, bool canMove, bool centerTxt)
	: Dialog(factory, mouseController) {
	hwnd = h;
	movable = canMove;
	centerText = centerTxt;


}

PromptDialog::~PromptDialog() {
	controls.clear();
	texturePanel.reset();
	panel.reset();
}

void PromptDialog::initialize(const pugi::char_t* font) {

	panel.reset(guiFactory->createPanel(false));
	panel->setTint(Color(0, 1, 1, 1));
	frame.reset(guiFactory->createRectangleFrame());
	bgSprite.reset(guiFactory->createRectangle());
	bgSprite->setTint(panel->getTint());
	titleSprite.reset(guiFactory->createRectangle());
	titleSprite->setTint(Color(1, 1, 1, 1));
	buttonFrameSprite.reset(guiFactory->createRectangle());
	buttonFrameSprite->setTint(Color(1, 1, 1, 1));

	controls.resize(5);

	unique_ptr<GUIControl> titleText;

	dialogText.reset(guiFactory->createTextLabel(Vector2::Zero, L"", font, false));
	dialogText->setTint(Color(0, 0, 0, 1));

	setLayerDepth(.99f);

	texturePanel.reset(guiFactory->createPanel());
}

void PromptDialog::setSelectorManager(Joystick* joy, KeyboardController* keys) {

	if (!selector) {
		selector = make_unique<SelectorManager>();
		selector->initialize(make_unique<ColorFlashSelector>(guiFactory));

		if (controls[ButtonOK]) {
			selector->addControl((Selectable*) controls[ButtonOK].release());
		}
		if (controls[ButtonNeutral]) {
			selector->addControl((Selectable*) controls[ButtonNeutral].release());
		}
		if (controls[ButtonCancel]) {
			selector->addControl((Selectable*) controls[ButtonCancel].release());
		}
	}

	selector->setControllers(joy, keys);
}


void PromptDialog::forceRefresh() {
	refreshTexture = true;
}

void PromptDialog::reloadGraphicsAsset() {
	panel.reset(guiFactory->createPanel(false));
	panel->setTint(Color(0, 1, 1, 1));
	dialogText->reloadGraphicsAsset();
	frame->reloadGraphicsAsset();
	bgSprite->reloadGraphicsAsset(guiFactory);
	titleSprite->reloadGraphicsAsset(guiFactory);
	buttonFrameSprite->reloadGraphicsAsset(guiFactory);
	for (auto const& control : controls) {
		if (control == NULL)
			continue;
		control->reloadGraphicsAsset();
	}
	if (selector)
		selector->reloadGraphicsAssets();

	if (closeTransition != NULL)
		closeTransition->initializeEffect(this);
	if (openTransition != NULL)
		openTransition->initializeEffect(this);

	texturePanel.reset(guiFactory->createPanel());
	refreshTexture = true;

	calculateDialogTextPos();


}

void PromptDialog::setDimensions(const Vector2& pos, const Vector2& sz,
	const int frmThcknss) {

	frameThickness = frmThcknss;
	size = sz;


	GUIControl::setPosition(pos);

	bgSprite->setDimensions(position, size);
	frame->setDimensions(position, size, frameThickness);
	titleFrameSize.x = size.x - frameThickness * 2;
	titleFrameSize.y -= frameThickness;
	titleFramePosition = Vector2(position.x + frameThickness, position.y + frameThickness);
	titleSprite->setDimensions(titleFramePosition, titleFrameSize);
	textFramePosition = Vector2(position.x + frameThickness, position.y + titleFrameSize.y);
	//buttonFrameSize.y += frameThickness/2;
	textFrameSize =
		Vector2(size.x - frameThickness * 2, size.y - titleFrameSize.y - buttonFrameSize.y);
	buttonFramePosition =
		Vector2(position.x + frameThickness, textFramePosition.y + textFrameSize.y);
	buttonFrameSize.x = size.x - frameThickness * 2;

	buttonFrameSprite->setDimensions(buttonFramePosition, buttonFrameSize);

	panel->setDimensions(textFramePosition, textFrameSize);

	if (controls[TitleText] != NULL) {
		Vector2 titlesize = controls[TitleText]->measureString();
		if (titlesize.x > 0) {
			calculateTitlePos();
		}
	}

	testMinimumSize();

	calculateDialogTextPos();

	refreshTexture = true;
}



/* PromptDialog checks to see if it's open before performing any logic. */
bool PromptDialog::update(double deltaTime) {

	if (!isShowing)
		return false;

	if (isOpening) {
		isOpening = !(openTransition->*runTransition)(deltaTime);
	} else if (isClosing) {
		isClosing = !(closeTransition->*runTransition)(deltaTime);
		isShowing = isClosing;
		isPressed = false;
		if (!isShowing)
			(closeTransition->*resetTransition)();
	}

	if (movable && !isOpening && !isClosing) {
		if ((isHover = titleSprite->contains(mouse->getPosition()))) {

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

	if (panel->update(deltaTime))
		refreshTexture = true;
	if (frame->update())
		refreshTexture = true;

	for (auto const& control : controls) {
		if (control == NULL)
			continue;
		if (control->update(deltaTime))
			refreshTexture = true;

	}

	if (selector)
		selector->update(deltaTime);

	if (refreshTexture) {
		texturePanel->setTexture(texturize());
		refreshTexture = false;
		return true;
	}

	return false;
}

/* PromptDialog checks to see if it's open before performing any logic. */
void PromptDialog::draw(SpriteBatch* batch) {

	if (!isShowing)
		return;

	if (isOpening && (openTransition->*drawTransition)(batch)) {
		//OutputDebugString(L"opening\n");
	} else if (isClosing && (closeTransition->*drawTransition)(batch)) {
		//OutputDebugString(L"Closing\n");
	} else {
		texturePanel->draw(batch);
		if (selector)
			selector->draw(batch);
	}
}

unique_ptr<GraphicsAsset> PromptDialog::texturize() {
	return guiFactory->createTextureFromTexturizable(this);
}

void PromptDialog::textureDraw(SpriteBatch* batch, ComPtr<ID3D11Device> device) {

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


wstring PromptDialog::reformatText(size_t* scrollBarBuffer) {

	// if the text is longer than the dialog box
	//		break the text down into multiple lines
	wstring newText = L"";


	// how long line length?
	int maxLineLength = INT(textFrameSize.x - *scrollBarBuffer - (dialogTextMargin.x * 2));

	size_t i = 0;
	size_t textLength = wcslen(dialogText->getText());
	bool scrollbarAdded = false;
	bool done = false;
	while (i < textLength) {
		wstring currentLine = L"";
		while (dialogText->measureString(currentLine).x < maxLineLength) {

			currentLine += dialogText->getText()[i++];
			if (i >= textLength) {
				done = true;
				break;
			}
		}

		// how long is currentLine?
		int currentLength = (int) dialogText->measureString(currentLine).x;

		if (!done) {
			// go through currentLine until a whitespace is found and add a newline char before it
			wchar_t ch = currentLine[currentLine.length() - 1];
			size_t back = 0;
			while (!isspace(ch)) {

				++back;
				--i;
				// check to see if word is too long for line
				int nextChar = INT(currentLine.length() - back - 1);
				if (nextChar < 0) {
					/* this means current word is too long for line
					(i.e. stupidly narrow dialog box or ridiculously long word) */
					// TODO: hyphenate word and put rest on next line
					int excessLength = currentLength - maxLineLength;
					size_t o = currentLine.length();
					while (excessLength > 0) {
						wstring choppedWord = currentLine.substr(0, --o);
						excessLength = (int) dialogText->measureString(choppedWord).x - maxLineLength;
					}
					// should have a nicely fiting word chunk now (no hypen)
					i += o;
					back -= o;
					break;
				}
				ch = currentLine[nextChar];
			}
			currentLine.erase(currentLine.end() - back, currentLine.end());
		}

		newText += currentLine + L"\n";

		// If text is getting too long, restart and adjust for scrollbar
		if (!scrollbarAdded
			&& dialogText->measureString(newText).y + dialogTextMargin.y * 2
		> textFrameSize.y) {

			*scrollBarBuffer = (size_t) panel->getScrollBarSize().x;
			maxLineLength = INT(textFrameSize.x - *scrollBarBuffer - (dialogTextMargin.x * 2));
			i = 0;
			newText = L"";
			scrollbarAdded = true;
			done = false;
		}
	}

	return newText;
}

void PromptDialog::testMinimumSize() {

	Vector2 mindialogtextSize = dialogText->measureString(L"Min accept");
	int maxLineLength = INT(textFrameSize.x - (dialogTextMargin.x * 2));
	Vector2 newSize = size;
	bool changed = false;
	if (maxLineLength < mindialogtextSize.x) {
		newSize.x = mindialogtextSize.x + (dialogTextMargin.x * 2);
		changed = true;
	}

	if (size.y < mindialogtextSize.y) {
		newSize.y = mindialogtextSize.y;
		changed = true;
	}

	if (changed)
		setDimensions(position, newSize);
}

void PromptDialog::calculateTitlePos() {

	Vector2 titlesize = controls[TitleText]->measureString();

	if (titlesize.x > titleFrameSize.x
		|| titlesize.y > titleFrameSize.y) { // if title is bigger than dialog box
											 //expand the box
		Vector2 newSize = size;
		Vector2 newPos = position;
		if (titlesize.x > titleFrameSize.x) {
			newSize.x = titlesize.x + titleTextMargin + frameThickness * 2;
			newPos.x -= (newSize.x - size.x) / 2 - frameThickness;
		}
		if (titlesize.y > titleFrameSize.y) {
			titleFrameSize.y += titleTextMargin + frameThickness;
			// might have to do somethin here
			//newPos.y -= titleTextMargin;
		}

		//setTitleAreaDimensions(newSize);
		setDimensions(newPos, newSize, frameThickness);
		// not sure if this is necessary, but it was a fun excersize :O
	}
	Vector2 titlePos = Vector2(
		titleFramePosition.x + (titleFrameSize.x - titlesize.x) / 2 /*+ frameThickness * 2*/,
		titleFramePosition.y + (titleFrameSize.y - titlesize.y) / 2 /*+ frameThickness*/);
	controls[TitleText]->setPosition(titlePos);
}

void PromptDialog::setTitle(wstring text, const Vector2& scale,
	const pugi::char_t* font, Color color) {

	controls[TitleText].reset(guiFactory->createTextLabel(Vector2::Zero, text, font));
	controls[TitleText]->setScale(scale);
	controls[TitleText]->setTint(color);
	calculateTitlePos();
}

void PromptDialog::calculateDialogTextPos() {

	Vector2 dialogtextsize = dialogText->measureString();
	if (dialogtextsize.x <= 0) {
		// if text changed from something to empty texturepanel needs to be reset
		panel->setDimensions(Vector2::Zero, Vector2::Zero);
		return;
	}

	TextLabel formattedText(guiFactory, NULL, dialogText->getText(), dialogText->getFont(), false);
	size_t scrollBarBuffer = 0;

	if (dialogtextsize.x + dialogTextMargin.x * 2 > textFrameSize.x) {

		// if the text is longer than the dialog box
		//		break the text down into multiple lines
		formattedText.setText(reformatText(&scrollBarBuffer));
		dialogtextsize = formattedText.measureString();
	} else if (dialogtextsize.y /*+ dialogTextMargin.y * 2*/ > textFrameSize.y) {

		// width is fine but text is getting long
		scrollBarBuffer = (size_t) panel->getScrollBarSize().x;
		formattedText.setText(reformatText(&scrollBarBuffer));
		dialogtextsize = formattedText.measureString();
	}

	Vector2 textpanelpos;
	if (centerText) {
		textpanelpos = Vector2(textFramePosition.x +
			(textFrameSize.x - dialogtextsize.x - scrollBarBuffer) / 2, 0);
	} else {
		textpanelpos = textFramePosition + dialogTextMargin;
	}

	if (dialogtextsize.y < textFrameSize.y)
		textpanelpos.y = textFramePosition.y + (textFrameSize.y - dialogtextsize.y) / 2;
	else
		textpanelpos.y = textFramePosition.y;
	textpanelpos.y += 5; // offset so first line isn't covered by TitleBar
	dialogText->setPosition(textpanelpos);
	Color tint = dialogText->getTint();
	formattedText.setTint(dialogText->getTint());
	formattedText.setPosition(textpanelpos);

	panel->setDimensions(textFramePosition, textFrameSize);
	panel->setTexture(formattedText.texturize());
	panel->setTexturePosition(textpanelpos);
}

void PromptDialog::setText(wstring text) {

	dialogText->setText(text);
	calculateDialogTextPos();
}

void PromptDialog::setTitleAreaDimensions(const Vector2& newSize) {

	titleFrameSize = newSize;
	titleSprite->setDimensions(titleFramePosition, titleFrameSize);
}

void PromptDialog::setConfirmButton(unique_ptr<Button> okButton,
	bool autoPosition, bool autoSize) {


	if (autoSize)
		okButton->setDimensions(okButtonPosition, standardButtonSize, 3);

	controls[ButtonOK].reset();
	controls[ButtonOK] = move(okButton);

	if (autoPosition) {
		okButtonPosition.x = position.x + buttonMargin + frameThickness;
		if (calculateButtonPosition(okButtonPosition))
			okButtonPosition.y -= controls[ButtonOK]->getHeight() / 2;
	} else {
		okButtonPosition = controls[ButtonOK]->getPosition();
	}


	controls[ButtonOK]->setPosition(okButtonPosition);


}

void PromptDialog::setConfirmButton(wstring text, const pugi::char_t* font) {

	unique_ptr<Button> okButton;
	okButton.reset(guiFactory->createButton(font));
	okButton->setDimensions(okButtonPosition, standardButtonSize, 3);

	controls[ButtonOK].reset();
	controls[ButtonOK] = move(okButton);
	controls[ButtonOK]->setText(text);
	okButtonPosition.x = position.x + buttonMargin + frameThickness;
	if (calculateButtonPosition(okButtonPosition))
		okButtonPosition.y -= controls[ButtonOK]->getHeight() / 2;
	controls[ButtonOK]->setPosition(okButtonPosition);
}

void PromptDialog::setConfirmOnClickListener(Button::ActionListener* iOnClickListener) {

	if (controls[ButtonOK].get() == NULL) {
		setConfirmButton(L"OK");
	}

	((Button*) controls[ButtonOK].get())->setActionListener(iOnClickListener);
}

void PromptDialog::setCancelButton(unique_ptr<Button> cancelButton,
	bool autoPosition, bool autoSize) {


	if (autoSize)
		cancelButton->setDimensions(cancelButtonPosition, standardButtonSize, 3);

	controls[ButtonCancel].reset();
	controls[ButtonCancel] = move(cancelButton);

	if (autoPosition) {
		cancelButtonPosition.x =
			position.x + (size.x - controls[ButtonCancel]->getWidth()) - (buttonMargin + frameThickness);
		if (calculateButtonPosition(cancelButtonPosition))
			cancelButtonPosition.y -= controls[ButtonCancel]->getHeight() / 2;
	} else {
		cancelButtonPosition = controls[ButtonCancel]->getPosition();
	}

	controls[ButtonCancel]->setPosition(cancelButtonPosition);

}

void PromptDialog::setCancelButton(wstring text, const pugi::char_t * font) {

	unique_ptr<Button> cancelButton;
	cancelButton.reset(guiFactory->createButton(font));
	cancelButton->setDimensions(cancelButtonPosition, standardButtonSize, 3);
	cancelButton->setActionListener(new OnClickListenerCancelButton(this));
	controls[ButtonCancel].reset();
	controls[ButtonCancel] = move(cancelButton);
	controls[ButtonCancel]->setText(text);
	cancelButtonPosition.x =
		position.x + size.x - controls[ButtonCancel]->getWidth() - buttonMargin - frameThickness;
	if (calculateButtonPosition(cancelButtonPosition))
		cancelButtonPosition.y -= controls[ButtonCancel]->getHeight() / 2;
	controls[ButtonCancel]->setPosition(cancelButtonPosition);
}

void PromptDialog::setCancelOnClickListener(Button::ActionListener* iOnClickListener) {

	if (controls[ButtonCancel].get() == NULL) {
		setCancelButton(L"OK");
	}

	((Button*) controls[ButtonCancel].get())->setActionListener(iOnClickListener);
}

bool PromptDialog::calculateButtonPosition(Vector2& buttonPos) {

	int buttonheight = getMaxButtonHeight();
	if (buttonheight + buttonMargin * 2 > buttonFrameSize.y) {
		// this will shrink the dialog text
		buttonFrameSize.y = (float) buttonheight + buttonMargin * 2 + frameThickness;
		setDimensions(position, size, frameThickness);

		// recalculate all button y positions
		if (controls[ButtonOK] != NULL) {
			okButtonPosition.y = buttonFramePosition.y
				+ (buttonFrameSize.y - controls[ButtonOK]->getHeight() - frameThickness) / 2;
			controls[ButtonOK]->setPosition(okButtonPosition);
		}
		if (controls[ButtonCancel] != NULL) {
			cancelButtonPosition.y = buttonFramePosition.y
				+ (buttonFrameSize.y - controls[ButtonCancel]->getHeight() - frameThickness) / 2;
			controls[ButtonCancel]->setPosition(cancelButtonPosition);

		}
		if (controls[ButtonNeutral] != NULL) {
			neutralButtonPosition.y = buttonFramePosition.y
				+ (buttonFrameSize.y - controls[ButtonNeutral]->getHeight() - frameThickness) / 2;
			controls[ButtonNeutral]->setPosition(neutralButtonPosition);
		}
		return false;
	} else
		buttonPos.y = buttonFramePosition.y + (buttonFrameSize.y - frameThickness) / 2;

	return true;
}

size_t PromptDialog::addControl(unique_ptr<GUIControl> control) {

	control->moveBy(textFramePosition);
	controls.push_back(move(control));
	return controls.size() - 1;
}

/* Not too sure how this will behave....*/
void PromptDialog::addControls(vector<unique_ptr<GUIControl>> newControls) {

	for (int i = 0; i < newControls.size(); ++i)
		controls.push_back(move(newControls[i]));
}

GUIControl* PromptDialog::getControl(size_t controlPosition) const {
	return controls.at(controlPosition).get();
}


void PromptDialog::setFont(const pugi::char_t* fontName) {

	dialogText->setFont(fontName);
	calculateDialogTextPos();
}

void PromptDialog::setTextTint(const XMFLOAT4 color) {

	dialogText->setTint(color);
	calculateDialogTextPos();
}

void PromptDialog::setTint(const XMFLOAT4 color) {
	bgSprite->setTint(color);
	panel->setTint(color);
	calculateDialogTextPos();
}

void PromptDialog::setScale(const Vector2& newScale) {

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

void PromptDialog::setPosition(const Vector2& newPosition) {

	Vector2 moveVector = newPosition - position;
	GUIControl::setPosition(newPosition);
	textFramePosition += moveVector;
	titleFramePosition += moveVector;
	frame->moveBy(moveVector);
	bgSprite->moveBy(moveVector);
	titleSprite->moveBy(moveVector);
	buttonFrameSprite->moveBy(moveVector);
	panel->moveBy(moveVector);

	if (selector)
		selector->moveBy(moveVector);

	for (auto const& control : controls) {
		if (control == NULL)
			continue;
		control->moveBy(moveVector);
	}

	texturePanel->setPosition(position);
}


void PromptDialog::setLayerDepth(const float newDepth, bool frontToBack) {

	layerDepth = newDepth - .00001f;
	if (layerDepth < 0) {
		if (!frontToBack)
			layerDepth = .00001f;
		else
			layerDepth = 0.0f;
	}
	float nudge = .00000001f;
	if (!frontToBack)
		nudge *= -1;
	//float ld = layerDepth + nudge;
	bgSprite->setLayerDepth(layerDepth + nudge, frontToBack);
	panel->setLayerDepth(layerDepth + nudge * 2, frontToBack);
	titleSprite->setLayerDepth(layerDepth + nudge * 3, frontToBack);
	buttonFrameSprite->setLayerDepth(layerDepth + nudge * 4, frontToBack);

	nudge *= 4;
	for (auto const& control : controls) {
		if (control == NULL)
			continue;
		nudge += nudge;
		control->setLayerDepth(layerDepth + nudge, frontToBack);
	}
	nudge += nudge;

	frame->setLayerDepth(layerDepth + nudge, frontToBack);
}


const Color& PromptDialog::getPanelTint() const {
	return panel->getTint();
}




const vector<IElement2D*> PromptDialog::getElements() const {

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



int PromptDialog::getMaxButtonHeight() {

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

void PromptDialog::setDraggedPosition(Vector2& newPosition) {

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

	Vector2 moveVector = newPosition - position;
	if (moveVector == Vector2::Zero)
		return;
	GUIControl::setPosition(newPosition);
	textFramePosition += moveVector;
	titleFramePosition += moveVector;
	frame->moveBy(moveVector);
	bgSprite->moveBy(moveVector);
	titleSprite->moveBy(moveVector);
	buttonFrameSprite->moveBy(moveVector);
	panel->moveBy(moveVector);

	if (selector)
		selector->moveBy(moveVector);

	for (auto const& control : controls) {
		if (control == NULL)
			continue;
		control->moveBy(moveVector);
	}

	texturePanel->setPosition(position);
}


void PromptDialog::setScrollBar(ScrollBarDesc& scrollBarDesc) {
	panel->setScrollBar(scrollBarDesc);
}

void PromptDialog::alwaysShowScrollBar(bool alwaysShow) {
	panel->alwaysShowScrollBar(alwaysShow);
}


