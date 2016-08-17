#pragma once

#include "IElement2D.h"

/* A unique graphical asset that multiple sprites may use. */
class GraphicsAsset {
public:
	GraphicsAsset();
	~GraphicsAsset();

	bool load(ID3D11Device* device, const wchar_t* file);

	const int getWidth() const;
	const int getHeight() const;

	ComPtr<ID3D11ShaderResourceView> getTexture();
	ComPtr<ID3D11Resource> getResource();

protected:
	ComPtr<ID3D11ShaderResourceView> texture;
	ComPtr<ID3D11Resource> resource;

	UINT width;
	UINT height;


};