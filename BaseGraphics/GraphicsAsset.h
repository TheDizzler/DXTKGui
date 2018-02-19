#pragma once

#include <pugixml.hpp>
#include <map>

#include "IElement2D.h"


struct Frame {
	/** If no origin is set in xml Vector2::Zero is assumed. */
	Frame(RECT srcRect, Vector2 orgn, float timeForFrame)
		: sourceRect(srcRect), origin(orgn), frameTime(timeForFrame) {
	}
	/* Rectangle which contains sprite in spritesheet. */
	RECT sourceRect;
	Vector2 origin;
	float frameTime;
};

struct Animation {

	Animation(ComPtr<ID3D11ShaderResourceView> tex, vector<unique_ptr<Frame>> frames,
		string aniName)
		: texture(tex), animationFrames(move(frames)), animationName(aniName) {
	}
	virtual ~Animation();

	vector<unique_ptr<Frame>> animationFrames;
	ComPtr<ID3D11ShaderResourceView> texture;
	string animationName;
};


/* A unique graphical asset that multiple sprites may use. */
class GraphicsAsset {
public:

	virtual ~GraphicsAsset();

	/** origin = Vector2(-1000, -1000) will auto-center origin. */
	bool load(ComPtr<ID3D11Device> device, const pugi::char_t* assetName,
		const wchar_t* texturefileName, const Vector2& origin = Vector2(-1000, -1000),
		bool showMessageBox = true);
	/** origin = Vector2(-1000, -1000) will auto-center origin. */
	void loadAsPartOfSheet(ComPtr<ID3D11ShaderResourceView> spriteSheetTexture,
		const pugi::char_t* assetName, const Vector2& locationInSheet, const Vector2& size,
		const Vector2& origin = Vector2(-1000, -1000));

	void getTextureDimensions(ID3D11Resource* res, UINT* width, UINT* height);

	const int getWidth() const;
	const int getHeight() const;
	const Vector2& getOrigin() const;
	const Vector2& getPosition() const;

	const RECT& getSourceRect() const;

	ComPtr<ID3D11ShaderResourceView> getTexture();
	ComPtr<ID3D11Resource> getResource();

	/* For Reloading textures on device change. */
	string assetName;
protected:
	ComPtr<ID3D11ShaderResourceView> texture;
	ComPtr<ID3D11Resource> resource;

	UINT width;
	UINT height;
	/* Optional. Default == Vector2(width / 2, height / 2). */
	Vector2 origin;
	/* Position in spritesheet (if applicable) */
	Vector2 position = Vector2::Zero;

	RECT sourceRect;


};


class AssetSet {
public:
	AssetSet(const pugi::char_t* setName);
	virtual ~AssetSet();

	void addAsset(string assetName, unique_ptr<GraphicsAsset> asset);
	void addAsset(string assetName, unique_ptr<Animation> asset);
	GraphicsAsset* const getAsset(const pugi::char_t* assetName);
	Animation* getAnimation(const pugi::char_t* animationName);

	const pugi::char_t* setName;

private:

	map<string, unique_ptr<GraphicsAsset> > assetMap;
	map<string, unique_ptr<Animation>> animationMap;
};