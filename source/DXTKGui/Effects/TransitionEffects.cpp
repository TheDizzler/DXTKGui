#include "TransitionEffects.h"


TransitionEffects::GrowTransition::GrowTransition(const Vector2& strt,
	const Vector2& end, float speed) {

	startScale = strt;
	endScale = end;
	transitionSpeed = speed;
}

bool TransitionEffects::GrowTransition::run(double deltaTime, IElement2D* control) {

	Vector2 newscale = Vector2::Lerp(control->getScale(), endScale,
		deltaTime*transitionSpeed);


	Vector2 diffScale = endScale - newscale;
	if (diffScale.x <= .01 && diffScale.y <= .01) {
		control->setScale(endScale);
		//OutputDebugString(L"Finished!");
		return true;
	}

	control->setScale(newscale);
	return false;
}

void TransitionEffects::GrowTransition::reset(IElement2D* control) {
	control->setScale(startScale);
}




TransitionEffects::ShrinkTransition::ShrinkTransition(
	const Vector2& startScale, const Vector2& endScale, float speed)
	: GrowTransition(startScale, endScale, speed) {
}

bool TransitionEffects::ShrinkTransition::run(double deltaTime, IElement2D* control) {

	Vector2 newscale = Vector2::Lerp(control->getScale(), endScale,
		deltaTime*transitionSpeed);

	Vector2 diffScale = newscale - endScale;
	if (diffScale.x <= .1 && diffScale.y <= .1) {
		control->setScale(endScale);
		//OutputDebugString(L"Finished!");
		return true;
	}

	control->setScale(newscale);

	return false;
}



TransitionEffects::SlideTransition::SlideTransition(const Vector2& startPos,
	const Vector2& endPos, float speed) {

	startPosition = startPos;
	endPosition = endPos;
	transitionSpeed = speed;
}

bool TransitionEffects::SlideTransition::run(double deltaTime,
	IElement2D* control) {

	Vector2 newpos = Vector2::Lerp(
		control->getPosition(), endPosition, deltaTime*transitionSpeed);

	Vector2 diffPos = endPosition - newpos;
	if (diffPos.x <= 3 && diffPos.y <= 3) {
		control->setPosition(endPosition);
		//OutputDebugString(L"Slide Finished!\n");
		return true;
	}
	control->setPosition(newpos);
	return false;
}

void TransitionEffects::SlideTransition::reset(IElement2D* control) {
	control->setPosition(startPosition);
}



TransitionEffects::SlideAndGrowTransition::SlideAndGrowTransition(
	const Vector2& startPos, const Vector2& endPos,
	const Vector2& startScl, const Vector2& endScl, float speed) {

	startPosition = startPos;
	endPosition = endPos;
	startScale = startScl;
	endScale = endScl;
	transitionSpeed = speed;
}

int waitingCount = 0;
bool TransitionEffects::SlideAndGrowTransition::run(double deltaTime, IElement2D* control) {

	Vector2 newpos = Vector2::Lerp(
		control->getPosition(), endPosition, deltaTime * transitionSpeed);
	Vector2 newscale = Vector2::Lerp(control->getScale(), endScale,
		deltaTime * transitionSpeed);

	Vector2 diffPos = endPosition - newpos;
	if (diffPos.x <= 3 && diffPos.y <= 3) {
		control->setPosition(endPosition);
		//OutputDebugString(L"Positioning Finished!\n");
		positioningDone = true;
	} else
		control->setPosition(newpos);

	Vector2 diffScale = endScale - newscale;
	if (diffScale.x <= .005 && diffScale.y <= .005) {
		control->setScale(endScale);
		//OutputDebugString(L"Scale Finished!\n");
		scalingDone = true;
	} else
		control->setScale(newscale);

	return positioningDone && scalingDone;
}

void TransitionEffects::SlideAndGrowTransition::reset(IElement2D* control) {

	control->setPosition(startPosition);
	control->setScale(startScale);
	scalingDone = false;
	positioningDone = false;

	waitingCount = 0;
}


#include "../Controls/Dialog.h"
TransitionEffects::TrueGrowTransition::TrueGrowTransition(
	Dialog* containerControl, const Vector2& startScl,
	const Vector2& endScl, float speed) {

	startScale = startScl;
	endScale = endScl;
	transitionSpeed = speed;

	elements = containerControl->getElements();
	for (IElement2D* element : elements) {
		endPositions.push_back(element->getPosition());
	}
}

int waitCount = 0;
bool TransitionEffects::TrueGrowTransition::run(double deltaTime, IElement2D* container) {


	bool allControlsDone = true;
	int i = 0;
	for (IElement2D* element : elements) {

		Vector2 endPos = endPositions[i++];
		Vector2 newpos = Vector2::Lerp(
			element->getPosition(), endPos, deltaTime*transitionSpeed);

		Vector2 diffPos = endPos - newpos;
		if (diffPos.x <= .3 && diffPos.y <= .3) {
			element->setPosition(endPos);
		} else {
			element->setPosition(newpos);
			allControlsDone = false;
		}
	}


	Vector2 newscale = Vector2::Lerp(container->getScale(), endScale,
		deltaTime*transitionSpeed);


	Vector2 diffScale = endScale - newscale;
	if (diffScale.x <= .01 && diffScale.y <= .01) {
		container->setScale(endScale);
		containerDone = true;
	} else
		container->setScale(newscale);

	return containerDone && allControlsDone;
}

void TransitionEffects::TrueGrowTransition::reset(IElement2D* containerControl) {

	// finish up transition
	int i = 0;
	if (!containerDone) {
		for (IElement2D* element : elements)
			element->setPosition(endPositions[i++]);
		containerControl->setScale(endScale);
	}

	containerDone = false;

	containerControl->setScale(startScale);
	i = 0;
	for (IElement2D* element : elements) {

		endPositions[i++] = element->getPosition();
		element->setPosition(containerControl->getPosition());
		element->setScale(startScale);
	}

	waitCount = 0;
}
/** ****** * **/




/******* TexturedTransitions ******/
TransitionEffects::TexturedTransition::TexturedTransition(
	GUIControl* control, float speed) {


	gfxAsset.reset(control->createTexture());
	texture = gfxAsset->getTexture();
	viewRect.left = 0;
	viewRect.top = 0;
	viewRect.right = gfxAsset->getWidth();
	viewRect.bottom = gfxAsset->getHeight();

	origin = Vector2(gfxAsset->getWidth() / 2, gfxAsset->getHeight() / 2);
	position = control->getPosition();

	transitionSpeed = speed;
}


bool TransitionEffects::TexturedTransition::draw(SpriteBatch* batch) {

	batch->Draw(texture.Get(), position, &viewRect,
		tint, rotation, origin, scale, SpriteEffects_None);
	return true;
}
/******* */




/******* */
TransitionEffects::BlindsTransition::BlindsTransition(
	GUIControl* control, float time, bool vertical, bool horizontal)
	: TexturedTransition(control, time) {

	endScale = control->getScale();
	if (vertical) {
		startScale.x = 0;
	}
	if (horizontal) {
		startScale.y = 0;
	}

	int row = ceil((float) gfxAsset->getWidth() / squareSize);
	int col = ceil((float) gfxAsset->getHeight() / squareSize);

	for (int j = 0; j < col; ++j) {
		vector<RECT> rowVect;
		for (int i = 0; i < row; ++i) {
			RECT square;
			square.left = i*squareSize;
			square.top = j*squareSize;
			if (i + 1 == row)
				square.right = square.left + gfxAsset->getWidth() % squareSize;
			else
				square.right = square.left + squareSize;
			if (j + 1 == col)
				square.bottom = square.top + gfxAsset->getHeight() % squareSize;
			else
				square.bottom = square.top + squareSize;

			rowVect.push_back(square);
		}
		squareRects.push_back(rowVect);
	}
}

bool TransitionEffects::BlindsTransition::run(double deltaTime, IElement2D* control) {

	scale = Vector2::Lerp(startScale, endScale, timer / transitionSpeed);
	timer += deltaTime;
	return timer >= transitionSpeed;

}

bool TransitionEffects::BlindsTransition::draw(SpriteBatch* batch) {

	Vector2 squarePos = position;
	for (int j = 0; j < squareRects.size(); ++j) {
		squarePos.x = position.x;
		for (int i = 0; i < squareRects[j].size(); ++i) {
			batch->Draw(texture.Get(), squarePos, &squareRects[j][i],
				tint, rotation, Vector2::Zero, scale, SpriteEffects_None);
			squarePos.x += squareSize;
		}
		squarePos.y += squareSize;
	}

	return true;
}

void TransitionEffects::BlindsTransition::reset(IElement2D* control) {

	position = control->getPosition();
	timer = 0;
}
/******* */



/******* */
TransitionEffects::SpinGrowTransition::SpinGrowTransition(
	GUIControl* control, float transitionTime)
	: TexturedTransition(control, transitionTime) {

	origin = Vector2(gfxAsset->getWidth() / 2, gfxAsset->getHeight() / 2);
	position.x += gfxAsset->getWidth() / 2;
	position.y += gfxAsset->getHeight() / 2;

	startScale = Vector2(0, 0);
	endScale = control->getScale();
	scale = startScale;
}


bool TransitionEffects::SpinGrowTransition::run(double deltaTime, IElement2D* control) {

	timer += deltaTime;
	scale = Vector2::Lerp(startScale, endScale, timer / transitionSpeed);
	rotation = (/*rotation * (1.0f - timer/ transitionSpeed)*/0)
		+ (XM_PI * 4 * timer / transitionSpeed);


	return timer >= transitionSpeed;

}

void TransitionEffects::SpinGrowTransition::reset(IElement2D* control) {

	scaleDone = false;
	rotationDone = false;
	scale = startScale;
	timer = 0;
	rotation = 0;

	position = control->getPosition();
	position.x += gfxAsset->getWidth() / 2;
	position.y += gfxAsset->getHeight() / 2;
}



TransitionEffects::SplitTransition::SplitTransition(
	GUIControl* control, int sWidth, float speed) : TexturedTransition(control, speed) {

	screenWidth = sWidth;


	viewRect.right = gfxAsset->getWidth() / 2;


	viewRectRight.left = gfxAsset->getWidth() / 2;
	viewRectRight.top = 0;
	viewRectRight.right = gfxAsset->getWidth();
	viewRectRight.bottom = gfxAsset->getHeight();

	positionRight = position;
	position.x = -gfxAsset->getWidth() / 2;
	positionRight.x = screenWidth;

	origin = Vector2::Zero;
}

bool TransitionEffects::SplitTransition::run(double deltaTime, IElement2D* control) {

	double change = deltaTime *transitionSpeed * 10;
	position.x += change;
	positionRight.x -= change;
	if (position.x >= control->getPosition().x)
		return true;
	return false;
}

void TransitionEffects::SplitTransition::reset(IElement2D* control) {

	position.y = control->getPosition().y;
	positionRight.y = control->getPosition().y;

	position.x = -gfxAsset->getWidth() / 2;
	positionRight.x = screenWidth;

	int center = control->getPosition().x + control->getWidth() / 2;
	int differenceRight = positionRight.x - center;
	int differenceLeft = center - (position.x + viewRect.right);

	if (differenceRight > differenceLeft)
		position.x += center - differenceRight;
	else
		positionRight.x = center + differenceLeft;
}

bool TransitionEffects::SplitTransition::draw(SpriteBatch* batch) {

	batch->Draw(texture.Get(), position, &viewRect,
		tint, rotation, origin, scale, SpriteEffects_None);
	batch->Draw(texture.Get(), positionRight, &viewRectRight,
		tint, rotation, origin, scale, SpriteEffects_None);
	return true;
}

