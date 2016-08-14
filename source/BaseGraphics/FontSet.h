#pragma once

#include "IElement2D.h"
#include "SpriteFont.h"

class FontSet: public IElement2D {
public:
	FontSet();
	~FontSet();

	virtual bool load(ID3D11Device* device, const wchar_t* file);

	XMVECTOR XM_CALLCONV measureString(_In_z_ wchar_t const* text) const;

	virtual const Vector2& getPosition() const;
	virtual const Vector2& getOrigin() const;
	virtual const Vector2& getScale() const;
	virtual const float getRotation() const;
	virtual const Color& getTint() const;
	virtual const float getAlpha() const;

	virtual void setPosition(const Vector2& position);
	virtual void setOrigin(const Vector2& origin);
	virtual void setScale(const Vector2& scale);
	virtual void setRotation(const float rotation);
	virtual void setTint(const Color& color);
	virtual void setAlpha(const float alpha);

	/** Don't use this draw function except for testing!! */
	virtual void draw(SpriteBatch* batch);
	void draw(SpriteBatch* batch, const wchar_t* text, Vector2 position);
	void draw(SpriteBatch* batch, const wchar_t* text, Vector2 pos, Color newTint);

	float layerDepth;
protected:

	std::unique_ptr<SpriteFont> font;

	wchar_t* defaultText = L"You are calling the wrong draw function, sire.";

	/*ID3D11Resource* resource;
	ID3D11ShaderResourceView* texture;
	UINT width;
	UINT height;
	RECT sourceRect;*/

	//Vector2 position; // used only for testing
	Vector2 origin;
	Vector2 scale;
	Color tint;
	float alpha;
	float rotation;
	
};

