#include "Spinner.h"

#include "GUIFactory.h"
Spinner::Spinner(const Vector2& pos, const size_t len, const size_t itmHght, bool autoSz) {

	position = pos;
	width = len + textBuffer * 2;
	itemHeight = itmHght;
	autoSize = autoSz;

}

Spinner::~Spinner() {
}

#include <sstream>
void Spinner::initialize(const pugi::char_t* fontName, GraphicsAsset* pixelAsset,
	const pugi::char_t* upButtonName, const pugi::char_t* downButtonName) {


	label.reset(guiFactory->createTextLabel(Vector2::Zero, fontName));
	label->setTint(Vector4(0, 0, 0, 1));
	label->setText("100");
	if (label->getHeight() > itemHeight)
		itemHeight = label->getHeight();


	upButton.reset((ImageButton*) guiFactory->createImageButton(upButtonName));
	/*upButton->setDimensions(Vector2(position.x + width, position.y), Vector2(upButton->getWidth(), itemHeight / 2));*/
	if (upButton->getHeight() * 2 > itemHeight)
		itemHeight = upButton->getHeight() * 2;

	upButton->setPosition(Vector2(position.x + width, position.y));
	upButton->setOnClickListener(new SpinnerUpButtonListener(this));
	downButton.reset((ImageButton*) guiFactory->createImageButton(downButtonName));
	/*downButton->setDimensions(Vector2(position.x + width, position.y + (itemHeight - upButton->getHeight())),
		Vector2(upButton->getWidth(), itemHeight / 2));*/
	downButton->setPosition(Vector2(position.x + width, position.y + (itemHeight - upButton->getHeight())));
	downButton->setOnClickListener(new SpinnerDownButtonListener(this));

	frame = make_unique<RectangleFrame>(pixelAsset);
	frame->setDimensions(position, Vector2(width/* + upButton->getWidth()*/, itemHeight));
	rectangle = make_unique<RectangleSprite>(pixelAsset);
	rectangle->setDimensions(position, Vector2(width, itemHeight));



	Vector2 labelpos = Vector2(position.x + textBuffer, position.y + (itemHeight - label->getHeight()) / 2);
	label->setPosition(labelpos);
}


void Spinner::update(double deltaTime) {

	upButton->update(deltaTime);
	downButton->update(deltaTime);
}


void Spinner::draw(SpriteBatch* batch) {
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
	return Vector2(longestStringLength, frame->getHeight());
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
	return frame->getHeight();
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
