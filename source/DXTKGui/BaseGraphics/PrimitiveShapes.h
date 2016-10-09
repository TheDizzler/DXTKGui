#pragma once

#include "Sprite.h"

/** For drawing a primitive, filled rectangle. 
	Unlike other sprites, the origin of a Rectangle Sprite is the top left
	corner (0, 0) to be consistent with GUIControls. */
class RectangleSprite : public Sprite {
public:
	RectangleSprite(GraphicsAsset* const graphicsAsset);
	RectangleSprite(ComPtr<ID3D11ShaderResourceView> pixel,
		const Vector2& position, const Vector2& size);
	~RectangleSprite();

	const Vector2 getSize() const;
	/* position is top left corner. */
	virtual void moveBy(const Vector2& moveVector);
};

/** A primitive, non-filled rectangle. */
class RectangleFrame : public IElement2D {
public:
	RectangleFrame(GraphicsAsset* pixelAsset);
	//RectangleFrame(ComPtr<ID3D11ShaderResourceView> pixel,
		//const Vector2& pos, const Vector2& size, int frameThickness);
	~RectangleFrame();

	void setDimensions(const Vector2& position, const Vector2& size,
		int frameThickness = 2);
	void refreshDimensions();

	void setPosition(const Vector2& newPosition);
	void moveBy(const Vector2& moveVector);

	void setTint(const XMFLOAT4 color);

	void draw(SpriteBatch* batch);

	const Vector2& getPosition() const;
	const int getWidth() const;
	const int getHeight() const;
	const int getThickness() const;

	virtual const Vector2& getOrigin() const override;
	virtual const Vector2& getScale() const override;
	virtual const float getRotation() const override;
	virtual const Color& getTint() const override;
	virtual const float getAlpha() const override;
	virtual void setOrigin(const Vector2& origin) override;
	virtual void setScale(const Vector2& scale) override;
	virtual void setRotation(const float rotation) override;
	virtual void setAlpha(const float alpha) override;

	bool contains(const Vector2& point);
private:
	ComPtr<ID3D11ShaderResourceView> pixel;


	RECT frameHorizontal;
	RECT frameVertical;
	Vector2 frameTopPos;
	Vector2 frameLeftPos;
	Vector2 frameRightPos;
	Vector2 frameBottomPos;

	Vector2 origin = Vector2(0, 0);
	Vector2 scale = Vector2(1, 1);
	float rotation = 0.0f;
	Color tint = Colors::Black;
	int frameThickness;

	unique_ptr<HitArea> hitArea;
	
};


/** A primitive line.
	This has not been tested.*/
class Line {
public:
	Line(ComPtr<ID3D11ShaderResourceView> whitePixel);
	Line(ComPtr<ID3D11ShaderResourceView> whitePixel,
		const Vector2& pos, const Vector2& size);
	~Line();

	void setDimensions(const Vector2& position, const Vector2& size);

	void setTint(const Color& color);

	void draw(SpriteBatch* batch);

private:
	ComPtr<ID3D11ShaderResourceView> pixel;

	RECT lineRect;
	Vector2 position;

	Color tint;
};