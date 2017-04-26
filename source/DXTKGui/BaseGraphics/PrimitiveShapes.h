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


class TexturePanel;
class GUIFactory;
/** A primitive, non-filled rectangle. */
class RectangleFrame : public IElement2D, public Texturizable {
public:
	/* GUIFactory is optional. */
	RectangleFrame(GraphicsAsset* pixelAsset, _In_opt_ GUIFactory* guifactory);
	~RectangleFrame();

	void setDimensions(const Vector2& position, const Vector2& size,
		int frameThickness = 2);
	void setSize(const Vector2& size);
	void refreshDimensions();

	virtual GraphicsAsset* texturize() override;
	virtual void textureDraw(SpriteBatch* batch) override;

	virtual void setPosition(const Vector2& newPosition) override;
	virtual void moveBy(const Vector2& moveVector) override;

	virtual void setTint(const XMFLOAT4 color) override;
	virtual void setTint(const Color& color) override;
	virtual void setTint(const XMVECTORF32 color) override;

	void draw(SpriteBatch* batch);

	virtual const Vector2& getPosition() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;
	virtual const float getLayerDepth() const override;
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
	/* bool frontToBack has no effect in RectangleFrame. */
	virtual void setLayerDepth(const float depth, bool frontToBack = true) override;

	bool contains(const Vector2& point);
private:
	ComPtr<ID3D11ShaderResourceView> pixel;

	bool useTexture = true;
	unique_ptr<TexturePanel> texturePanel;

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
	float layerDepth = .9f;
	unique_ptr<HitArea> hitArea;

	GUIFactory* guiFactory;

};


/** Still just a proto-type. Not recommended for real life.
	Lines don't cross so it looks a little funny...*/
class TriangleFrame : public IElement2D {
public:
	TriangleFrame(GraphicsAsset* pixelAsset);
	~TriangleFrame();

	void setDimensions(const Vector2& point1, const Vector2& point2, const Vector2& point3,
		USHORT thickness);

	virtual const Vector2& getPosition() const override;
	virtual const Vector2& getOrigin() const override;
	virtual const Vector2& getScale() const override;
	virtual const float getRotation() const override;
	virtual const Color& getTint() const override;
	virtual const float getAlpha() const override;
	/* Returns 0.... */
	virtual const int getWidth() const override;
	/* Returns 0.... */
	virtual const int getHeight() const override;
	virtual const float getLayerDepth() const override;

	virtual void moveBy(const Vector2& moveVector) override;
	/* Returns position of point1. */
	virtual void setPosition(const Vector2& position) override;
	virtual void setOrigin(const Vector2& origin) override;
	virtual void setScale(const Vector2& scale) override;
	virtual void setRotation(const float rotation) override;
	virtual void setTint(const XMFLOAT4 color) override;
	virtual void setTint(const Color& color) override;
	virtual void setTint(const XMVECTORF32 color) override;
	virtual void setAlpha(const float alpha) override;
	/* bool frontToBack has no effect in TriangleFrame. */
	virtual void setLayerDepth(const float depth, bool frontToBack = true) override;

	virtual void draw(SpriteBatch * batch) override;

private:
	ComPtr<ID3D11ShaderResourceView> pixel;

	Vector2 origin;
	Vector2 originLine1, originLine2, originLine3;
	Vector2 point1, point2, point3;
	RECT lineRECT1, lineRECT2, lineRECT3;
	float angle1to2, angle2to3, angle3to1;

	float rotation = 0.0f;
	int lengthBuffer;
	float layerDepth = .9f;
	Vector2 scale = Vector2(1, 1);
	Color tint;
};


/** A primitive line. */
class Line {
public:
	Line(GraphicsAsset* pixelAsset);
	Line(GraphicsAsset* pixelAsset,
		const Vector2& pos, const Vector2& size);
	~Line();

	const float getRotation() const;

	void setDimensions(const Vector2& position, const Vector2& size);

	void setRotation(const float rotation);
	void setTint(const Color& color);
	void setTint(const XMVECTORF32 color);
	/* bool frontToBack has no effect in Line. */
	virtual void setLayerDepth(const float depth, bool frontToBack = true);

	void draw(SpriteBatch* batch);

private:
	ComPtr<ID3D11ShaderResourceView> pixel;

	RECT lineRect;
	Vector2 position;
	Vector2 scale = Vector2(1, 1);
	float rotation = 0.0f;
	float layerDepth = .9f;
	Color tint;
};