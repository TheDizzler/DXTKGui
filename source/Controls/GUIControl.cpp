#include "GUIControl.h"

void GUIControl::setPosition(const Vector2& pos) {
	position = pos;
	hitArea->position = Vector2(pos.x - getWidth()*scale.x / 2,
		pos.y - getHeight()*scale.y / 2);
	hitArea->size = Vector2(getWidth()*scale.x, getHeight()*scale.y);
}

void GUIControl::setScale(const Vector2& scl) {
	scale = scl;
	setPosition(position);
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

const Color & GUIControl::getTint() const {
	return tint;
}

const float GUIControl::getAlpha() const {
	return alpha;
}


void GUIControl::setOrigin(const Vector2 & org) {
	origin = org;
}

void GUIControl::setRotation(const float rot) {
	rotation = rot;
}

void GUIControl::setTint(const Color& color) {
	tint = color;
}

void GUIControl::setAlpha(const float alfa) {
	alpha = alfa;
}

