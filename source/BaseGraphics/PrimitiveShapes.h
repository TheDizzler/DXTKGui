#pragma once

#include "Sprite.h"

/** For drawing a primitive, filled rectangle. 
	Unlike other sprites, the origin of a Rectangle Sprite is the top left
	corner (0, 0) to be consistent with GUIControls. */
class RectangleSprite : public Sprite {
public:
	RectangleSprite(ComPtr<ID3D11ShaderResourceView> pixel);
	RectangleSprite(ComPtr<ID3D11ShaderResourceView> pixel,
		const Vector2& position, const Vector2& size);
	~RectangleSprite();

	/* position is top left corner. */
	void setDimensions(const Vector2& position, const Vector2& size);
	virtual void moveBy(const Vector2& moveVector);
};

/** A primitive, non-filled rectangle. */
class RectangleFrame {
public:
	RectangleFrame(ComPtr<ID3D11ShaderResourceView> pixel);
	RectangleFrame(ComPtr<ID3D11ShaderResourceView> pixel,
		const Vector2& pos, const Vector2& size, int frameThickness);
	~RectangleFrame();

	void setDimensions(const Vector2& position, const Vector2& size,
		int frameThickness);

	void setPosition(const Vector2& newPosition);
	void moveBy(const Vector2& moveVector);

	void setTint(const Color& color);

	void draw(SpriteBatch* batch);


	int getThickness();

private:
	ComPtr<ID3D11ShaderResourceView> pixel;


	RECT frameHorizontal;
	RECT frameVertical;
	Vector2 frameTopPos;
	Vector2 frameLeftPos;
	Vector2 frameRightPos;
	Vector2 frameBottomPos;

	Color tint;
	int frameThickness;
};


/** A primitive live.
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