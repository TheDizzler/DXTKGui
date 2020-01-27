#include "DynamicDialog.h"
#include "../GUIFactory.h"
#include <sstream>
#include <comdef.h>

DynamicDialog::DynamicDialog(GUIFactory* factory,
	MouseController* mouseController) : Dialog(factory, mouseController) {

	texturePanel.reset(guiFactory->createPanel());
}

DynamicDialog::~DynamicDialog() {
}


void DynamicDialog::initialize(AssetSet* set, const pugi::char_t* font) {

	assetSet = set;
	topLeftCorner = assetSet->getAsset("Top Left Corner");
	topCenter = assetSet->getAsset("Top Center");
	topRightCorner = assetSet->getAsset("Top Right Corner");

	centerLeft = assetSet->getAsset("Left Center");
	middle = assetSet->getAsset("Center");
	centerRight = assetSet->getAsset("Right Center");

	bottomLeftCorner = assetSet->getAsset("Bottom Left Corner");
	bottomCenter = assetSet->getAsset("Bottom Center");
	bottomRightCorner = assetSet->getAsset("Bottom Right Corner");

	dialogText.reset(guiFactory->createTextLabel(Vector2::Zero, L"", font));

	setLayerDepth(.95f);

}

void DynamicDialog::forceRefresh() {
	refreshTexture = true;
}

void DynamicDialog::reloadGraphicsAsset() {
	const pugi::char_t* assetSetName = assetSet->setName;
	
	assetSet = guiFactory->getAssetSet(assetSetName);

	topLeftCorner = assetSet->getAsset("Top Left Corner");
	topCenter = assetSet->getAsset("Top Center");
	topRightCorner = assetSet->getAsset("Top Right Corner");

	centerLeft = assetSet->getAsset("Left Center");
	middle = assetSet->getAsset("Center");
	centerRight = assetSet->getAsset("Right Center");

	bottomLeftCorner = assetSet->getAsset("Bottom Left Corner");
	bottomCenter = assetSet->getAsset("Bottom Center");
	bottomRightCorner = assetSet->getAsset("Bottom Right Corner");

	dialogText->reloadGraphicsAsset();

	refreshTexture = false;
}


void DynamicDialog::setText(wstring text) {
	dialogText->setText(text);
}

void DynamicDialog::setDimensions(const Vector2& posit, const Vector2& sz) {

	size = sz;
	position = posit;

	if (assetSet == NULL) {
		OutputDebugString(L"\n\t!!Critical Failure in Dynamic Dialog: asset set is NULL!!\n\n");
		return;
	}


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

	refreshTexture = true;
}


bool DynamicDialog::update(double deltaTime) {

	bool refreshed = false;

	if (dialogText->update(deltaTime)) {
		refreshTexture = true;
		refreshed = true;
	}

	if (refreshTexture) {
		texturePanel->setTexture(texturize());
		refreshTexture = false;
		refreshed = true;
	}

	return refreshed;
}


void DynamicDialog::draw(SpriteBatch* batch) {

	if (!isShowing)
		return;

	texturePanel->draw(batch);
	dialogText->draw(batch);
}


unique_ptr<GraphicsAsset> DynamicDialog::texturize() {
	return move(guiFactory->createTextureFromTexturizable(this));
}


void DynamicDialog::textureDraw(SpriteBatch* batch, ComPtr<ID3D11Device> device) {

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
	Vector2 cornerSize((float) widthAdd, (float) topLeftCorner->getHeight());
	topPos = position + cornerSize;
	int maxLength = INT(topPos.x + size.x - cornerSize.x * 2);
	int maxHeight = INT(topPos.y + size.y - cornerSize.y * 2);
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
	maxLength = INT(position.x + size.x - widthAdd);
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
	maxHeight = INT(position.y + size.y - heightAdd);
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


void DynamicDialog::setPosition(const Vector2& newPosition) {

	Vector2 moveBy = newPosition - position;
	dialogText->moveBy(moveBy);
	GUIControl::setPosition(newPosition);

	texturePanel->setTexturePosition(position);
}

void DynamicDialog::setLayerDepth(const float depth, bool frontToBack) {

	layerDepth = depth - .00001f;
	if (layerDepth < 0) {
		if (!frontToBack)
			layerDepth = .00001f;
		else
			layerDepth = 0.0f;
	}
	float nudge = .00000001f;
	if (!frontToBack)
		nudge *= -1;
	dialogText->setLayerDepth(layerDepth + nudge, frontToBack);
}

