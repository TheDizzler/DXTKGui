#include "RectangleSprite.h"

RectangleSprite::RectangleSprite(ComPtr<ID3D11ShaderResourceView> pixel)
	: Sprite() {

	texture = pixel;
}

RectangleSprite::RectangleSprite(ComPtr<ID3D11ShaderResourceView> pixel,
	const Vector2& pos, const Vector2 & size) : Sprite(pos) {

	setDimensions(pos, size);
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




RectangleFrame::RectangleFrame(ComPtr<ID3D11ShaderResourceView> pxl) {

	pixel = pxl;
}

RectangleFrame::RectangleFrame(ComPtr<ID3D11ShaderResourceView> pxl,
	const Vector2& position, const Vector2& size, int frameThickness) {

	pixel = pxl;

	setDimensions(position, size, frameThickness);

	tint = DirectX::Colors::Black.v;
}

RectangleFrame::~RectangleFrame() {
}

void RectangleFrame::setDimensions(const Vector2& pos, const Vector2& size,
	int frameThickness) {

	Vector2 position = pos;
	position.x -= size.x/2;
	position.y -= size.y/2;
	// upper horizontal frame
	frameHorizontal.left = 0;
	frameHorizontal.top = 0;
	frameHorizontal.right = size.x;
	frameHorizontal.bottom = frameThickness; // thickness of frame
	frameTopPos = Vector2(position.x, position.y);


	// lower horizontal frame
	int height = size.y;
	frameBottomPos = frameTopPos;
	frameBottomPos.y += height;


	// left vertical frame
	frameLeftPos = frameBottomPos;
	frameLeftPos.y = position.y;
	frameVertical = frameHorizontal;
	frameVertical.right = frameThickness;
	frameVertical.bottom = height;


	// right vertical frame
	frameRightPos = frameLeftPos;
	frameRightPos.x += size.x;


}

void RectangleFrame::setTint(const Color& color) {

	tint = color;
}

void RectangleFrame::draw(SpriteBatch* batch) {

	// draw top horizontal bar
	batch->Draw(pixel.Get(), frameTopPos, &frameHorizontal,
		tint, 0.0f, Vector2(0, 0), Vector2(1, 1),
		SpriteEffects_None, 0.0f);

	// draw bottom horizontal bar
	batch->Draw(pixel.Get(), frameBottomPos, &frameHorizontal,
		tint, 0.0f, Vector2(0, 0), Vector2(1, 1),
		SpriteEffects_None, 0.0f);

	// draw left vertical bar
	batch->Draw(pixel.Get(), frameLeftPos, &frameVertical,
		tint, 0.0f, Vector2(0, 0), Vector2(1, 1),
		SpriteEffects_None, 0.0f);

	// draw right vertical bar
	batch->Draw(pixel.Get(), frameRightPos, &frameVertical,
		tint, 0.0f, Vector2(0, 0), Vector2(1, 1),
		SpriteEffects_None, 0.0f);
}
