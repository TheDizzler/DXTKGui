#include "RectangleSprite.h"

RectangleSprite::RectangleSprite(ID3D11ShaderResourceView* pixel)
	: Sprite() {

	texture = pixel;
}

RectangleSprite::~RectangleSprite() {
}

void RectangleSprite::setDimensions(const Vector2& pos, const Vector2& size) {

	position = pos;
	
	width = size.x;
	height = size.y;
	origin = Vector2(width / 2.0f, height / 2.0f);

	/*position.x += width / 2;
	position.y += height /2;*/

	sourceRect.left = 0;
	sourceRect.top = 0;
	sourceRect.bottom = height;
	sourceRect.right = width;

	hitArea.reset(new HitArea(
		Vector2(position.x - width*scale.x / 2, position.y - height*scale.y / 2),
		Vector2(width*scale.x, height*scale.y)));
}


//void RectangleSprite::setSize(const Vector2& size) {
//
//	width = size.x;
//	height = size.y;
//	origin = Vector2(width / 2.0f, height / 2.0f);
//
//	sourceRect.left = 0;
//	sourceRect.top = 0;
//	sourceRect.bottom = height;
//	sourceRect.right = width;
//
//	hitArea.reset(new HitArea(
//		Vector2(position.x - width*scale.x / 2, position.y - height*scale.y / 2),
//		Vector2(width*scale.x, height*scale.y)));
//}
