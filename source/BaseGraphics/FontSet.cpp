#include "FontSet.h"


FontSet::FontSet() {

	alpha = 1.0f;
	rotation = 0.0f;
	scale = Vector2(1, 1);
	tint = DirectX::Colors::White.v;
	layerDepth = 1.0f;
}

FontSet::~FontSet() {
}


bool FontSet::load(ID3D11Device* device, const wchar_t* file) {

	font.reset(new SpriteFont(device, file));


	return true;
}

XMVECTOR XM_CALLCONV FontSet::measureString(wchar_t const * text) const {

	Vector2 measurement = font->MeasureString(text);
	measurement.x *= scale.x;
	measurement.y *= scale.y;
	return measurement;
}


void FontSet::draw(SpriteBatch* batch, const wchar_t* text, Vector2 pos) {

	font->DrawString(batch, text, pos, tint, rotation, origin, scale, SpriteEffects_None, layerDepth);
}

void FontSet::draw(SpriteBatch* batch, const wchar_t* text, Vector2 pos, Color newTint) {

	font->DrawString(batch, text, pos, newTint, rotation, origin, scale, SpriteEffects_None, layerDepth);
}


/** Don't use this draw function except for testing!! */
void FontSet::draw(SpriteBatch* batch) {

	font->DrawString(batch, defaultText, Vector2(0, 0));

}


const Vector2& FontSet::getPosition() const {

	return Vector2(0, 0);
}


const Vector2& FontSet::getOrigin() const {

	return origin;
}

const Vector2& FontSet::getScale() const {

	return scale;
}

const float FontSet::getRotation() const {
	return rotation;
}


const Color& FontSet::getTint() const {

	return tint;
}


const float FontSet::getAlpha() const {

	return alpha;
}


void FontSet::setPosition(const Vector2& position) {

}


void FontSet::setOrigin(const Vector2& origin) {


}


void FontSet::setScale(const Vector2& scl) {

	scale = scl;
}


void FontSet::setRotation(const float rot) {

	rotation = rot;
}


void FontSet::setTint(const Color& color) {

	tint = color;
}


void FontSet::setAlpha(const float alph) {

	alpha = alph;
}
