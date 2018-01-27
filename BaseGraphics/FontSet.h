#pragma once

#include <SpriteFont.h>
#include <pugixml.hpp>

#include "IElement2D.h"


/* A wrapper class for more convenient use of SpriteFonts.
	Default font color is White. */
class FontSet {
public:
	FontSet();
	virtual ~FontSet();

	virtual void load(ComPtr<ID3D11Device> device, const wchar_t* file,
		const pugi::char_t* fontName);

	XMVECTOR XM_CALLCONV measureString(_In_z_ wchar_t const* text) const;

	/** Don't use this draw function except for testing!! */
	virtual void draw(SpriteBatch* batch);
	void draw(SpriteBatch* batch, const wchar_t* text, const Vector2& position,
		const Color& newTint, float rotation, const Vector2& origin,
		const Vector2& scale, const float layerDepth = .91f);

	const pugi::char_t* fontName;
protected:

	unique_ptr<SpriteFont> font;

	wchar_t* defaultText = L"You are calling the wrong draw function, sire.";



};

