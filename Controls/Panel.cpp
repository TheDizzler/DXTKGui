#include "Panel.h"


Panel::~Panel() {
}


bool Panel::update(double deltaTime) {
	return false;
}


void Panel::draw(SpriteBatch* batch) {
}


void Panel::setFont(const pugi::char_t* font) {
}

void Panel::setText(wstring text) {
}

const Vector2 XM_CALLCONV Panel::measureString() const {
	return Vector2::Zero;
}

const Vector2 & Panel::getPosition() const {
	return position;
}

const int Panel::getWidth() const {
	return 0;
}

const int Panel::getHeight() const {
	return 0;
}

bool Panel::clicked() {
	return false;
}

bool Panel::pressed() {
	return false;
}

bool Panel::hovering() {
	return false;
}

size_t Panel::addControl(unique_ptr<GUIControl> control) {
	return size_t();
}

void Panel::addControls(vector<unique_ptr<GUIControl>> controls) {
}

GUIControl* Panel::getControl(size_t controlPosition) const {
	return controls.at(controlPosition).get();
}
