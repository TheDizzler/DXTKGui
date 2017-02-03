#pragma once

#include "IElement2D.h"


struct Frame {
	Frame(RECT srcRect) : sourceRect(srcRect) {
	}
	/* Rectangle which contains sprite in spritesheet. */
	RECT sourceRect;
};

struct Animation {

	Animation(ComPtr<ID3D11ShaderResourceView> tex, vector<shared_ptr<Frame>> frames, float frameTime)
		: texture(tex), animationFrames(frames), timePerFrame(frameTime) {
	}
	~Animation() {
		animationFrames.clear();
	}

	vector<shared_ptr<Frame>> animationFrames;
	ComPtr<ID3D11ShaderResourceView> texture;
	float timePerFrame;

};


/* A unique graphical asset that multiple sprites may use. */
class GraphicsAsset {
public:


	GraphicsAsset();
	~GraphicsAsset();

	bool load(ComPtr<ID3D11Device> device, const wchar_t* file, const Vector2& origin = Vector2(-1000, -1000));
	void loadAsPartOfSheet(ComPtr<ID3D11ShaderResourceView> spriteSheetTexture,
		const Vector2& locationInSheet, const Vector2& size, const Vector2& origin = Vector2(-1000, -1000));

	void getTextureDimensions(ID3D11Resource* res, UINT* width, UINT* height);

	const int getWidth() const;
	const int getHeight() const;
	const Vector2& getOrigin() const;
	const Vector2& getPosition() const;

	ComPtr<ID3D11ShaderResourceView> getTexture();
	ComPtr<ID3D11Resource> getResource();


protected:
	ComPtr<ID3D11ShaderResourceView> texture;
	ComPtr<ID3D11Resource> resource;

	UINT width;
	UINT height;
	/* Optional. Currently only used for MouseController. Default == Vector2(width / 2, height / 2). */
	Vector2 origin;
	/* Position in spritesheet (if applicable) */
	Vector2 position = Vector2::Zero;

};