#include "../pch.h"
#pragma once

#include "GUIControl.h"
#include "ScrollBar.h"


class Panel : public GUIControl {
public:

	Panel(GraphicsAsset* pixelAsset, ScrollBar* scrllbr);
	~Panel();


	void setScrollBar(ScrollBarDesc& scrollBarDesc);
	void alwaysShowScrollBar(bool alwaysShow);

	void setTexture(unique_ptr<GraphicsAsset> gfxAsset);
	

	virtual void update(double deltaTime, MouseController* mouse) override;
	virtual void draw(SpriteBatch* batch) override;

	void setDimensions(const Vector2& position, const Vector2& size);

	virtual void setFont(const pugi::char_t* font = "Default Font") override;
	virtual void setText(wstring text) override;
	virtual XMVECTOR XM_CALLCONV measureString() const override;

	virtual void moveBy(const Vector2& moveVector) override;
	virtual const Vector2& getPosition() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;

	virtual bool clicked() override;
	virtual bool pressed() override;
	virtual bool hovering() override;
private:

	//unique_ptr<RectangleSprite> bgSprite;
	unique_ptr<GraphicsAsset> gfxAsset;
	//ComPtr<ID3D11RenderTargetView> renderTargetView;
	//ComPtr<ID3D11Texture2D> renderTargetTexture;

	//RECT panelRect;
	/* Portion of panel that is visible. */
	RECT viewRect;

	unique_ptr<ScrollBar> verticalScrollBar;
	bool alwaysDisplayScrollBar = false;
};