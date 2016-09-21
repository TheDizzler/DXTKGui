#include "GraphicsAsset.h"
#include "DDSTextureLoader.h"


GraphicsAsset::GraphicsAsset() {
}
GraphicsAsset::~GraphicsAsset() {

}

#include "../Engine/GameEngine.h"
#include "../assets.h"
bool GraphicsAsset::load(ComPtr<ID3D11Device> device, const wchar_t* textureFile) {

	if (GameEngine::reportError(
		CreateDDSTextureFromFile(device.Get(), textureFile,
			resource.GetAddressOf(), texture.GetAddressOf()),
		L"Failed to load sprite", L"ERROR"))
		return false;


	Assets::getTextureDimensions(resource.Get(), &width, &height);

	return true;
}


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