#include "Spinner.h"
#include "../GUIFactory.h"

Spinner::Spinner(GUIFactory* factory, MouseController* mouseController,
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


void Spinner::initialize(const pugi::char_t* fontName,
	const pugi::char_t* upButtonName, const pugi::char_t* downButtonName) {

	label.reset(guiFactory->createTextLabel(Vector2::Zero, L"Empty", fontName));
	label->setTint(Vector4(0, 0, 0, 1));
	//label->setText("Empty");
	if (label->getHeight() > itemHeight)
		itemHeight = label->getHeight();
	if (label->getWidth() > width)
		width = label->getWidth() + textBuffer*3;

	upButton.reset((ImageButton*) guiFactory->createImageButton(upButtonName));
	if (upButton->getHeight() * 2 > itemHeight)
		itemHeight = upButton->getHeight() * 2;
	upButton->setActionListener(new SpinnerUpButtonListener(this));
	upButton->setPosition(Vector2(position.x + width, position.y));

	downButton.reset((ImageButton*) guiFactory->createImageButton(downButtonName));
	downButton->setActionListener(new SpinnerDownButtonListener(this));
	downButton->setPosition(
		Vector2(position.x + width, position.y + (itemHeight - upButton->getHeight())));

	frame.reset(guiFactory->createRectangleFrame());
	frame->setDimensions(position, Vector2((float) width, (float) itemHeight));
	rectangle.reset(guiFactory->createRectangle());
	rectangle->setDimensions(position, Vector2((float) width, (float) itemHeight));

	Vector2 labelpos = Vector2(
		position.x + textBuffer, position.y + (itemHeight - label->getHeight()) / 2);
	label->setPosition(labelpos);

	texturePanel.reset(guiFactory->createPanel());
	texturePanel->setPosition(position);

	setLayerDepth(layerDepth);
}


void Spinner::forceRefresh() {
	refreshTexture = true;
}

void Spinner::reloadGraphicsAsset() {
	label->reloadGraphicsAsset();
	upButton->reloadGraphicsAsset();
	downButton->reloadGraphicsAsset();
	frame.reset(guiFactory->createRectangleFrame(position,
		Vector2((float) width, (float) itemHeight), frame->getThickness(), frame->getTint()));
	rectangle->reloadGraphicsAsset(guiFactory);
	texturePanel.reset(guiFactory->createPanel());
	refreshTexture = true;
}

unique_ptr<GraphicsAsset> Spinner::texturize() {
	return move(guiFactory->createTextureFromTexturizable(this));
}

bool Spinner::update(double deltaTime) {

	if (upButton->update(deltaTime))
		refreshTexture = true;
	if (downButton->update(deltaTime))
		refreshTexture = true;
	if (frame->update())
		refreshTexture = true;
	if (label->update(deltaTime))
		refreshTexture = true;

	if (refreshTexture) {
		texturePanel->setTexture(texturize());
		refreshTexture = false;
		return true;
	}
	return false;
}


void Spinner::draw(SpriteBatch* batch) {
	texturePanel->draw(batch);
}

void Spinner::textureDraw(SpriteBatch* batch, ComPtr<ID3D11Device> device) {
	rectangle->draw(batch);
	frame->draw(batch);
	label->draw(batch);
	upButton->draw(batch);
	downButton->draw(batch);
}


void Spinner::addItem(wstring item) {
	if (autoSize) {
		if (label->measureString(item).x + textBuffer * 2 > width) {
			width = size_t(label->measureString(item).x + textBuffer * 2);
			if (width == 0) {
				OutputDebugString(L"Spinner Warning: Item received with 0 length string.");
				return;
			}
			upButton->setPosition(Vector2(position.x + width, position.y));
			downButton->setPosition(
				Vector2(position.x + width,
					position.y + (itemHeight - upButton->getHeight())));
			frame->setSize(Vector2((float) width, (float) itemHeight));
			rectangle->setSize(Vector2((float) width, (float) itemHeight));
		}
	}
	list.push_back(item);

	if (list.size() == 1) {
		selected = 0;
		label->setText(list[selected]);
		refreshTexture = true;
	}
}

void Spinner::addItems(vector<wstring> items) {

	if (items.size() == 0)
		return;

	if (autoSize) {
		bool changed = false;
		for (wstring item : items) {
			if (label->measureString(item).x + textBuffer * 2 > width) {
				changed = true;
				width = size_t(label->measureString(item).x + textBuffer * 2);
				if (width == 0) {
					OutputDebugString(L"Spinner Warning: Item received with 0 length string.");
					continue;
				}
			}
			list.push_back(item);
		}
		if (changed) {
			upButton->setPosition(Vector2((float) position.x + width, position.y));
			downButton->setPosition(
				Vector2(position.x + width,
					position.y + (itemHeight - upButton->getHeight())));
			frame->setSize(Vector2((float) width, (float) itemHeight));
			rectangle->setSize(Vector2((float) width, (float) itemHeight));
		}
	} else {
		vector<wstring> AB;
		AB.reserve(list.size() + items.size()); // preallocate memory
		AB.insert(AB.end(), list.begin(), list.end());
		AB.insert(AB.end(), items.begin(), items.end());
		list = AB;
	}

	label->setText(list[selected]);
	items.clear();
	refreshTexture = true;
}

bool Spinner::removeItem(wstring removeItem) {

	for (size_t i = 0; i < list.size(); ++i) {
		if (list[i] == removeItem) {
			swap(list[i], list.back());
			list.pop_back();
			if (list.size() == 0) {
				label->setText("Empty");
				refreshTexture = true;
			} else if (selected == i) {
				if (selected >= list.size())
					selected = list.size() - 1;
				label->setText(list[selected]);
			}
			return true;
		}
	}

	return false;
}

const wstring Spinner::getSelected() const {
	return list[selected];
}

void Spinner::increase() {
	if (list.size() == 0)
		return;
	if (++selected >= list.size())
		selected = 0;
	label->setText(list[selected]);
}

void Spinner::decrease() {
	if (list.size() == 0)
		return;
	if (--selected >= list.size())
		selected = list.size() - 1;
	label->setText(list[selected]);
}

void Spinner::setLayerDepth(float newDepth, bool frontToBack) {
	layerDepth = newDepth - .00001f;
	if (layerDepth < 0) {
		if (!frontToBack)
			layerDepth = .00001f;
		else
			layerDepth = 0;
	}
	float nudge = .00000001f;
	if (!frontToBack)
		nudge *= -1;



	rectangle->setLayerDepth(layerDepth + nudge, frontToBack);
	frame->setLayerDepth(layerDepth + nudge * 2, frontToBack);
	label->setLayerDepth(layerDepth + nudge * 2, frontToBack);
	upButton->setLayerDepth(layerDepth + nudge * 3, frontToBack);
	downButton->setLayerDepth(layerDepth + nudge * 3, frontToBack);
}


void Spinner::setScale(const Vector2& scl) {
	scale = scl;
}

void Spinner::setFont(const pugi::char_t* font) {
	label->setFont(font);
}

void Spinner::setText(wstring text) {
}

const Vector2 XM_CALLCONV Spinner::measureString() const {
	return Vector2((float) longestStringLength, (float) itemHeight);
}

void Spinner::moveBy(const Vector2& moveVector) {
	rectangle->moveBy(moveVector);
	upButton->moveBy(moveVector);
	downButton->moveBy(moveVector);
	frame->moveBy(moveVector);
	label->moveBy(moveVector);
	position += moveVector;

	texturePanel->setPosition(position);
}

void Spinner::setPosition(const Vector2& newPosition) {

	Vector2 change = newPosition - position;
	position = newPosition;
	rectangle->moveBy(change);
	upButton->moveBy(change);
	downButton->moveBy(change);
	frame->moveBy(change);
	label->moveBy(change);

	texturePanel->setPosition(position);
}

const Vector2& Spinner::getPosition() const {
	return position;
}

const int Spinner::getWidth() const {
	return (int) width + upButton->getWidth();
}

const int Spinner::getHeight() const {
	return (int) itemHeight;
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

void SpinnerUpButtonListener::resetState(Button * button) {
}

void SpinnerDownButtonListener::onClick(Button* button) {
	spinner->decrease();
}

void SpinnerDownButtonListener::onPress(Button* button) {
}

void SpinnerDownButtonListener::onHover(Button* button) {
}

void SpinnerDownButtonListener::resetState(Button * button) {
}
