#include "../pch.h"
#pragma once

#include "GUIControl.h"
#include "ScrollBar.h"


class TexturePanel : public GUIControl {
public:

	TexturePanel(GraphicsAsset* pixelAsset, ScrollBar* scrllbr);
	~TexturePanel();


	void setScrollBar(ScrollBarDesc& scrollBarDesc);
	void alwaysShowScrollBar(bool alwaysShow);

	void setTexture(unique_ptr<GraphicsAsset> gfxAsset);
	

	virtual void update(double deltaTime, MouseController* mouse) override;
	virtual void draw(SpriteBatch* batch) override;

	void setDimensions(const Vector2& position, const Vector2& size);
	void setTexturePosition(const Vector2& position);

	virtual void setFont(const pugi::char_t* font = "Default Font") override;
	virtual void setText(wstring text) override;
	virtual const Vector2& XM_CALLCONV measureString() const override;

	virtual void moveBy(const Vector2& moveVector) override;
	virtual const Vector2& getPosition() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;
	const Vector2& getScrollBarSize() const;

	virtual bool clicked() override;
	virtual bool pressed() override;
	virtual bool hovering() override;

	int scrollSpeed = 10;
private:

	unique_ptr<GraphicsAsset> gfxAsset;
	ComPtr<ID3D11ShaderResourceView> texture;

	//RECT panelRect;
	/* Portion of panel that is visible. */
	RECT viewRect;

	unique_ptr<ScrollBar> verticalScrollBar;
	bool alwaysDisplayScrollBar = false;
};