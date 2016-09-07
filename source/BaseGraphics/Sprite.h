#pragma once

#include "GraphicsAsset.h"


struct HitArea {

	HitArea(Vector2 pos, Vector2 sz) : position(pos), size(sz) {
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

	bool contains(const Vector2& point) const {

		return position.x < point.x && position.x + size.x > point.x
			&& position.y < point.y && position.y + size.y > point.y;
	}

	Vector2 position; // top left position NOT center/origin
	Vector2 size; // (width, height)
};


class Sprite : public IElement2D {
public:
	Sprite();
	Sprite(const Vector2& position);
	virtual ~Sprite();

	virtual void load(GraphicsAsset* const graphicsAsset);

	const HitArea* getHitArea() const;
	virtual const Vector2& getPosition() const override;
	virtual const Vector2& getOrigin() const override;
	virtual const Vector2& getScale() const override;
	virtual const float getRotation() const override;
	virtual const Color& getTint() const override;
	/* No alpha implementation. */
	virtual const float getAlpha() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;
	virtual const RECT getRect() const;

	virtual void moveBy(const Vector2& moveVector);
	virtual void setDimensions(Sprite* baseSprite);
	virtual void setPosition(const Vector2& position) override;
	virtual void setOrigin(const Vector2& origin) override;
	virtual void setScale(const Vector2& scale) override;
	virtual void setRotation(const float rotation) override;
	virtual void setTint(const Color& color) override;
	/* No alpha implementation. */
	virtual void setAlpha(const float alpha) override;

	virtual void update(double deltaTime);
	virtual void draw(SpriteBatch* batch) override;

	
	ComPtr<ID3D11ShaderResourceView> getTexture();
	//ComPtr<ID3D11Resource> getResource();


	bool isAlive = true;


protected:
	ComPtr<ID3D11ShaderResourceView> texture;
	//ComPtr<ID3D11Resource> resource;
	
	RECT sourceRect;

	Vector2 origin;
	Color tint;
	float alpha;
	float rotation;
	float layerDepth;

	UINT width;
	UINT height;
	Vector2 position;
	
	Vector2 scale;
	std::unique_ptr<HitArea> hitArea;
};




