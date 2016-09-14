#include "../pch.h"
#pragma once

#include <d3d11_1.h>
#include <SimpleMath.h>
#include "SpriteBatch.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;


interface IElement2D {
public:

	virtual const Vector2& getPosition() const = 0;
	virtual const Vector2& getOrigin() const = 0;
	virtual const Vector2& getScale() const = 0;
	virtual const float getRotation() const = 0;
	virtual const Color& getTint() const = 0;
	virtual const float getAlpha() const = 0;
	virtual const int getWidth() const = 0;
	virtual const int getHeight() const =0 ;

	virtual void moveBy(const Vector2& moveVector) = 0;
	virtual void setPosition(const Vector2& position) = 0;
	virtual void setOrigin(const Vector2& origin) = 0;
	virtual void setScale(const Vector2& scale) = 0;
	virtual void setRotation(const float rotation) = 0;
	virtual void setTint(const Color& color) = 0;
	virtual void setAlpha(const float alpha) = 0;

	virtual void draw(SpriteBatch* batch) = 0;


};

