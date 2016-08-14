#pragma once

#include "Button.h"


class ImageButton : public Button {
public:
	ImageButton();
	~ImageButton();

	bool load(ID3D11Device* device,
		const wchar_t* upButtonFile, const wchar_t* downButtonFile);


	virtual void draw(SpriteBatch * batch) override;

};
