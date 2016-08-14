#pragma once

#include <d3d11.h>
#include <SimpleMath.h>
#include "SpriteBatch.h"
#include "../assets.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;


class IElement2D {
public:


	virtual bool load(ID3D11Device* device, const wchar_t* file) = 0;

	virtual const Vector2& getPosition() const = 0;
	virtual const Vector2& getOrigin() const = 0;
	virtual const Vector2& getScale() const = 0;
	virtual const float getRotation() const = 0;
	virtual const Color& getTint() const = 0;
	virtual const float getAlpha() const = 0;

	virtual void setPosition(const Vector2& position) = 0;
	virtual void setOrigin(const Vector2& origin) = 0;
	virtual void setScale(const Vector2& scale) = 0;
	virtual void setRotation(const float rotation) = 0;
	virtual void setTint(const Color& color) = 0;
	virtual void setAlpha(const float alpha) = 0;

	virtual void draw(SpriteBatch* batch) = 0;


};

