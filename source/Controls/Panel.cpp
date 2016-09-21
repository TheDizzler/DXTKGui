#include "Panel.h"

#include "../Engine/GameEngine.h"
Panel::Panel(GraphicsAsset* pixelAsset) {
	//bgSprite.reset(new RectangleSprite(pixelAsset));
	hitArea.reset(new HitArea(Vector2::Zero, Vector2::Zero));
}
Panel::~Panel() {
}


void Panel::setTexture(ComPtr<ID3D11ShaderResourceView> tex) {
	texture = tex;
}

void Panel::update(double deltaTime, MouseController* mouse) {

}

void Panel::draw(SpriteBatch* batch) {

	//bgSprite->draw(batch);
	if (texture != NULL)
		batch->Draw(texture.Get(), position, &viewRect, Color(1,1,1), rotation,
			origin, scale, SpriteEffects_None, layerDepth);
}

void Panel::setDimensions(const Vector2& pos, const Vector2& size) {


	//bgSprite->setDimensions(pos, size);
	//bgSprite->setTint(Vector4(.5, 1, 1, 1));
	viewRect.left = pos.x;
	viewRect.top = pos.y /*- 25*/;
	viewRect.right = pos.x + size.x;
	viewRect.bottom = pos.y /*-25 */+ size.y;

	hitArea->size = size;
	setPosition(pos);

}


void Panel::moveBy(const Vector2 & moveVector) {
	GUIControl::moveBy(moveVector);
	//bgSprite->moveBy(moveVector);
}

const Vector2& Panel::getPosition() const {
	return position;
}

const int Panel::getWidth() const {
	//return bgSprite->getWidth();
	return hitArea->size.x;
}

const int Panel::getHeight() const {
	//return bgSprite->getHeight();
	return hitArea->size.y;
}

bool Panel::clicked() {
	return isClicked;
}

bool Panel::pressed() {
	return isPressed;
}

bool Panel::hovering() {
	return isHover;
}


void Panel::setFont(const pugi::char_t* font) {
}

void Panel::setText(wstring text) {
}

XMVECTOR XM_CALLCONV Panel::measureString() const {
	return Vector2::Zero;
}
