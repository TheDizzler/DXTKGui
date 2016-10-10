#include "GraphicsAsset.h"



GraphicsAsset::GraphicsAsset() {
}
GraphicsAsset::~GraphicsAsset() {

}

#include "../StringHelper.h"
#include <DDSTextureLoader.h>
bool GraphicsAsset::load(ComPtr<ID3D11Device> device, const wchar_t* textureFile) {

	if (StringHelper::reportError(
		CreateDDSTextureFromFile(device.Get(), textureFile,
			resource.GetAddressOf(), texture.GetAddressOf()),
		L"Failed to load sprite", L"ERROR"))
		return false;


	getTextureDimensions(resource.Get(), &width, &height);

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

void GraphicsAsset::getTextureDimensions(ID3D11Resource* res, UINT* width, UINT* height) {

	D3D11_RESOURCE_DIMENSION dim;
	res->GetType(&dim);

	switch (dim) {
		case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
		{

			auto texture = reinterpret_cast<ID3D11Texture2D*>(res);
			D3D11_TEXTURE2D_DESC desc;
			texture->GetDesc(&desc);
			if (width)
				*width = desc.Width; // width in pixels
			if (height)
				*height = desc.Height; // height in pixels

			break;

		}
		default:

			if (width)
				*width = 0; // width in pixels
			if (height)
				*height = 0; // height in pixels
	}
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