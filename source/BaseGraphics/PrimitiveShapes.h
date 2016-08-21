#pragma once

#include "Sprite.h"

/** For drawing a primitive, filled rectangle. */
class RectangleSprite : public Sprite {
public:
	RectangleSprite(ComPtr<ID3D11ShaderResourceView> pixel);
	RectangleSprite(ComPtr<ID3D11ShaderResourceView> pixel,
		const Vector2& position, const Vector2& size);
	~RectangleSprite();

	/* position is top left corner. */
	void setDimensions(const Vector2& position, const Vector2& size);

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


/** A primitive live. */
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