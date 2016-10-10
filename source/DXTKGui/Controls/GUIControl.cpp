#include "GUIControl.h"


void GUIControl::moveBy(const Vector2& moveVector) {

	setPosition(position + moveVector);
}

void GUIControl::setPosition(const Vector2& pos) {
	position = pos;
	hitArea->position = Vector2(position.x, position.y);
	hitArea->size = Vector2(getWidth()*scale.x, getHeight()*scale.y);
}

const wchar_t * GUIControl::getText() {
	return L"";
}

void GUIControl::setScale(const Vector2& scl) {
	scale = scl;
	hitArea->size = Vector2(getWidth()*scale.x, getHeight()*scale.y);
}


const Vector2& GUIControl::getOrigin() const {
	return origin;
}

const Vector2& GUIControl::getScale() const {
	return scale;
}

const float GUIControl::getRotation() const {
	return rotation;
}

const Color& GUIControl::getTint() const {
	return tint;
}

const float GUIControl::getAlpha() const {
	return tint.w;
}

bool GUIControl::contains(const Vector2& point) {
	return hitArea->contains(point);
}

#include "GUIFactory.h"
GraphicsAsset* GUIControl::createTexture() {
	return guiFactory->createTextureFromIElement2D(this);

}

void GUIControl::setOrigin(const Vector2 & org) {
	origin = org;
}

void GUIControl::setRotation(const float rot) {
	rotation = rot;
}

void GUIControl::setTint(const XMFLOAT4 color) {
	tint = color;
}

void GUIControl::setAlpha(const float alpha) {
	tint.w = alpha;
}

