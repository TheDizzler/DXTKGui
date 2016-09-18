#include "GraphicsAsset.h"
#include "DDSTextureLoader.h"


GraphicsAsset::GraphicsAsset() {
}
GraphicsAsset::~GraphicsAsset() {

}

#include "../assets.h"
bool GraphicsAsset::load(ComPtr<ID3D11Device> device, const wchar_t* textureFile) {

	if (Globals::reportError(CreateDDSTextureFromFile(device.Get(), textureFile,
		resource.GetAddressOf(), texture.GetAddressOf()))) {
		//MessageBox(NULL, L"Failed to load sprite", L"ERROR", MB_OK);
		return false;
	}

	Assets::getTextureDimensions(resource.Get(), &width, &height);
	/*sourceRect.left = 0;
	sourceRect.top = 0;
	sourceRect.bottom = height;
	sourceRect.right = width;

	hitArea.reset(new HitArea(
		Vector2(position.x - width / 2, position.y - height / 2),
		Vector2(width, height)));*/


	return true;
}

//void GraphicsAsset::loadAsAnimation(ComPtr<ID3D11ShaderResourceView> spriteSheetTexture,
//	const vector<Frame*> frames) {
//
//	texture = spriteSheetTexture;
//	animationFrames = frames;
//
//	position = Vector2(frames[0]->sourceRect.left, frames[0]->sourceRect.top);
//	width = frames[0]->sourceRect.right - frames[0]->sourceRect.left;
//	height = frames[0]->sourceRect.bottom - frames[0]->sourceRect.top;
//}

void GraphicsAsset::loadAsPartOfSheet(
	ComPtr<ID3D11ShaderResourceView> spriteSheetTexture,
	const Vector2& locationInSheet, const Vector2& size) {

	texture = spriteSheetTexture;
	position = locationInSheet;
	width = size.x;
	height = size.y;

}


const int GraphicsAsset::getWidth() const {
	return width;
}

const int GraphicsAsset::getHeight() const {
	return height;
}

const Vector2 GraphicsAsset::getPosition() const {
	return position;
}


ComPtr<ID3D11ShaderResourceView> GraphicsAsset::getTexture() {
	return texture;
}

ComPtr<ID3D11Resource> GraphicsAsset::getResource() {
	return resource;
}