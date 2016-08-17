#include "GUIControl.h"

void GUIControl::setPosition(Vector2& pos) {
		position = pos;
		hitArea->position = Vector2(pos.x - getWidth()*scale.x / 2,
			pos.y - getHeight()*scale.y / 2);
		hitArea->size = Vector2(getWidth()*scale.x, getHeight()*scale.y);
}

void GUIControl::setScale(const Vector2& scl) {
	scale = scl;
	setPosition(position);
}