#pragma once

#include "ScrollBar.h"


class TexturePanel : public GUIControl {
public:

	TexturePanel(GUIFactory* factory, MouseController* mouseController,
		ScrollBar* scrllbr);
	virtual ~TexturePanel();

	

	void setScrollBar(ScrollBarDesc& scrollBarDesc);
	void alwaysShowScrollBar(bool alwaysShow);

	void setTexture(unique_ptr<GraphicsAsset> gfxAsset);

	virtual void reloadGraphicsAsset() override;

	virtual bool update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;

	void setDimensions(const Vector2& position, const Vector2& size);
	/* Use for repositioning the texture within the viewable area of the panel
		(ex: centering). Not recommended for textures bigger than the viewing
		area. */
	void setTexturePosition(const Vector2& position);

	virtual void setLayerDepth(const float depth, bool frontToBack = true) override;
	virtual void setScale(const Vector2& newScale) override;
	/* Font is not set here. The object that creates the texture sets the font. */
	virtual void setFont(const pugi::char_t* font = "Default Font") override;
	/* Text is not set here. The object that creates the texture sets the text. */
	virtual void setText(wstring text) override;
	/* Nothing to measure here. This is a texture.*/
	virtual const Vector2 XM_CALLCONV measureString() const override;

	virtual void moveBy(const Vector2& moveVector) override;
	virtual const Vector2& getPosition() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;
	const Vector2 getScrollBarSize() const;

	bool scrollBarVisible() const;

	virtual bool clicked() override;
	virtual bool pressed() override;
	virtual bool hovering() override;

	/** Not used in TexturePanel. */
	virtual void onClick() override {
	};
	/** Not used in TexturePanel. */
	virtual void onPress() override {
	};
	/** Not used in TexturePanel. */
	virtual void onHover() override {
	};
	/** Not used in TexturePanel. */
	virtual void resetState() override {};

	int scrollSpeed = 10;
private:

	unique_ptr<GraphicsAsset> gfxAsset;
	ComPtr<ID3D11ShaderResourceView> texture;

	/* Portion of panel that is visible. */
	RECT viewRect;

	unique_ptr<ScrollBar> verticalScrollBar = NULL;
	bool alwaysDisplayScrollBar = false;
	bool showScrollBar = false;
	bool neverShowScrollBar = false;
};