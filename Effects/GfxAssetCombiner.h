#pragma once
#include "../GUIFactory.h"

class GfxAssetCombiner : public Texturizable {
public:
	GfxAssetCombiner(GUIFactory* factory);
	virtual ~GfxAssetCombiner();

	//unique_ptr<GraphicsAsset> combine(GraphicsAsset* asset1, GraphicsAsset* asset2);
	unique_ptr<GraphicsAsset> combine(Sprite* sprite1, Sprite* sprite2);


	/* Not used */
	virtual unique_ptr<GraphicsAsset> texturize() override;
	virtual void textureDraw(SpriteBatch* batch, ComPtr<ID3D11Device> device = NULL) override;

	virtual void setPosition(const Vector2 & position) override;
	virtual const Vector2& getPosition() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;
private:
	GUIFactory* guiFactory;

	Sprite* sprite1;
	Sprite* sprite2;
	Vector2 origin;
	int width, height;
};
