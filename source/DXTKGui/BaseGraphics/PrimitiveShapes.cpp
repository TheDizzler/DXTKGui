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



/**** ***** RECTANGLE FRAME START ***** ****/
RectangleFrame::RectangleFrame(GraphicsAsset* pixelAsset) {

	pixel = pixelAsset->getTexture();
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
	frameHorizontal.right = size.x * scale.x;
	frameHorizontal.bottom = frameThickness; // thickness of frame
	frameTopPos = Vector2(position.x, position.y);


	// lower horizontal frame
	int height = size.y * scale.y;
	frameBottomPos = frameTopPos;
	frameBottomPos.y += height - frameThickness;
	// frame sticks out passed rectangle area; (-frameThickness) pulls it back in

	// left vertical frame
	frameLeftPos = frameBottomPos;
	frameLeftPos.y = position.y;
	frameVertical = frameHorizontal;
	frameVertical.right = frameThickness;
	frameVertical.bottom = height * scale.y;


	// right vertical frame
	frameRightPos = frameLeftPos;
	frameRightPos.x += size.x - frameThickness;
	// frame sticks out passed rectangle area; (-frameThickness) pulls it back in

	hitArea.reset(new HitArea(pos, size * scale));

}

void RectangleFrame::setSize(const Vector2& size) {

	setDimensions(frameTopPos, size);
}

bool cyberGrow = true;
void RectangleFrame::refreshDimensions() {

	if (!cyberGrow) {

		frameBottomPos = frameTopPos;
		frameBottomPos.y += getHeight() *scale.y - frameThickness;
		frameLeftPos = frameTopPos;
		frameRightPos = frameTopPos;
		frameRightPos.x += getWidth() *scale.x - frameThickness;

	}
	hitArea->size = Vector2(getWidth()*scale.x, getHeight()*scale.y);
	hitArea->position = frameTopPos;

}


void RectangleFrame::draw(SpriteBatch* batch) {

	// draw top horizontal bar
	batch->Draw(pixel.Get(), frameTopPos, &frameHorizontal,
		tint, rotation, origin, scale,
		SpriteEffects_None, layerDepth);

	// draw bottom horizontal bar
	batch->Draw(pixel.Get(), frameBottomPos, &frameHorizontal,
		tint, rotation, origin, scale,
		SpriteEffects_None, layerDepth);

	// draw left vertical bar
	batch->Draw(pixel.Get(), frameLeftPos, &frameVertical,
		tint, rotation, origin, scale,
		SpriteEffects_None, layerDepth);

	// draw right vertical bar
	batch->Draw(pixel.Get(), frameRightPos, &frameVertical,
		tint, rotation, origin, scale,
		SpriteEffects_None, layerDepth);
}

void RectangleFrame::setPosition(const Vector2& newPosition) {

	frameTopPos = newPosition;
	frameBottomPos = newPosition;
	frameBottomPos.y += getHeight() - frameThickness;
	frameLeftPos = newPosition;
	frameRightPos = newPosition;
	frameRightPos.x += getWidth() - frameThickness;
	hitArea->position = newPosition;
}

void RectangleFrame::moveBy(const Vector2& moveVector) {

	frameTopPos += moveVector;
	frameBottomPos += moveVector;
	frameLeftPos += moveVector;
	frameRightPos += moveVector;
	hitArea->position += moveVector;
}


const Vector2& RectangleFrame::getPosition() const {
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

const float RectangleFrame::getLayerDepth() const {
	return layerDepth;
}

const Vector2& RectangleFrame::getOrigin() const {
	return origin;
}

void RectangleFrame::setTint(const XMFLOAT4 color) {
	tint = color;
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
	return tint.w;
}

void RectangleFrame::setOrigin(const Vector2& orgn) {
	origin = orgn;
}

void RectangleFrame::setScale(const Vector2& scl) {

	scale = scl;
	refreshDimensions();
}

void RectangleFrame::setRotation(const float rot) {
	rotation = rot;
}

void RectangleFrame::setAlpha(const float alpha) {
	tint.w = alpha;
}

void RectangleFrame::setLayerDepth(const float depth) {
	layerDepth = depth;
}

bool RectangleFrame::contains(const Vector2& point) {

	return hitArea->contains(point);
}
/** **** END RECTANGLEFRAME ****** **/




/** ***** LINE START ***** **/
Line::Line(GraphicsAsset* pixelAsset) {

	pixel = pixelAsset->getTexture();
}

Line::Line(GraphicsAsset* pixelAsset,
	const Vector2& pos, const Vector2& size) {

	pixel = pixelAsset->getTexture();
	setDimensions(pos, size);
}

Line::~Line() {
}

const float Line::getRotation() const {
	return rotation;
}

void Line::setDimensions(const Vector2& pos, const Vector2& size) {

	position = pos;

	lineRect.left = 0;
	lineRect.top = 0;
	lineRect.right = size.x;
	lineRect.bottom = size.y;
}

void Line::setRotation(const float rot) {
	rotation = rot;
}

void Line::setTint(const Color& color) {
	tint = color;

}

void Line::draw(SpriteBatch* batch) {

	batch->Draw(pixel.Get(), position, &lineRect,
		tint, rotation, Vector2(0, 0), scale,
		SpriteEffects_None, layerDepth);
}
/** ***** END LINE ***** **/



/** ***** TRIANGLEFRAME START ***** **/
TriangleFrame::TriangleFrame(GraphicsAsset* pixelAsset) {

	pixel = pixelAsset->getTexture();
}

TriangleFrame::~TriangleFrame() {
}

void TriangleFrame::setDimensions(const Vector2& p1, const Vector2& p2,
	const Vector2& p3, USHORT thickness) {

	point1 = p1;
	point2 = p2;
	point3 = p3;

	angle1to2 = atan2(point1.y - point2.y, point1.x - point2.x) + XM_PI + rotation;
	angle2to3 = atan2(point2.y - point3.y, point2.x - point3.x) + XM_PI + rotation;
	angle3to1 = atan2(point3.y - point1.y, point3.x - point1.x) + XM_PI + rotation;


	originLine1 = Vector2::Zero;
	originLine2 = originLine1;
	originLine3 = originLine1;

	lengthBuffer = 0;
	Vector2 diff = point1 - point2;
	float length = sqrt(diff.x * diff.x + diff.y * diff.y);
	lineRECT1.left = 0;
	lineRECT1.top = 0;
	lineRECT1.right = length + lengthBuffer;
	lineRECT1.bottom = thickness;

	diff = point2 - point3;
	length = sqrt(diff.x * diff.x + diff.y * diff.y);
	lineRECT2.left = 0;
	lineRECT2.top = 0;
	lineRECT2.right = length + lengthBuffer;
	lineRECT2.bottom = thickness;

	diff = point3 - point1;
	length = sqrt(diff.x * diff.x + diff.y * diff.y);
	lineRECT3.left = 0;
	lineRECT3.top = 0;
	lineRECT3.right = length + lengthBuffer;
	lineRECT3.bottom = thickness;
}

void TriangleFrame::draw(SpriteBatch* batch) {

	batch->Draw(pixel.Get(), point1, &lineRECT1,
		tint, angle1to2, originLine1, scale,
		SpriteEffects_None, layerDepth);

	batch->Draw(pixel.Get(), point2, &lineRECT2,
		tint, angle2to3, originLine2, scale,
		SpriteEffects_None, layerDepth);

	batch->Draw(pixel.Get(), point3, &lineRECT3,
		tint, angle3to1, originLine3, scale,
		SpriteEffects_None, layerDepth);
}


const Vector2& TriangleFrame::getPosition() const {
	return point1;
}

const Vector2& TriangleFrame::getOrigin() const {
	return origin;
}

const Vector2& TriangleFrame::getScale() const {
	return scale;
}

const float TriangleFrame::getRotation() const {
	return rotation;
}

const Color& TriangleFrame::getTint() const {
	return tint;
}

const float TriangleFrame::getAlpha() const {
	return tint.w;
}

const int TriangleFrame::getWidth() const {
	return 0;
}

const int TriangleFrame::getHeight() const {
	return 0;
}

const float TriangleFrame::getLayerDepth() const {
	return layerDepth;
}

void TriangleFrame::moveBy(const Vector2& moveVector) {

	point1 += moveVector;
	point2 += moveVector;
	point3 += moveVector;
}

void TriangleFrame::setPosition(const Vector2& newPosition) {

	Vector2 diff = newPosition - point1;
	moveBy(diff);

}

void TriangleFrame::setOrigin(const Vector2& orgn) {
	origin = orgn;
}

void TriangleFrame::setScale(const Vector2& scl) {

	Vector2 origpos = point1;

	setPosition(Vector2::Zero);
	point2 *= scl;
	point3 *= scl;

	Vector2 diff = point1 - point2;
	float length = sqrt(diff.x * diff.x + diff.y * diff.y);
	lineRECT1.right = length + lengthBuffer;
	lineRECT1.bottom *= scl.x;

	diff = point2 - point3;
	length = sqrt(diff.x * diff.x + diff.y * diff.y);
	lineRECT2.right = length + lengthBuffer;
	lineRECT2.bottom *= scl.x;

	diff = point3 - point1;
	length = sqrt(diff.x * diff.x + diff.y * diff.y);
	lineRECT3.left = 0;
	lineRECT3.top = 0;
	lineRECT3.right = length + lengthBuffer;
	lineRECT3.bottom *= scl.x;

	setPosition(origpos);
}

void TriangleFrame::setRotation(const float rot) {
	rotation = rot;
	angle1to2 += rot;
	angle2to3 += rot;
	angle3to1 += rot;
}

void TriangleFrame::setTint(const XMFLOAT4 color) {
	tint = color;
}

void TriangleFrame::setAlpha(const float alpha) {
	tint.w = alpha;
}

void TriangleFrame::setLayerDepth(const float depth) {
	layerDepth = depth;
}
/** ***** END TRIANGLEFRAME ***** **/

