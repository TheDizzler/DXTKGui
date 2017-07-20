#include "Sprite.h"
#include "../GUIFactory.h"
#include <sstream>

Sprite::Sprite() {

	rotation = 0.0f;
	scale = Vector2(1, 1);
	tint = DirectX::Colors::White;
	layerDepth = 0.1f;

	width = 0;
	height = 0;
}

Sprite::Sprite(const Vector2& pos) {

	position = pos;
	rotation = 0.0f;
	scale = Vector2(1, 1);
	tint = DirectX::Colors::White;
	layerDepth = 0.1f;
	width = 0;
	height = 0;

}


Sprite::~Sprite() {
	//int numRefRemaining = texture.Reset();
	//stringstream ss;
	//ss << assetName << ": " << numRefRemaining;
	//if (numRefRemaining == 0)
	//	ss << "!!!!!!!";
	//ss << endl;
	///*char buffer[255];
	//sprintf(buffer, ss.str().c_str());*/

	//OutputDebugStringA(ss.str().c_str());
}

/* GraphicsAsset is not stored in Sprite. */
void Sprite::load(GraphicsAsset* const graphicsAsset) {

	assetName = graphicsAsset->assetName;
	texture = graphicsAsset->getTexture();
	width = graphicsAsset->getWidth();
	height = graphicsAsset->getHeight();

	origin = graphicsAsset->getOrigin();

	sourceRect = graphicsAsset->getSourceRect();

	hitArea.reset(new HitArea(
		Vector2(position.x - origin.x, position.y - origin.y),
		Vector2(width, height)));
}


void Sprite::reloadGraphicsAsset(GUIFactory* guiFactory) {
	load(guiFactory->getAsset(assetName.c_str()));
}



void Sprite::draw(SpriteBatch* batch) {
	batch->Draw(texture.Get(), position, &sourceRect, tint, rotation,
		origin, scale, spriteEffect, layerDepth);
}


ComPtr<ID3D11ShaderResourceView> Sprite::getTexture() {
	return texture;
}

const HitArea* Sprite::getHitArea() const {
	return hitArea.get();
}

const Vector2& Sprite::getPosition() const {
	return position;
}

const Vector2& Sprite::getOrigin() const {
	return origin;
}

const Vector2& Sprite::getScale() const {
	return scale;
}

const float Sprite::getRotation() const {
	return rotation;
}

const Color& Sprite::getTint() const {
	return tint;
}

const float Sprite::getAlpha() const {
	return tint.w;
}

const RECT Sprite::getRect() const {
	return sourceRect;
}

const float Sprite::getLayerDepth() const {
	return layerDepth;
}

const int Sprite::getWidth() const {
	return width;
}

const int Sprite::getHeight() const {
	return height;
}


void Sprite::setDimensions(Sprite* baseSprite) {

	width = baseSprite->width;
	height = baseSprite->height;

	hitArea.reset(new HitArea(
		Vector2(position.x - width*scale.x / 2, position.y - height*scale.y / 2),
		Vector2(width*scale.x, height*scale.y)));
}

void Sprite::setDimensions(const Vector2& pos, const Vector2& size) {

	setSize(size);
	setPosition(pos);
}

void Sprite::setSize(const Vector2& size) {

	width = size.x;
	height = size.y;

	sourceRect.left = 0;
	sourceRect.top = 0;
	sourceRect.bottom = size.y;
	sourceRect.right = size.x;

	hitArea.reset(new HitArea(
		Vector2(position.x - origin.x *scale.x, position.y - origin.y*scale.y),
		Vector2(size.x*scale.x, size.y*scale.y)));
}

void Sprite::setPosition(const Vector2& pos) {

	position = pos;
	hitArea->position = Vector2(position.x - origin.x*scale.x,
		position.y - origin.y*scale.y);
}


void Sprite::setOrigin(const Vector2& orgn) {
	origin = orgn;
}

void Sprite::setScale(const Vector2& scl) {

	scale = scl;
	hitArea->position = Vector2(position.x - origin.x*scale.x,
		position.y - origin.y*scale.y);
	hitArea->size = Vector2(width * scale.x, height * scale.y);
}

void Sprite::setRotation(const float rot) {
	rotation = rot;
}

void Sprite::setTint(const XMFLOAT4 colr) {
	tint = colr;
}

void Sprite::setTint(const Color& color) {
	tint = color;
}

void Sprite::setTint(const XMVECTORF32 color) {
	tint = color;
}

void Sprite::setAlpha(const float alpha) {
	tint.w = alpha;
}

void Sprite::setLayerDepth(const float depth, bool frontToBack) {
	layerDepth = depth;
}

void Sprite::moveBy(const Vector2& moveVector) {
	setPosition(position + moveVector);
}

void Sprite::rotateBy(float rotateAmount) {
	rotation += rotateAmount;
}


