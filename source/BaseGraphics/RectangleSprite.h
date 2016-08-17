#pragma once

#include "Sprite.h"

/** For drawing a primitive filled rectangle. */
class RectangleSprite : public Sprite {
public:
	RectangleSprite(ComPtr<ID3D11ShaderResourceView> pixel);
	RectangleSprite(ComPtr<ID3D11ShaderResourceView> pixel,
		const Vector2& position, const Vector2& size);
	~RectangleSprite();

	void setDimensions(const Vector2& position, const Vector2& size);

private:

};