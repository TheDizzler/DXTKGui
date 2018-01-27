#include "FontSet.h"


FontSet::FontSet() {

}


FontSet::~FontSet() {
}


void FontSet::load(ComPtr<ID3D11Device> device, const wchar_t* file,
	const pugi::char_t* name) {

	fontName = name;
	font.reset(new SpriteFont(device.Get(), file));

}

XMVECTOR XM_CALLCONV FontSet::measureString(wchar_t const* text) const {

	Vector2 measurement = font->MeasureString(text);
	return measurement;
}


void FontSet::draw(SpriteBatch* batch, const wchar_t* text,
	const Vector2& position, const Color& newTint, float rotation,
	const Vector2& origin, const Vector2& scale, const float layerDepth) {

	font->DrawString(batch, text, position, newTint, rotation, origin,
		scale, SpriteEffects_None, layerDepth);
}


/** Don't use this draw function except for testing!! */
void FontSet::draw(SpriteBatch* batch) {
	font->DrawString(batch, defaultText, Vector2(0, 0));
}