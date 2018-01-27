#include "GfxAssetCombiner.h"
#include "CommonStates.h"


GfxAssetCombiner::GfxAssetCombiner(GUIFactory* factory) {
	guiFactory = factory;
}

GfxAssetCombiner::~GfxAssetCombiner() {
}

unique_ptr<GraphicsAsset> GfxAssetCombiner::combine(Sprite* sprt1, Sprite* sprt2) {

	origin = Vector2::Zero;
	sprite1 = sprt1;
	sprite2 = sprt2;


	width = sprite1->getWidth();
	height = sprite1->getHeight();

	return guiFactory->createTextureFromTexturizable(this, false);
}

/* Probably not needed. */
unique_ptr<GraphicsAsset> GfxAssetCombiner::texturize() {
	return NULL;
}

void GfxAssetCombiner::textureDraw(SpriteBatch* batch, ComPtr<ID3D11Device> device) {
	batch->Begin(SpriteSortMode_Deferred, CommonStates(device.Get()).NonPremultiplied());
	sprite1->draw(batch);
	sprite2->draw(batch);
	batch->End();
}

void GfxAssetCombiner::setPosition(const Vector2& position) {
	origin = position;
}

const Vector2& GfxAssetCombiner::getPosition() const {
	return origin;
}

const int GfxAssetCombiner::getWidth() const {
	return width;
}

const int GfxAssetCombiner::getHeight() const {
	return height;
}
