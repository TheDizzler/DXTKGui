#include "GraphicsAsset.h"
#include <DDSTextureLoader.h>
#include "../StringHelper.h"


GraphicsAsset::~GraphicsAsset() {

	//int numRefRemaining = texture.Reset();
	//if (assetName.find("Texturized") == string::npos) {
	//	stringstream ss;
	//	ss << "\n\n*** GraphicsAsset: " << assetName << endl;
	//	//ss << "\t\tID3D11Resource release #: " << resource.Reset() << endl;
	//	ss << "\t\tResource release #: " << numRefRemaining;
	//	if (numRefRemaining == 0)
	//		ss << "!!!!!!!!";
	//	ss << endl;
	//	OutputDebugStringA(ss.str().c_str());
	//}
}


bool GraphicsAsset::load(ComPtr<ID3D11Device> device, const pugi::char_t* asset,
	const wchar_t* texturefileName, const Vector2& org, bool showMessageBox) {

	assetName = string(asset);

	wostringstream wss;
	wss << L"Unable to load texture file: " << texturefileName << " in GraphicsAsset::load().";

	if (StringHelper::reportError(
		CreateDDSTextureFromFile(device.Get(), texturefileName,
			resource.GetAddressOf(), texture.GetAddressOf()),
		wss.str(), L"ERROR", showMessageBox))
		return false;


	getTextureDimensions(resource.Get(), &width, &height);

	if (org == Vector2(-1000, -1000))
		origin = Vector2(FLOAT(width) / 2, FLOAT(height) / 2);
	else
		origin = org;

	sourceRect.left = 0;
	sourceRect.top = 0;
	sourceRect.bottom = height;
	sourceRect.right = width;

	return true;
}


void GraphicsAsset::loadAsPartOfSheet(
	ComPtr<ID3D11ShaderResourceView> spriteSheetTexture,
	const pugi::char_t* asset, const Vector2& locationInSheet, const Vector2& size,
	const Vector2& org) {

	assetName = string(asset);

	texture = spriteSheetTexture;
	position = locationInSheet;
	width = UINT(size.x);
	height = UINT(size.y);

	if (org == Vector2(-1000, -1000))
		origin = Vector2(FLOAT(width) / 2, FLOAT(height) / 2);
	else
		origin = org;

	sourceRect.left = LONG(position.x);
	sourceRect.top = LONG(position.y);
	sourceRect.right = LONG(position.x) + width;
	sourceRect.bottom = LONG(position.y) + height;
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

const Vector2& GraphicsAsset::getOrigin() const {
	return origin;
}

const Vector2& GraphicsAsset::getPosition() const {
	return position;
}

const RECT& GraphicsAsset::getSourceRect() const {
	return sourceRect;
}


ComPtr<ID3D11ShaderResourceView> GraphicsAsset::getTexture() {
	return texture;
}

ComPtr<ID3D11Resource> GraphicsAsset::getResource() {
	return resource;
}



/**** ***** AssetSet START***** ****/
AssetSet::AssetSet(const pugi::char_t* name) {
	setName = name;
}

AssetSet::~AssetSet() {
}

void AssetSet::addAsset(string assetName, unique_ptr<GraphicsAsset> asset) {
	assetMap[assetName] = move(asset);
}

void AssetSet::addAsset(string assetName, unique_ptr<Animation> asset) {
	animationMap[assetName] = move(asset);
}

GraphicsAsset* const AssetSet::getAsset(const pugi::char_t* assetName) {

	if (assetMap.find(assetName) == assetMap.end()) {
		wostringstream ws;
		ws << "Cannot find asset file: " << assetName << " in " << setName << "\n";
		OutputDebugString(ws.str().c_str());
		return NULL;
	}

	return assetMap[assetName].get();
}

Animation* AssetSet::getAnimation(const pugi::char_t* animationName) {

	if (animationMap.find(animationName) == animationMap.end()) {
		wostringstream ws;
		ws << "Cannot find asset file: " << animationName << " in " << setName << "\n";
		OutputDebugString(ws.str().c_str());
		return NULL;
	}
	return animationMap[animationName].get();
}

Animation::~Animation() {

	wostringstream woo;
	woo << L"\n\n*** Animation Release ***\n\t" << endl;
	woo << "\t\tTexture release #: " << texture.Reset() << "\n\t ->";
	//OutputDebugString(woo.str().c_str());

	for (auto& frame : animationFrames)
		frame.reset();
	animationFrames.clear();

	//OutputDebugString(L"*** Animation Done ***\n");
}