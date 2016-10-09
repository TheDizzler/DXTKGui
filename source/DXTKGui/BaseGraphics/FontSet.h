#pragma once

#include <SpriteFont.h>


#include "IElement2D.h"


/* A wrapper class for more convenient use of SpriteFonts.
	Default font color is White. */
class FontSet {
public:
	FontSet();
	~FontSet();

	virtual void load(ComPtr<ID3D11Device> device, const wchar_t* file);

	XMVECTOR XM_CALLCONV measureString(_In_z_ wchar_t const* text) const;

	virtual const Vector2& getOrigin() const;
	virtual const Vector2& getScale() const;
	virtual const Color& getTint() const;
	virtual const float getAlpha() const;


	virtual void setOrigin(const Vector2& origin);
	virtual void setScale(const Vector2& scale);
	virtual void setTint(const XMFLOAT4 color);
	virtual void setAlpha(const float alpha);

	/** Don't use this draw function except for testing!! */
	virtual void draw(SpriteBatch* batch);
	void draw(SpriteBatch* batch, const wchar_t* text, const Vector2& position,
		const Color& newTint, float rotation, const Vector2& origin, const Vector2& scale);

protected:

	std::unique_ptr<SpriteFont> font;

	wchar_t* defaultText = L"You are calling the wrong draw function, sire.";

	Vector2 origin;
	Vector2 scale;
	Color tint;
	float layerDepth;


};

