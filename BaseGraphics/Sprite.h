#pragma once

#include "GraphicsAsset.h"


struct HitArea {

	HitArea(Vector2 pos = Vector2::Zero, Vector2 sz = Vector2::Zero)
		: position(pos), size(sz) {
	}
	HitArea(const HitArea* toCopy) {
		position = toCopy->position;
		size = toCopy->size;
	}

	bool collision(_In_ const HitArea* other) const {

		if (position.x < other->position.x + other->size.x
			&& position.x + size.x > other->position.x
			&& position.y < other->position.y + other->size.y
			&& position.y + size.y > other->position.y) {

			return true;
		}
		return false;
	}

	bool collision(_In_ const HitArea& other) const {

		if (position.x < other.position.x + other.size.x
			&& position.x + size.x > other.position.x
			&& position.y < other.position.y + other.size.y
			&& position.y + size.y > other.position.y) {

			return true;
		}
		return false;
	}

	bool contains(const Vector2& point) const {

		return position.x < point.x && position.x + size.x > point.x
			&& position.y < point.y && position.y + size.y > point.y;
	}

	Vector2 position; // top left position NOT center/origin
	Vector2 size; // (width, height)
};

class GUIFactory;
class Sprite : public IElement2D {
public:
	Sprite();
	Sprite(const Vector2& position);
	virtual ~Sprite();

	/* GraphicsAsset is not stored in Sprite. Note: scale should be set after this.*/
	virtual void load(GraphicsAsset* const graphicsAsset);
	virtual void reloadGraphicsAsset(GUIFactory* guiFactory);

	const HitArea& getHitArea() const;
	virtual const Vector2& getPosition() const override;
	virtual const Vector2& getOrigin() const override;
	virtual const Vector2& getScale() const override;
	virtual const float getRotation() const override;
	virtual const Color& getTint() const override;
	virtual const float getAlpha() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;
	virtual const RECT getRect() const;
	virtual const float getLayerDepth() const override;


	virtual void setDimensions(Sprite* baseSprite);
	virtual void setDimensions(const Vector2& position, const Vector2& size);
	virtual void setSize(const Vector2& size);
	virtual void setPosition(const Vector2& position) override;
	virtual void setOrigin(const Vector2& origin) override;
	virtual void setOriginCenter();
	virtual void setScale(const Vector2& scale) override;
	virtual void setRotation(const float rotation) override;
	virtual void setTint(const XMFLOAT4 color) override;
	virtual void setTint(const Color& color) override;
	virtual void setTint(const XMVECTORF32 color) override;
	virtual void setAlpha(const float alpha) override;
	/** bool frontToBack has no effect in Sprite. */
	virtual void setLayerDepth(const float depth, bool frontToBack = true) override;

	virtual void moveBy(const Vector2& moveVector);
	virtual void rotateBy(float rotateAmount);

	virtual void draw(SpriteBatch* batch) override;

	virtual bool contains(const Vector2& point) override;

	ComPtr<ID3D11ShaderResourceView> getTexture();

	bool isAlive = true;


protected:
	string assetName;
	ComPtr<ID3D11ShaderResourceView> texture;
	bool isPixel = false;

	RECT sourceRect;

	Vector2 origin;
	Color tint;
	float rotation;
	float layerDepth;

	UINT width;
	UINT height;
	Vector2 position;
	Vector2 scale;
	SpriteEffects spriteEffect = SpriteEffects_None;
	HitArea hitArea;
};




