#include "FontSet.h"


FontSet::FontSet() {

	scale = Vector2(1, 1);
	tint = Color(0, 0, 0, 1);
	layerDepth = 1.0f;
	origin = Vector2(0, 0);
}


FontSet::~FontSet() {
}


void FontSet::load(ComPtr<ID3D11Device> device, const wchar_t* file) {

	font.reset(new SpriteFont(device.Get(), file));

}

XMVECTOR XM_CALLCONV FontSet::measureString(wchar_t const* text) const {

	Vector2 measurement = font->MeasureString(text);
	return measurement * scale;
}


void FontSet::draw(SpriteBatch* batch, const wchar_t* text,
	const Vector2& position, const Color& newTint, float rotation,
	const Vector2& origin, const Vector2& scale) {

	font->DrawString(batch, text, position, newTint, rotation, origin,
		scale, SpriteEffects_None, layerDepth);
}


/** Don't use this draw function except for testing!! */
void FontSet::draw(SpriteBatch* batch) {
	font->DrawString(batch, defaultText, Vector2(0, 0));
}


const Vector2& FontSet::getOrigin() const {
	return origin;
}

const Vector2& FontSet::getScale() const {
	return scale;
}


const Color& FontSet::getTint() const {
	return tint;
}

const float FontSet::getAlpha() const {
	return tint.w;
}

void FontSet::setOrigin(const Vector2& org) {
	origin = org;
}

void FontSet::setScale(const Vector2& scl) {
	scale = scl;
}


void FontSet::setTint(const XMFLOAT4 color) {
	tint = color;
}

void FontSet::setAlpha(const float alpha) {
	tint.w = alpha;
}
