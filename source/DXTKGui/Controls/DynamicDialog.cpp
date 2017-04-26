#include "DynamicDialog.h"

DynamicDialog::DynamicDialog() {

}

DynamicDialog::~DynamicDialog() {
}

#include "GUIFactory.h"
void DynamicDialog::initializeControl(GUIFactory* factory,
	shared_ptr<MouseController> mouseController) {
	GUIControl::initializeControl(factory, mouseController);
}


void DynamicDialog::initialize(shared_ptr<AssetSet> set, const pugi::char_t* font) {

	assetSet = set;

	hitArea = make_unique<HitArea>(Vector2::Zero, Vector2::Zero);
	dialogText = make_unique<TextLabel>(guiFactory->getFont(font));

	setLayerDepth(.95);


}


void DynamicDialog::setText(wstring text) {
	dialogText->setText(text);
}

#include <sstream>
#include <comdef.h>



void DynamicDialog::setDimensions(const Vector2& posit, const Vector2& sz) {

	size = sz;
	position = posit;
	
	topLeftCorner = assetSet->getAsset("Top Left Corner");
	topCenter = assetSet->getAsset("Top Center");
	topRightCorner = assetSet->getAsset("Top Right Corner");

	centerLeft = assetSet->getAsset("Center Left");
	middle = assetSet->getAsset("Middle");
	centerRight = assetSet->getAsset("Center Right");

	bottomLeftCorner = assetSet->getAsset("Bottom Left Corner");
	bottomCenter = assetSet->getAsset("Bottom Center");
	bottomRightCorner = assetSet->getAsset("Bottom Right Corner");

	if (size.x > 0 && size.y > 0) {
		// adjust to size of images
		int leftOver = int(size.x - topLeftCorner->getWidth() * 2) % middle->getWidth();
		if (leftOver != 0) {

			if (leftOver < middle->getWidth() / 2) {
				size.x -= leftOver;
			} else {
				size.x += middle->getWidth() - leftOver;
			}
		}

		leftOver = int(size.y - topLeftCorner->getHeight() * 2) % middle->getHeight();
		if (leftOver != 0) {

			if (leftOver < middle->getHeight() / 2) {
				size.y -= leftOver;
			} else {
				size.y += middle->getHeight() - leftOver;
			}
		}

	}


	Vector2 textPos = position + dialogTextMargin;
	dialogText->setPosition(textPos);
}


GraphicsAsset* DynamicDialog::texturize() {

	texturePanel.reset(guiFactory->createPanel());
	//texturePanel->setTint(Color(0, 1, 1, 1));
	texturePanel->setDimensions(position, size);
	bool originalStatus = isShowing;
	isShowing = true;
	bool original = useTexture;
	useTexture = false;

	GraphicsAsset* gfxAss = guiFactory->createTextureFromIElement2D(this);
	texturePanel->setTexture(gfxAss);
	useTexture = original;
	isShowing = originalStatus;

	texturePanel->setTexturePosition(position);

	return gfxAss;
}


void DynamicDialog::textureDraw(SpriteBatch* batch) {

	Vector2 topPos = Vector2::Zero;
	Vector2 bottomPos = Vector2(0, size.y - topLeftCorner->getHeight());

	// draw left corners
	batch->Draw(topLeftCorner->getTexture().Get(), topPos,
		&topLeftCorner->getSourceRect(), tint, rotation, origin,
		scale, SpriteEffects_None, layerDepth);

	batch->Draw(bottomLeftCorner->getTexture().Get(), bottomPos,
		&bottomLeftCorner->getSourceRect(), tint, rotation, origin,
		scale, SpriteEffects_None, layerDepth);

	// draw middle
	int widthAdd = topLeftCorner->getWidth();
	Vector2 cornerSize(widthAdd, topLeftCorner->getHeight());
	topPos = position + cornerSize;
	int maxLength = topPos.x + size.x - cornerSize.x * 2;
	int maxHeight = topPos.y + size.y - cornerSize.y * 2;
	while (topPos.y <= maxHeight) {
		topPos.x = position.x + cornerSize.x;
		while (topPos.x <= maxLength) {
			batch->Draw(middle->getTexture().Get(), topPos,
				&middle->getSourceRect(), tint, rotation, origin,
				scale, SpriteEffects_None, layerDepth);

			topPos.x += middle->getWidth();
		}
		topPos.y += middle->getHeight();
	}

	topPos = position;
	maxLength = position.x + size.x - widthAdd;
	topPos.x += widthAdd;
	bottomPos.x += widthAdd;
	widthAdd = topCenter->getWidth();
	// draw horz sides
	while (topPos.x < maxLength) {

		batch->Draw(topCenter->getTexture().Get(), topPos,
			&topCenter->getSourceRect(), tint, rotation, origin,
			scale, SpriteEffects_None, layerDepth);

		batch->Draw(bottomCenter->getTexture().Get(), bottomPos,
			&bottomCenter->getSourceRect(), tint, rotation, origin,
			scale, SpriteEffects_None, layerDepth);


		topPos.x += widthAdd;
		bottomPos.x += widthAdd;
	}

	// draw right corners
	batch->Draw(topRightCorner->getTexture().Get(), topPos,
		&topRightCorner->getSourceRect(), tint, rotation, origin,
		scale, SpriteEffects_None, layerDepth);

	batch->Draw(bottomRightCorner->getTexture().Get(), bottomPos,
		&bottomRightCorner->getSourceRect(), tint, rotation, origin,
		scale, SpriteEffects_None, layerDepth);


	// draw verticals
	Vector2 leftPos = position;
	Vector2 rightPos = position;
	//rightPos.x += size.x - topLeftCorner->getWidth();
	rightPos.x = bottomPos.x;

	int heightAdd = topLeftCorner->getHeight();
	maxHeight = position.y + size.y - heightAdd;
	leftPos.y += heightAdd;
	rightPos.y += heightAdd;
	heightAdd = centerLeft->getHeight();
	while (leftPos.y < maxHeight) {

		batch->Draw(centerLeft->getTexture().Get(), leftPos,
			&centerLeft->getSourceRect(), tint, rotation, origin,
			scale, SpriteEffects_None, layerDepth);

		batch->Draw(centerRight->getTexture().Get(), rightPos,
			&centerRight->getSourceRect(), tint, rotation, origin,
			scale, SpriteEffects_None, layerDepth);


		leftPos.y += heightAdd;
		rightPos.y += heightAdd;
	}
}


void DynamicDialog::update(double deltaTime) {
}


void DynamicDialog::draw(SpriteBatch* batch) {

	if (!isShowing)
		return;

	//if (useTexture) {
		texturePanel->draw(batch);
	//} else {
	//	
	//}
	dialogText->draw(batch);
}

void DynamicDialog::setPosition(const Vector2& newPosition) {


	Vector2 moveBy = newPosition - position;
	dialogText->moveBy(moveBy);
	GUIControl::setPosition(newPosition);
	//panel->setPosition(newPosition);
	texturePanel->setTexturePosition(position);
}

void DynamicDialog::setLayerDepth(const float depth, bool frontToBack) {

	layerDepth = depth - .00001;
	if (layerDepth < 0) {
		if (!frontToBack)
			layerDepth = .00001;
		else
			layerDepth = 0;
	}
	float nudge = .00000001;
	if (!frontToBack)
		nudge *= -1;
	dialogText->setLayerDepth(layerDepth + nudge, frontToBack);
}

