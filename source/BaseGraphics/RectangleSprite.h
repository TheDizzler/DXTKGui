#pragma once

#include "Sprite.h"

/** For drawing a primitive, filled rectangle. */
class RectangleSprite : public Sprite {
public:
	RectangleSprite(ComPtr<ID3D11ShaderResourceView> pixel);
	RectangleSprite(ComPtr<ID3D11ShaderResourceView> pixel,
		const Vector2& position, const Vector2& size);
	~RectangleSprite();

	void setDimensions(const Vector2& position, const Vector2& size);

};


class RectangleFrame {
public:
	RectangleFrame(ComPtr<ID3D11ShaderResourceView> pixel);
	RectangleFrame(ComPtr<ID3D11ShaderResourceView> pixel,
		const Vector2& pos, const Vector2& size, int frameThickness);
	~RectangleFrame();

	void setDimensions(const Vector2& position, const Vector2& size,
		int frameThickness);

	void setTint(const Color & color);

	void draw(SpriteBatch* batch);

private:
	ComPtr<ID3D11ShaderResourceView> pixel;

	RECT frameHorizontal;
	RECT frameVertical;
	Vector2 frameTopPos;
	Vector2 frameLeftPos;
	Vector2 frameRightPos;
	Vector2 frameBottomPos;

	Color tint;
};