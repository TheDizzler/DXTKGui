#include "PrimitiveShapes.h"

RectangleSprite::RectangleSprite(GraphicsAsset* const graphicsAsset)
	: Sprite() {

	Sprite::load(graphicsAsset);
	origin = Vector2(0, 0);
}

RectangleSprite::RectangleSprite(ComPtr<ID3D11ShaderResourceView> pixel,
	const Vector2& pos, const Vector2 & size) : Sprite(pos) {

	texture = pixel;
	setDimensions(pos, size);
}

RectangleSprite::~RectangleSprite() {
}

const Vector2 RectangleSprite::getSize() const {
	return Vector2(width, height);
}


void RectangleSprite::moveBy(const Vector2& moveVector) {

	position += moveVector;
	hitArea->position = position;
}




RectangleFrame::RectangleFrame(GraphicsAsset* pixelAsset) {

	//if (pixelAsset != NULL)
		pixel = pixelAsset->getTexture();
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
	int frmThcknss) {

	frameThickness = frmThcknss;
	Vector2 position = pos;


	// upper horizontal frame
	frameHorizontal.left = 0;
	frameHorizontal.top = 0;
	frameHorizontal.right = size.x;
	frameHorizontal.bottom = frameThickness; // thickness of frame
	frameTopPos = Vector2(position.x, position.y);


	// lower horizontal frame
	int height = size.y;
	frameBottomPos = frameTopPos;
	frameBottomPos.y += height - frameThickness;
	// frame sticks out passed rectangle area; (-frameThickness) pulls it back in

	// left vertical frame
	frameLeftPos = frameBottomPos;
	frameLeftPos.y = position.y;
	frameVertical = frameHorizontal;
	frameVertical.right = frameThickness;
	frameVertical.bottom = height;


	// right vertical frame
	frameRightPos = frameLeftPos;
	frameRightPos.x += size.x - frameThickness;
	// frame sticks out passed rectangle area; (-frameThickness) pulls it back in


}

void RectangleFrame::setPosition(const Vector2& newPosition) {

	frameTopPos = newPosition;
	frameBottomPos = newPosition;
	frameLeftPos = newPosition;
	frameRightPos = newPosition;
}

void RectangleFrame::moveBy(const Vector2& moveVector) {

	frameTopPos += moveVector;
	frameBottomPos += moveVector;
	frameLeftPos += moveVector;
	frameRightPos += moveVector;
}

void RectangleFrame::setTint(const Color& color) {

	tint = color;
}

void RectangleFrame::draw(SpriteBatch* batch) {

	// draw top horizontal bar
	batch->Draw(pixel.Get(), frameTopPos, &frameHorizontal,
		tint, rotation, origin, scale,
		SpriteEffects_None, 0.0f);

	// draw bottom horizontal bar
	batch->Draw(pixel.Get(), frameBottomPos, &frameHorizontal,
		tint, rotation, origin, scale,
		SpriteEffects_None, 0.0f);

	// draw left vertical bar
	batch->Draw(pixel.Get(), frameLeftPos, &frameVertical,
		tint, rotation, origin, scale,
		SpriteEffects_None, 0.0f);

	// draw right vertical bar
	batch->Draw(pixel.Get(), frameRightPos, &frameVertical,
		tint, rotation, origin, scale,
		SpriteEffects_None, 0.0f);
}

const Vector2 & RectangleFrame::getPosition() const {
	return frameTopPos;
}

const int RectangleFrame::getWidth() const {
	return frameHorizontal.right;
}

const int RectangleFrame::getHeight() const {
	return frameVertical.bottom;
}

const int RectangleFrame::getThickness() const {
	return frameThickness;
}

const Vector2 & RectangleFrame::getOrigin() const {
	return origin;
}

const Vector2 & RectangleFrame::getScale() const {
	return scale;
}

const float RectangleFrame::getRotation() const {
	return rotation;
}

const Color & RectangleFrame::getTint() const {
	return tint;
}

const float RectangleFrame::getAlpha() const {
	return 0.0f;
}

void RectangleFrame::setOrigin(const Vector2& orgn) {
	origin = orgn;
}

void RectangleFrame::setScale(const Vector2& scl) {
	scale = scl;
}

void RectangleFrame::setRotation(const float rot) {
	rotation = rot;
}

void RectangleFrame::setAlpha(const float alpha) {

}




Line::Line(ComPtr<ID3D11ShaderResourceView> whitePixel) {

	pixel = whitePixel;
}

Line::Line(ComPtr<ID3D11ShaderResourceView> whitePixel,
	const Vector2& pos, const Vector2& size) {

	pixel = whitePixel;
	setDimensions(pos, size);
}

Line::~Line() {
}

void Line::setDimensions(const Vector2& pos, const Vector2& size) {

	Vector2 position = pos;
	/*position.x -= size.x / 2;
	position.y -= size.y / 2;*/

	lineRect.left = 0;
	lineRect.top = 0;
	lineRect.right = size.x;
	lineRect.bottom = size.y;
}

void Line::setTint(const Color& color) {
	tint = color;
}

void Line::draw(SpriteBatch* batch) {

	batch->Draw(pixel.Get(), position, &lineRect,
		tint, 0.0f, Vector2(0, 0), Vector2(1, 1),
		SpriteEffects_None, 0.0f);
}
