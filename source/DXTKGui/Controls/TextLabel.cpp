#include "TextLabel.h"
#include "../GUIFactory.h"

TextLabel::TextLabel(GUIFactory* factory, shared_ptr<MouseController> mouseController,
	Vector2 pos, wstring text, const pugi::char_t* fontName, bool texture)
	: GUIControl(factory, mouseController) {

	position = pos;
	font = guiFactory->getFont(fontName);
	hitArea = make_unique<HitArea>(position, Vector2::Zero);

	useTexture = texture;
	texturePanel.reset(guiFactory->createPanel());
	setText(text);
	setTint(normalColorText);
}

TextLabel::TextLabel(GUIFactory* factory, shared_ptr<MouseController> mouseController,
	wstring text, shared_ptr<FontSet> fnt, bool texture) : GUIControl(factory, mouseController) {

	font = fnt;
	hitArea = make_unique<HitArea>(position, Vector2::Zero);
	useTexture = texture;
	texturePanel.reset(guiFactory->createPanel());
	setText(text);
	setTint(normalColorText);
}

TextLabel::~TextLabel() {
	if (actionListener != NULL)
		delete actionListener;
}

void TextLabel::reloadGraphicsAsset() {
	const char_t* fontName = font->fontName;
	font.reset();
	font = guiFactory->getFont(fontName);
	texturePanel.reset(guiFactory->createPanel());
	
	refreshTexture = true;
}


bool TextLabel::update(double deltaTime) {

	if (isHoverable) {
		if (hitArea->contains(mouse->getPosition())) {
			isHover = true;
			if (!isPressed) {
				if (!hasBeenSetHover) {
					onHover();
				}
			}
		} else
			isHover = false;

		if (isPressed && !mouse->leftButton()) {
			onClick();
		} else {
			isClicked = false;
			if (!isHover) {
				if (!hasBeenSetUnpressed) {
					resetState();
				}
			} else if (mouse->pressed()) {
				onPress();
			}
		}
	}
	
	if (useTexture && refreshTexture) {
		texturePanel->setTexture(move(texturize()));
		texturePanel->setAlpha(tint.w);
		refreshTexture = false;
		return true;
	}
	
	return false;
}


void TextLabel::draw(SpriteBatch* batch) {

	if (!useTexture)
		font->draw(batch, label.c_str(), position, tint,
			rotation, origin, scale, layerDepth);
	else
		texturePanel->draw(batch);
}

void TextLabel::draw(SpriteBatch* batch, Color color) {
	font->draw(batch, label.c_str(), position, color,
		rotation, origin, scale, layerDepth);
}


unique_ptr<GraphicsAsset> TextLabel::texturize() {
	return guiFactory->createTextureFromTexturizable(this);
}

void TextLabel::textureDraw(SpriteBatch* batch, ComPtr<ID3D11Device> device) {

	font->draw(batch, label.c_str(), position, tint,
		rotation, origin, scale, layerDepth);
}

#include <sstream>
void TextLabel::setText(string text) {

	wostringstream wss;
	wss << text.c_str();
	setText(wss.str());
}

void TextLabel::setText(wostringstream& text) {
	setText(text.str());
}


void TextLabel::setText(wstring text) {

	label = text;
	Vector2 size = font->measureString(label.c_str());
	size *= scale;
	hitArea->position = position;
	hitArea->size = size;

	refreshTexture = true;
}

const Vector2& XM_CALLCONV TextLabel::measureString() const {
	Vector2 size = font->measureString(label.c_str());
	Vector2 scaledSize = size*scale;
	return scaledSize;
}

const Vector2& XM_CALLCONV TextLabel::measureString(wstring string) const {
	Vector2 size = font->measureString(string.c_str());
	return size;
}

const wchar_t* TextLabel::getText() {
	return label.c_str();
}


bool TextLabel::clicked() {

	if (isClicked) {
		isClicked = isPressed = false;
		return true;
	}

	return false;
}

bool TextLabel::pressed() {
	return isPressed;
}

bool TextLabel::hovering() {
	return isHover;
}

void TextLabel::setHoverable(bool hoverable) {
	isHoverable = hoverable;
}

void TextLabel::setToUnpressedState() {
	setTint(normalColorText);
}

void TextLabel::setToHoverState() {
	setTint(hoverColorText);
}

void TextLabel::setToSelectedState() {
	setTint(selectedColorText);
}


void TextLabel::moveBy(const Vector2& moveVector) {
	setPosition(position + moveVector);
}

void TextLabel::setPosition(const Vector2& pos) {
	GUIControl::setPosition(pos);

	if (useTexture)
		texturePanel->setPosition(position);
}

void TextLabel::setFont(const pugi::char_t* fontName) {
	font = guiFactory->getFont(fontName);
	refreshTexture = true;
}

void TextLabel::setFont(shared_ptr<FontSet> newFont) {
	font = newFont;
	refreshTexture = true;
}

void TextLabel::setTint(const XMFLOAT4 color) {
	tint = color;
	refreshTexture = true;
}

void TextLabel::setTint(const Color& color) {
	tint = color;
	refreshTexture = true;
}

void TextLabel::setTint(const XMVECTORF32 color) {
	tint = color;
	refreshTexture = true;
}

void TextLabel::setAlpha(const float alpha) {
	tint.w = alpha;
	texturePanel->setAlpha(alpha);
}

void TextLabel::setScale(const Vector2 & scl) {
	scale = scl;
	Vector2 size = font->measureString(label.c_str());
	size *= scale;
	hitArea->position = position;
	hitArea->size = size;
	refreshTexture = true;
}

void TextLabel::setLayerDepth(const float newDepth, bool frontToBack) {
	layerDepth = newDepth;
}



const Vector2& TextLabel::getPosition() const {
	return position;
}


int const TextLabel::getWidth() const {
	return ceil(hitArea->size.x);
}

int const TextLabel::getHeight() const {
	return ceil(hitArea->size.y);
}

const shared_ptr<FontSet> TextLabel::getFont() const {
	return font;
}
