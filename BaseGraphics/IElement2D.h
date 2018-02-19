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


	virtual void moveBy(const Vector2& moveVector) = 0;
	virtual void setPosition(const Vector2& position) = 0;
	virtual void setOrigin(const Vector2& origin) = 0;
	virtual void setScale(const Vector2& scale) = 0;
	virtual void setRotation(const float rotation) = 0;
	virtual void setTint(const XMFLOAT4 color) = 0;
	virtual void setTint(const Color& color) = 0;
	virtual void setTint(const XMVECTORF32 color) = 0;
	virtual void setAlpha(const float alpha) = 0;
	virtual void setLayerDepth(const float depth, bool frontToBack = true) = 0;

	virtual bool contains(const Vector2& point) = 0;

	virtual void draw(SpriteBatch* batch) = 0;

};

class GraphicsAsset;
interface Texturizable {
public:

	/* Force a refresh of the texture on the objects next update.
		In general, this is unnecessary. If it is required, that is likely an 
		indication of a bug. */
	virtual void forceRefresh() = 0;
	/* Call this when you want this object to be texturized. */
	virtual unique_ptr<GraphicsAsset> texturize() = 0;
	/* Called by GUIFactory::createTextureFromTexturizable(). Generally should not be called otherwise. */
	virtual void textureDraw(SpriteBatch* batch, ComPtr<ID3D11Device> device = NULL) = 0;

	virtual void setPosition(const Vector2& position) = 0;
	virtual const Vector2& getPosition() const = 0;
	virtual const int getWidth() const = 0;
	virtual const int getHeight() const = 0;
};

