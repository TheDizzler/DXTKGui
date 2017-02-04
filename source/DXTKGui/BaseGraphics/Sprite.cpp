#include "Sprite.h"


Sprite::Sprite() {

	rotation = 0.0f;
	scale = Vector2(1, 1);
	tint = DirectX::Colors::White;
	layerDepth = 0.0f;

	width = 0;
	height = 0;
}

Sprite::Sprite(const Vector2& pos) {

	position = pos;
	rotation = 0.0f;
	scale = Vector2(1, 1);
	tint = DirectX::Colors::White;
	layerDepth = 0.0f;
	width = 0;
	height = 0;

}

Sprite::~Sprite() {
	texture.Reset();
}

void Sprite::load(GraphicsAsset* const graphicsAsset) {

	texture.Reset();
	texture = graphicsAsset->getTexture();
	width = graphicsAsset->getWidth();
	height = graphicsAsset->getHeight();
	Vector2 sheetLoc = graphicsAsset->getPosition();

	origin = Vector2(width / 2.0f, height / 2.0f);
	sourceRect.left = sheetLoc.x;
	sourceRect.top = sheetLoc.y;
	sourceRect.bottom = sheetLoc.y + height;
	sourceRect.right = sheetLoc.x + width;

	hitArea.reset(new HitArea(
		Vector2(position.x - origin.x, position.y - origin.y),
		Vector2(width, height)));
}


void Sprite::update(double deltaTime) {

	hitArea->position = Vector2(position.x - origin.x, position.y - origin.y);

}


void Sprite::draw(SpriteBatch* batch) {

	batch->Draw(texture.Get(), position, &sourceRect, tint, rotation,
		origin, scale, SpriteEffects_None, layerDepth);

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
	hitArea->size = Vector2(width*scale.x, height*scale.y);
}

void Sprite::moveBy(const Vector2& moveVector) {
	setPosition(position + moveVector);
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

void Sprite::setAlpha(const float alpha) {
	tint.w = alpha;
}

void Sprite::setLayerDepth(const float depth) {
	layerDepth = depth;
}


