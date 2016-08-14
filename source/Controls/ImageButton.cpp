#include "ImageButton.h"

ImageButton::ImageButton() {
}

ImageButton::~ImageButton() {
}

bool ImageButton::load(ID3D11Device * device, const wchar_t * upButtonFile,
	const wchar_t * downButtonFile) {


	normalSprite.reset(new Sprite());
	pressedSprite.reset(new Sprite());
	if (!normalSprite->load(device, upButtonFile) ||
		!pressedSprite->load(device, downButtonFile))
		return false;

	hitArea = normalSprite->getHitArea();

	return true;
}


void ImageButton::draw(SpriteBatch * batch) {

	Sprite* drawSprite;
	drawSprite = normalSprite.get();

	if (isSelected) {
		drawSprite = pressedSprite.get();
	} else if (isHover)
		drawSprite->setTint(hoverColor);
	else
		drawSprite->setTint(normalColor);

	drawSprite->draw(batch);
}
