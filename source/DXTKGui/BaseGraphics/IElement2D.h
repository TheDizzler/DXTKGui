#pragma once

#include <wrl.h>
#include <d3d11_1.h>
#include <SimpleMath.h>
#include <vector>
#include "SpriteBatch.h"



using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace Microsoft::WRL;

interface IElement2D {
public:

	virtual const Vector2& getPosition() const = 0;
	virtual const Vector2& getOrigin() const = 0;
	virtual const Vector2& getScale() const = 0;
	virtual const float getRotation() const = 0;
	virtual const Color& getTint() const = 0;
	virtual const float getAlpha() const = 0;
	virtual const int getWidth() const = 0;
	virtual const int getHeight() const = 0;
	virtual const float getLayerDepth() const = 0;

	/** Get coordinates of object relative to screen. */
	//virtual const Vector2& getScreenPosition() const = 0;
	//virtual const HitArea& getScreenHitArea() const = 0;

	virtual void moveBy(const Vector2& moveVector) = 0;
	virtual void setPosition(const Vector2& position) = 0;
	virtual void setOrigin(const Vector2& origin) = 0;
	virtual void setScale(const Vector2& scale) = 0;
	virtual void setRotation(const float rotation) = 0;
	virtual void setTint(const XMFLOAT4 color) = 0;
	virtual void setAlpha(const float alpha) = 0;
	virtual void setLayerDepth(const float depth) = 0;

	virtual void draw(SpriteBatch* batch) = 0;

	//virtual GraphicsAsset* createTexture() = 0;
};

