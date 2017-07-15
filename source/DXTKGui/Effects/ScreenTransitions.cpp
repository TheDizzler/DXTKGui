#include "ScreenTransitions.h"
#include "../BaseGraphics/screen.h"
#include "../StringHelper.h"

using namespace ScreenTransitions;


ScreenTransitionManager::ScreenTransitionManager() {
}


ScreenTransitionManager::~ScreenTransitionManager() {
	if (transition != NULL)
		delete transition;
}

void ScreenTransitionManager::initialize(
	GUIFactory* factory, const char_t* bgName, bool resizeBGToFit) {


	guiFactory = factory;
	bg = move(guiFactory->getSpriteFromAsset(bgName));
	if (bg == NULL) {
		bg = guiFactory->getSpriteFromAsset("Default Transition BG");
	}
	bg->setPosition(Vector2::Zero);
	bg->setOrigin(Vector2::Zero);
	bg->setLayerDepth(0);

	RECT rect;
	GetClientRect(guiFactory->getHWND(), &rect);

	//int buffer = 5; // padding to give a bit of lee-way to prevent tearing
	int screenWidth = rect.right - rect.left;
	int screenHeight = rect.bottom - rect.top;

	if (resizeBGToFit) {
		//scale bg image to screen
		int horzDif = bg->getWidth() - screenWidth;
		int vertDif = bg->getHeight() - screenHeight;
		if (horzDif > 0 || vertDif > 0) {
			// bg image is bigger in one or more dimensions than screen
			if (horzDif > vertDif) {
				float horzRatio = float(screenWidth) / bg->getWidth();
				bg->setScale(Vector2(horzRatio, horzRatio));
			} else {
				float vertRatio = float(screenHeight) / bg->getHeight();
				bg->setScale(Vector2(vertRatio, vertRatio));
			}
		} else {
			if (horzDif < vertDif) {
				float horzRatio = float(screenWidth) / bg->getWidth();
				bg->setScale(Vector2(horzRatio, horzRatio));
			} else {
				float vertRatio = float(screenHeight) / bg->getHeight();
				bg->setScale(Vector2(vertRatio, vertRatio));
			}
		}
	}
}

void ScreenTransitionManager::reloadGraphicsAssets() {
	bg->reloadGraphicsAsset(guiFactory);

}

void ScreenTransitionManager::setTransition(ScreenTransition* effect) {

	if (transition != NULL)
		delete transition;
	transition = effect;
}


void ScreenTransitionManager::transitionBetween(
	Screen* oldScreen, Screen* newScr, float transitionTime, bool autoBatchDraw) {

	Color purple = Color(158, 0, 58);
	Color blue = Color(0, 58, 158);

	transition->setTransitionBetween(
		guiFactory->createTextureFromScreen(oldScreen, autoBatchDraw, purple),
		guiFactory->createTextureFromScreen(newScr, autoBatchDraw, blue), transitionTime);

	newScreen = newScr;


}

bool ScreenTransitionManager::runTransition(double deltaTime) {
	return transition->run(deltaTime);
}

void ScreenTransitionManager::drawTransition(SpriteBatch* batch) {
	bg->draw(batch);
	transition->draw(batch);
}


void ScreenTransition::setTransitionBetween(
	unique_ptr<GraphicsAsset> oldScreen, unique_ptr<GraphicsAsset> newScreen, float time) {

	/*wostringstream woo;
	woo << L"\n\n *** ScreenTransition Release *** " << endl;
	woo << "\t\oldTexture release #: " << oldTexture.Reset() << endl;
	woo << "\t\newTexture release #: " << newTexture.Reset() << endl;*/

	oldScreenAsset = move(oldScreen);
	newScreenAsset = move(newScreen);

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


ScreenTransitions::ScreenTransition::~ScreenTransition() {

	/*wostringstream woo;
	woo << L"\n\n *** ScreenTransition Release *** " << endl;
	woo << "\t\oldTexture release #: " << oldTexture.Reset() << endl;
	woo << "\t\newTexture release #: " << newTexture.Reset() << endl;
	oldScreenAsset.reset();
	newScreenAsset.reset();
	OutputDebugString(L"\n*** ScreenTransition Done ***");*/
}





FlipScreenTransition::FlipScreenTransition(bool verticalFlip) {

	startScale = Vector2(0, 0);
	if (verticalFlip) {
		startScale.x = 1;
	} else {
		startScale.y = 1;
	}

}

bool FlipScreenTransition::run(double deltaTime) {

	if (texture == oldTexture) {
		scale = Vector2::Lerp(Vector2(1, 1), startScale, timer / transitionTime * 2);
		scale.Clamp(startScale, Vector2(1, 1));
		if (scale == startScale) {
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

	texture = oldTexture;
	origin = Vector2(oldScreenAsset->getWidth() / 2, oldScreenAsset->getHeight() / 2);
	position = Vector2(oldScreenAsset->getWidth() / 2, oldScreenAsset->getHeight() / 2);
	timer = 0;
}



SquareFlipScreenTransition::~SquareFlipScreenTransition() {
	for (int j = 0; j < squares.size(); ++j) {
		for (int i = 0; i < squares[j].size(); ++i) {
			delete squares[j][i];
		}
	}
	squares.clear();
}

void SquareFlipScreenTransition::setTransitionBetween(
	unique_ptr<GraphicsAsset> oldScreen, unique_ptr<GraphicsAsset> newScreen, float time) {
	ScreenTransition::setTransitionBetween(move(oldScreen), move(newScreen), time);

	int squareSize = 64;
	int row = ceil((float) oldScreenAsset->getWidth() / squareSize) + 1;
	int col = ceil((float) oldScreenAsset->getHeight() / squareSize) + 1;

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
			square->timer += deltaTime;
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
}



ScreenTransitions::LineWipeScreenTransition::~LineWipeScreenTransition() {
	for (Line* line : lines)
		delete line;
	lines.clear();
}

void LineWipeScreenTransition::setTransitionBetween(
	unique_ptr<GraphicsAsset> oldScreen, unique_ptr<GraphicsAsset> newScreen, float time) {
	ScreenTransition::setTransitionBetween(move(oldScreen), move(newScreen), time);

	int rows = 7;
	int rowHeight = ceil((float) oldScreenAsset->getHeight() / rows);

	for (Line* line : lines)
		delete line;
	lines.clear();

	for (int i = 0; i < rows; ++i) {

		Line* line = new Line();
		RECT rect;
		rect.left = 0;
		rect.right = oldScreenAsset->getWidth();
		rect.top = i*rowHeight;
		rect.bottom = rect.top + rowHeight;
		line->rect = rect;

		line->position = Vector2(0, rect.top);
		line->start = Vector2(0, rect.top);
		line->end = Vector2(line->position.x - oldScreenAsset->getWidth(), line->position.y);
		lines.push_back(line);

	}

	delay = transitionTime / rows;
}


bool ScreenTransitions::LineWipeScreenTransition::run(double deltaTime) {

	bool allDone = true;
	timer += deltaTime;
	double slideTimer = timer;
	for (Line* line : lines) {
		line->position = Vector2::Lerp(
			line->start,
			line->end,
			slideTimer / transitionTime);
		slideTimer -= delay;
		if (slideTimer <= 0) {
			allDone = false;
			break;
		}
		if (line->position.x + oldScreenAsset->getWidth() > 0)
			allDone = false;
	}


	return allDone;
}

void ScreenTransitions::LineWipeScreenTransition::draw(SpriteBatch* batch) {

	batch->Draw(newTexture.Get(), position, &screenRect,
		tint, rotation, origin, scale, SpriteEffects_None);

	for (Line* line : lines)
		batch->Draw(oldTexture.Get(), line->position, &line->rect,
			tint, rotation, origin, scale, SpriteEffects_None);
}

void ScreenTransitions::LineWipeScreenTransition::reset() {

	timer = 0;
}
