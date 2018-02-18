#pragma once

#include "Dialog.h"

/* This Dialog box requires an AssetSet with nine assets:
	"Top Left Corner",  "Top Center", "Top Right Corner",
	"Center Left", "Middle", "Center Right"
	"Bottom Left Corner", "Bottom Center", "Bottom Right Corner"
	NOTE: Snaps the size to the dimensions of the assets that make it.
	Converts the final image into a texture and uses that for display. */
class DynamicDialog : public Dialog {
public:
	/** Movable and centerText not yet implemented). */
	DynamicDialog(GUIFactory* factory,
		MouseController* mouseController);
	virtual ~DynamicDialog();

	void initialize(AssetSet* assetSet, const pugi::char_t* font = "Default Font");

	virtual void forceRefresh() override;
	virtual void reloadGraphicsAsset() override;

	virtual bool update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;

	virtual unique_ptr<GraphicsAsset> texturize() override;
	virtual void textureDraw(SpriteBatch* batch, ComPtr<ID3D11Device> device = NULL) override;

	virtual void setText(wstring text) override;
	virtual void setDimensions(const Vector2& position, const Vector2& size);

	virtual void setPosition(const Vector2& newPosition) override;
	virtual void setLayerDepth(const float depth, bool frontToBack = true) override;


protected:

	AssetSet* assetSet;
	
	GraphicsAsset* topLeftCorner;
	GraphicsAsset* topCenter;
	GraphicsAsset* topRightCorner;

	GraphicsAsset* centerLeft;
	GraphicsAsset* middle;
	GraphicsAsset* centerRight;

	GraphicsAsset* bottomLeftCorner;
	GraphicsAsset* bottomCenter;
	GraphicsAsset* bottomRightCorner;



};