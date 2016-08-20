#include "Sprite.h"


Sprite::Sprite() {

	alpha = 1.0f;
	rotation = 0.0f;
	scale = Vector2(1, 1);
	tint = DirectX::Colors::White.v;
	layerDepth = 0.0f;

	width = 0;
	height = 0;
}

Sprite::Sprite(const Vector2& pos) {

	position = pos;
	alpha = 1.0f;
	rotation = 0.0f;
	scale = Vector2(1, 1);
	tint = DirectX::Colors::White.v;
	layerDepth = 0.0f;
	width = 0;
	height = 0;

}

Sprite::~Sprite() {

}

#include "../globals.h"
void Sprite::load( GraphicsAsset* const graphicsAsset) {

	texture = graphicsAsset->getTexture();
	width = graphicsAsset->getWidth();
	height = graphicsAsset->getHeight();

	
	origin = Vector2(width / 2.0f, height / 2.0f);
	sourceRect.left = 0;
	sourceRect.top = 0;
	sourceRect.bottom = height;
	sourceRect.right = width;

	hitArea.reset(new HitArea(
		Vector2(position.x - width / 2, position.y - height / 2),
		Vector2(width, height)));

}



void Sprite::draw(SpriteBatch* batch) {

	batch->Draw(texture.Get(), position, &sourceRect, tint, rotation,
		origin, scale, SpriteEffects_None, layerDepth);

}

ComPtr<ID3D11ShaderResourceView> Sprite::getTexture() {
	return texture;
}

//ComPtr<ID3D11Resource> Sprite::getResource() {
//	return resource;
//}


void Sprite::update(double deltaTime) {

	hitArea->position = Vector2(position.x - width / 2, position.y - height / 2);

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

	return alpha;
}

const RECT Sprite::getRect() const {
	return sourceRect;
}

const int Sprite::getWidth() const {
	return width;
}

const int Sprite::getHeight() const {
	return height;
}

//void Sprite::setHitArea(const HitArea* hitarea) {
//
//	hitArea = new HitArea(Vector2(position.x - width / 2, position.y - height / 2),
//		Vector2(hitarea->size.x - 2, hitarea->size.y - 2));
//}

void Sprite::setDimensions(Sprite* baseSprite) {

	width = baseSprite->width;
	height = baseSprite->height;

	hitArea.reset(new HitArea(
		Vector2(position.x - width*scale.x / 2, position.y - height*scale.y / 2),
		Vector2(width*scale.x, height*scale.y)));
}

void Sprite::setPosition(const Vector2& pos) {

	position = pos;
	hitArea->position = Vector2(position.x - width*scale.x / 2,
		position.y - height*scale.y / 2);
	hitArea->size = Vector2(width*scale.x, height*scale.y);
}


void Sprite::setOrigin(const Vector2& orgn) {

	origin = orgn;
}


void Sprite::setScale(const Vector2& scl) {

	scale = scl;
	setPosition(position);
}


void Sprite::setRotation(const float rot) {

	rotation = rot;
}


void Sprite::setTint(const Color& colr) {

	tint = colr;
}


void Sprite::setAlpha(const float alph) {

	alpha = alph;
}


