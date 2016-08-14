#pragma once

#include "Sprite.h"

/** For drawing a primitive filled rectangle. */
class RectangleSprite : public Sprite {
public:
	RectangleSprite(ID3D11ShaderResourceView* pixel);
	~RectangleSprite();

	virtual void setDimensions(const Vector2& position, const Vector2& size);
	//void setSize(const Vector2& size);
private:

};