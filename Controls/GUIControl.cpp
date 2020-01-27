#include "GUIControl.h"


void GUIControl::moveBy(const Vector2& moveVector) {

	setPosition(position + moveVector);
}

void GUIControl::setPosition(const Vector2& pos) {
	position = pos;
	hitArea.position = Vector2(position.x, position.y);
	hitArea.size = Vector2(getWidth()*scale.x, getHeight()*scale.y);
}


GUIControl::~GUIControl() {
}

const wchar_t* GUIControl::getText() {
	return L"";
}

void GUIControl::setScale(const Vector2& scl) {
	scale = scl;
	hitArea.position = Vector2(position.x - origin.x*scale.x,
		position.y - origin.y*scale.y);
	hitArea.size = Vector2(getWidth()*scale.x, getHeight()*scale.y);
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
	return projectedHitArea.contains(point);
}


const float GUIControl::getLayerDepth() const {
	return layerDepth;
}

const HitArea& GUIControl::getHitArea() const {
	return hitArea;
}


void GUIControl::updateProjectedHitArea() {

	Vector2 screenCords = getScreenPosition(translationMatrix());
	projectedHitArea.position = screenCords;
	projectedHitArea.size = hitArea.size * cameraZoom();
}

const Vector2 GUIControl::getScreenPosition(Matrix viewProjectionMatrix) const {

	Vector2 screenCords = XMVector2Transform(hitArea.position, viewProjectionMatrix);
	return screenCords;
}

unique_ptr<HitArea> GUIControl::getScreenHitArea(Matrix viewProjectionMatrix) const {

	Vector2 screenCords = getScreenPosition(viewProjectionMatrix);
	return make_unique<HitArea>(screenCords, hitArea.size*scale);
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

void GUIControl::setTint(const Color & color) {
	tint = color;
}

void GUIControl::setTint(const XMVECTORF32 color) {
	tint = color;
}

void GUIControl::setAlpha(const float alpha) {
	tint.w = alpha;
}

void GUIControl::setLayerDepth(const float depth, bool frontToBack) {
	layerDepth = depth;
}

void GUIControl::setHitArea(HitArea* newHitArea) {
	hitArea.position = newHitArea->position;
	hitArea.size = newHitArea->size;
}

