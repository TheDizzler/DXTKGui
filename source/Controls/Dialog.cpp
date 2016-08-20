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
void Dialog::initialize(unique_ptr<FontSet> fnt,
	ComPtr<ID3D11ShaderResourceView> pixelTexture) {

	/*if (!Sprite::load(device, Assets::uglyDialogBox))
		return false;

	setScale(Vector2(3, 1.5));*/

	font = move(fnt);
	font->setTint(DirectX::Colors::White.v);

	frame.reset(new RectangleFrame(pixelTexture));
	bgSprite.reset(new RectangleSprite(pixelTexture));
	hitArea.reset(new HitArea(Vector2::Zero, Vector2::Zero));

	titleText.reset(new TextLabel(GameManager::guiFactory->getFont("BlackCloak")));

	/*TextLabel* label = new TextLabel(textLoc, font.get());
	label->setText("Really Quit Tender Torrent?");
	Vector2 size = font->measureString(label->getText());
	textLoc = Vector2(position.x - size.x / 2, position.y - height / 4);
	label->setPosition(textLoc);
	controls.push_back(label);


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

	setPosition(pos);
	size = sz;
	frame->setDimensions(position, size, frameThickness);

	Vector2 titlePos = position;
	titlePos.y += 10;
	Vector2 titlesize = titleText->measureString();
	titlePos.x = position.x - titlesize.x / 2;
	titleText->setPosition(titlePos);
	//TextLabel* label = new TextLabel(textLoc, font.get());
}


void Dialog::setTitle(wstring text) {

	titleText->setText(text);
	Vector2 titlePos = position;
	titlePos.y += 10; // random offset?
	Vector2 titlesize = titleText->measureString();
	titlePos.x = position.x - titlesize.x / 2;
	titleText->setPosition(titlePos);
}


void Dialog::setText(wstring text) {

	textLabel->setText(text);
	Vector2 textsize = font->measureString(textLabel->getText());
	Vector2 textLoc = Vector2(position.x - textsize.x / 2,
		position.y - textsize.y / 4);
	textLabel->setPosition(textLoc);
	//controls.push_back(label);
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

	/*for (TextButton* button : buttons) {
		button->update(deltaTime, mouse);
		if (button->clicked()) {
			switch (button->action) {
				case Button::OK:
					result = CONFIRM;
					break;
				case Button::CANCEL:
					result = DialogResult::CANCEL;
					break;
			}
		}
	}*/

	//for (unique_ptr<GUIControl> control : controls) {
	for (auto const& control : controls) {
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

	for (auto const& control : controls)
		control->draw(batch);
	/*Sprite::draw(batch);

	for (TextButton* button : buttons)
		button->draw(batch);

	for (TextLabel* label : labels)
		label->draw(batch);*/
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

	font.release();
	font = move(newFont);
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



