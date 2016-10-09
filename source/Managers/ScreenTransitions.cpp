#include "ScreenTransitions.h"

using namespace ScreenTransitions;


ScreenTransitionManager::ScreenTransitionManager(GUIFactory* factory) {

	guiFactory = factory;
}

ScreenTransitionManager::~ScreenTransitionManager() {
	if (transition != NULL)
		delete transition;
}

void ScreenTransitionManager::setTransition(ScreenTransition* effect) {

	if (transition != NULL)
		delete transition;
	transition = effect;
}

void ScreenTransitionManager::transitionBetween(
	Screen* oldScreen, Screen* newScreen, float transitionTime) {

	transition->setTransitionBetween(guiFactory->createTextureFromScreen(oldScreen),
		guiFactory->createTextureFromScreen(newScreen), transitionTime);
}

bool ScreenTransitionManager::runTransition(double deltaTime) {
	return transition->run(deltaTime);
}

void ScreenTransitionManager::drawTransition(SpriteBatch* batch) {
	transition->draw(batch);
}



#include "../Managers/screen.h"
void ScreenTransition::setTransitionBetween(
	GraphicsAsset* oldScreen, GraphicsAsset* newScreen, float time) {

	oldScreenAsset.reset(oldScreen);
	newScreenAsset.reset(newScreen);

	oldTexture = oldScreenAsset->getTexture();
	newTexture = newScreenAsset->getTexture();
	//screenOrigin = Vector2(oldScreenAsset->getWidth() / 2, oldScreenAsset->getHeight() / 2);
	//screenPosition = Vector2(oldScreenAsset->getWidth() / 2, oldScreenAsset->getHeight() / 2);

	screenRect.left = 0;
	screenRect.top = 0;
	screenRect.right = oldScreenAsset->getWidth();
	screenRect.bottom = oldScreenAsset->getHeight();
	transitionTime = time;

	reset();
}





FlipScreenTransition::FlipScreenTransition(bool verticalFlip) {

	startScale = Vector2(0, 0);
	if (verticalFlip) {
		startScale.x = 1;
		//currentOrientation = SpriteEffects::SpriteEffects_FlipVertically;
	} else {
		startScale.y = 1;
		//currentOrientation = SpriteEffects::SpriteEffects_FlipHorizontally;
	}

}

bool FlipScreenTransition::run(double deltaTime) {

	//if (currentOrientation != SpriteEffects::SpriteEffects_None) {
	if (texture == oldTexture) {
		scale = Vector2::Lerp(Vector2(1, 1), startScale, timer / transitionTime * 2);
		scale.Clamp(startScale, Vector2(1, 1));
		if (scale == startScale) {
			//currentOrientation = SpriteEffects::SpriteEffects_None;
			texture = newTexture;
			timer = 0;
		}
	} else {
		scale = Vector2::Lerp(startScale, Vector2(1, 1), timer / transitionTime * 2);
		scale.Clamp(startScale, Vector2(1, 1));
		if (scale == Vector2(1, 1))
			return true;
	}
	timer += deltaTime;
	return false;
}

void FlipScreenTransition::draw(SpriteBatch* batch) {

	batch->Draw(texture.Get(), position, &screenRect,
		tint, rotation, origin, scale, SpriteEffects_None);
}

void FlipScreenTransition::reset() {

	//currentOrientation = startOrientation;
	texture = oldTexture;
	origin = Vector2(oldScreenAsset->getWidth() / 2, oldScreenAsset->getHeight() / 2);
	position = Vector2(oldScreenAsset->getWidth() / 2, oldScreenAsset->getHeight() / 2);
	timer = 0;
}


void SquareFlipScreenTransition::setTransitionBetween(
	GraphicsAsset* oldScreen, GraphicsAsset* newScreen, float time) {
	ScreenTransition::setTransitionBetween(oldScreen, newScreen, time);

	int squareSize = 64;
	int row = ceil((float) oldScreenAsset->getWidth() / squareSize);
	int col = ceil((float) oldScreenAsset->getHeight() / squareSize);

	for (int j = 0; j < squares.size(); ++j) {
		for (int i = 0; i < squares[j].size(); ++i) {
			delete squares[j][i];
		}
	}
	squares.clear();
	numInRowActive.clear();
	for (int j = 0; j < col; ++j) {
		vector<Square*> rowVect;
		for (int i = 0; i < row; ++i) {
			RECT rect;
			rect.left = i*squareSize;
			rect.top = j*squareSize;
			if (i + 1 == row)
				rect.right = rect.left + oldScreenAsset->getWidth() % squareSize;
			else
				rect.right = rect.left + squareSize;
			if (j + 1 == col)
				rect.bottom = rect.top + oldScreenAsset->getHeight() % squareSize;
			else
				rect.bottom = rect.top + squareSize;

			Vector2 pos = Vector2(i * squareSize, j * squareSize);

			Square* square = new Square();
			square->rect = rect;
			square->origin = Vector2(
				(rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2);
			square->position = pos + square->origin;
			square->texture = oldTexture;
			square->scale = Vector2(1, 1);
			rowVect.push_back(square);
		}
		squares.push_back(rowVect);
		numInRowActive.push_back(1);
	}

	startScale = Vector2(0, 1);
	maxJ = 1;
	delay = transitionTime / 10;
}

SquareFlipScreenTransition::~SquareFlipScreenTransition() {
	for (int j = 0; j < squares.size(); ++j) {
		for (int i = 0; i < squares[j].size(); ++i) {
			delete squares[j][i];
		}
	}
	squares.clear();
}

bool SquareFlipScreenTransition::run(double deltaTime) {

	if (oldTexture == NULL || newTexture == NULL)
		return true;
	bool allDone = true;

	if (timer >= delay) {
		++maxJ;
		timer = 0;
	}
	if (maxJ > squares.size())
		maxJ = squares.size();

	for (int j = 0; j < maxJ; ++j) {

		for (int i = 0; i < numInRowActive[j]; ++i) {
			Square* square = squares[j][i];
			square->timer += deltaTime; // change this to unsync flips
			if (square->texture == oldTexture) {
				square->scale = Vector2::Lerp(
					Vector2(1, 1), startScale, square->timer / transitionTime * 2);
				square->scale.Clamp(startScale, Vector2(1, 1));

				if (i == numInRowActive[j] - 1 && square->timer >= delay) {
					// start next square in row
					++numInRowActive[j];
					if (numInRowActive[j] > squares[j].size())
						numInRowActive[j] = squares[j].size();
				}
				if (square->scale == startScale) {
					square->texture = newTexture;
					square->timer = 0;
				}
				allDone = false;
			} else {
				squares[j][i]->scale = Vector2::Lerp(
					startScale, Vector2(1, 1), squares[j][i]->timer / transitionTime * 2);
				squares[j][i]->scale.Clamp(startScale, Vector2(1, 1));
				if (squares[j][i]->scale != Vector2(1, 1))
					allDone = false;
			}
		}
		//--maxI;
	}
	timer += deltaTime;
	return allDone;
}

void SquareFlipScreenTransition::draw(SpriteBatch* batch) {

	for (int j = 0; j < squares.size(); ++j) {
		for (int i = 0; i < squares[j].size(); ++i) {
			batch->Draw(squares[j][i]->texture.Get(), squares[j][i]->position,
				&squares[j][i]->rect, tint, rotation, squares[j][i]->origin,
				squares[j][i]->scale, SpriteEffects_None);
		}
	}
}

void SquareFlipScreenTransition::reset() {

	timer = 0;
	//for (int j = 0; j < squares.size(); ++j) {
	//	for (int i = 0; i < squares[j].size(); ++i) {
	//	sqare
	//	}
	//	numInRowActive[j] = 1;
	//	}
}