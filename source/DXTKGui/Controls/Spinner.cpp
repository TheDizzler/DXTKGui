#include "Spinner.h"

#include "../GUIFactory.h"
Spinner::Spinner(GUIFactory* factory, shared_ptr<MouseController> mouseController,
	const Vector2& pos, const size_t len, const size_t itmHght, bool autoSz)
	: GUIControl(factory, mouseController) {

	position = pos;
	width = len + textBuffer * 2;
	itemHeight = itmHght;
	autoSize = autoSz;

}

Spinner::~Spinner() {
	list.clear();
}

#include <sstream>
void Spinner::initialize(const pugi::char_t* fontName,
	const pugi::char_t* upButtonName, const pugi::char_t* downButtonName) {


	label.reset(guiFactory->createTextLabel(Vector2::Zero, L"", fontName));
	label->setTint(Vector4(0, 0, 0, 1));
	label->setText("100");
	if (label->getHeight() > itemHeight)
		itemHeight = label->getHeight();


	upButton.reset((ImageButton*) guiFactory->createImageButton(upButtonName));
	if (upButton->getHeight() * 2 > itemHeight)
		itemHeight = upButton->getHeight() * 2;
	upButton->setActionListener(new SpinnerUpButtonListener(this));
	upButton->setPosition(Vector2(position.x + width, position.y));


	downButton.reset((ImageButton*) guiFactory->createImageButton(downButtonName));
	downButton->setPosition(Vector2(position.x + width, position.y + (itemHeight - upButton->getHeight())));
	downButton->setActionListener(new SpinnerDownButtonListener(this));

	frame.reset(guiFactory->createRectangleFrame());
	frame->setDimensions(position, Vector2(width, itemHeight));
	rectangle.reset(guiFactory->createRectangle());
	rectangle->setDimensions(position, Vector2(width, itemHeight));



	Vector2 labelpos = Vector2(position.x + textBuffer, position.y + (itemHeight - label->getHeight()) / 2);
	label->setPosition(labelpos);

	texturePanel.reset(guiFactory->createPanel());
}



unique_ptr<GraphicsAsset> Spinner::texturize() {
	return move(guiFactory->createTextureFromIElement2D(this));
}


bool Spinner::update(double deltaTime) {

	bool refreshed = false;
	if (upButton->update(deltaTime))
		refreshed = true;
	if (downButton->update(deltaTime))
		refreshed = true;
	if (frame->update())
		refreshed = true;
	if (label->update(deltaTime))
		refreshed = true;

	if (refreshed) {
		texturePanel->setTexture(texturize());
		return true;
	}
	return false;
}


void Spinner::draw(SpriteBatch* batch) {
	texturePanel->draw(batch);
}

void Spinner::textureDraw(SpriteBatch* batch, ComPtr<ID3D11Device> device) {
	rectangle->draw(batch);
	upButton->draw(batch);
	downButton->draw(batch);
	frame->draw(batch);
	label->draw(batch);
}


void Spinner::addItems(vector<wstring> items) {

	if (autoSize) {
		for (wstring item : items) {
			if (label->measureString(item).x + textBuffer * 2 > width)
				width = label->measureString(item).x + textBuffer * 2;
			list.push_back(item);
		}
		upButton->setPosition(Vector2(position.x + width, position.y));
		downButton->setPosition(Vector2(position.x + width, position.y + (itemHeight - upButton->getHeight())));
		frame->setSize(Vector2(width, itemHeight));
		rectangle->setSize(Vector2(width, itemHeight));
	} else {
		vector<wstring> AB;
		AB.reserve(list.size() + items.size()); // preallocate memory
		AB.insert(AB.end(), list.begin(), list.end());
		AB.insert(AB.end(), items.begin(), items.end());
		list = AB;
	}

	label->setText(list[selected]);
	items.clear();
}

const wstring Spinner::getSelected() const {
	return list[selected];
}

void Spinner::increase() {
	if (++selected >= list.size())
		selected = 0;
	label->setText(list[selected]);
}

void Spinner::decrease() {
	if (--selected >= list.size())
		selected = list.size() - 1;
	label->setText(list[selected]);
}


void Spinner::setScale(const Vector2& scl) {
	scale = scl;
}

void Spinner::setFont(const pugi::char_t* font) {
	label->setFont(font);
}

void Spinner::setText(wstring text) {
}

const Vector2 &XM_CALLCONV Spinner::measureString() const {
	return Vector2(longestStringLength, itemHeight);
}

void Spinner::moveBy(const Vector2& moveVector) {
	rectangle->moveBy(moveVector);
	upButton->moveBy(moveVector);
	downButton->moveBy(moveVector);
	frame->moveBy(moveVector);
	label->moveBy(moveVector);
	position += moveVector;
}

void Spinner::setPosition(const Vector2& newPosition) {

	Vector2 change = newPosition - position;
	moveBy(change);

}

const Vector2& Spinner::getPosition() const {
	return position;
}

const int Spinner::getWidth() const {
	return width;
}

const int Spinner::getHeight() const {
	return itemHeight;
}

bool Spinner::clicked() {
	return false;
}

bool Spinner::pressed() {
	return false;
}

bool Spinner::hovering() {
	return false;
}

void SpinnerUpButtonListener::onClick(Button* button) {
	spinner->increase();
}

void SpinnerUpButtonListener::onPress(Button* button) {
}

void SpinnerUpButtonListener::onHover(Button* button) {
}

void SpinnerDownButtonListener::onClick(Button* button) {
	spinner->decrease();
}

void SpinnerDownButtonListener::onPress(Button* button) {
}

void SpinnerDownButtonListener::onHover(Button* button) {
}
