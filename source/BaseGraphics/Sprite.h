#pragma once

#include "IElement2D.h"


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

	
	Vector2 origin;
	Color tint;
	float alpha;
	float rotation;
	float layerDepth;

	
	RECT sourceRect;

	virtual bool load(ID3D11Device* device, const wchar_t* file);

	const HitArea* getHitArea() const;
	virtual const Vector2& getPosition() const;
	virtual const Vector2& getOrigin() const;
	virtual const Vector2& getScale() const;
	virtual const float getRotation() const;
	virtual const Color& getTint() const;
	virtual const float getAlpha() const;
	virtual const RECT getRect() const;
	const int getWidth() const;
	const int getHeight() const;

	//virtual void setHitArea(const HitArea* hitarea);
	virtual void setDimensions(Sprite* baseSprite);
	virtual void setPosition(const Vector2& position);
	virtual void setOrigin(const Vector2& origin);
	virtual void setScale(const Vector2& scale);
	virtual void setRotation(const float rotation);
	virtual void setTint(const Color& color);
	virtual void setAlpha(const float alpha);

	virtual void update(double deltaTime);
	virtual void draw(SpriteBatch* batch);

	
	ComPtr<ID3D11ShaderResourceView> getTexture();
	ComPtr<ID3D11Resource> getResource();


	bool isAlive = true;


protected:
	ComPtr<ID3D11ShaderResourceView> texture;
	ComPtr<ID3D11Resource> resource;
	
	//ID3D11Resource* resource = 0;
	

	UINT width;
	UINT height;
	Vector2 position;
	
	Vector2 scale;
	std::unique_ptr<HitArea> hitArea;
};




