#include "GraphicsAsset.h"
#include "DDSTextureLoader.h"


GraphicsAsset::GraphicsAsset() {
}
GraphicsAsset::~GraphicsAsset() {
}

#include "../globals.h"
bool GraphicsAsset::load(ID3D11Device* device, const wchar_t* textureFile) {
	
	if (Globals::reportError(CreateDDSTextureFromFile(device, textureFile,
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


const int GraphicsAsset::getWidth() const {
	return width;
}

const int GraphicsAsset::getHeight() const {
	return height;
}

ComPtr<ID3D11ShaderResourceView> GraphicsAsset::getTexture() {
	return texture;
}

ComPtr<ID3D11Resource> GraphicsAsset::getResource() {
	return resource;
}