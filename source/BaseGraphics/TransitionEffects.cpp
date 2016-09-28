#include "TransitionEffects.h"


TransitionEffects::GrowTransition::GrowTransition(const Vector2& strt,
	const Vector2& end, float speed) {

	startScale = strt;
	endScale = end;
	transitionSpeed = speed;
}

bool TransitionEffects::GrowTransition::run(double deltaTime, GUIControl* control) {

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

void TransitionEffects::GrowTransition::reset(GUIControl* control) {
	control->setScale(startScale);
}




TransitionEffects::ShrinkTransition::ShrinkTransition(
	const Vector2& startScale, const Vector2& endScale, float speed)
	: GrowTransition(startScale, endScale, speed) {
}

bool TransitionEffects::ShrinkTransition::run(double deltaTime, GUIControl* control) {

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
	GUIControl* control) {

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

void TransitionEffects::SlideTransition::reset(GUIControl* control) {
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
bool TransitionEffects::SlideAndGrowTransition::run(double deltaTime, GUIControl* control) {

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

	/*if (!(positioningDone && scalingDone)) {
		++waitingCount;
	} else {
		wostringstream wss;
		wss << "WaitCount: " << waitingCount << "\n";
		OutputDebugString(wss.str().c_str());
	}*/
	return positioningDone && scalingDone;
}

void TransitionEffects::SlideAndGrowTransition::reset(GUIControl* control) {

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
		//element->setPosition(containerControl->getPosition()/*Vector2::Zero*/);
		//element->setScale(startScale);
	}
}

int waitCount = 0;
bool TransitionEffects::TrueGrowTransition::run(double deltaTime, GUIControl* container) {


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
	//if (allControlsDone)
		//OutputDebugString(L"Controls Finished!\n");

	Vector2 newscale = Vector2::Lerp(container->getScale(), endScale,
		deltaTime*transitionSpeed);


	Vector2 diffScale = endScale - newscale;
	if (diffScale.x <= .01 && diffScale.y <= .01) {
		container->setScale(endScale);
		//OutputDebugString(L"Container Finished!\n");
		containerDone = true;
	} else
		container->setScale(newscale);


	/*if (!(containerDone && allControlsDone))
		++waitCount;
	else {
		wostringstream wss;
		wss << "WaitCount: " << waitCount << "\n";
		OutputDebugString(wss.str().c_str());
	}*/

	return containerDone && allControlsDone;
}

void TransitionEffects::TrueGrowTransition::reset(GUIControl* containerControl) {

	// finish up transition
	int i = 0;
	if (!containerDone) {
		for (IElement2D* element : elements)
			element->setPosition(endPositions[i++]);
		containerControl->setScale(endScale);
	}

	containerDone = false;

	//containerControl->setPosition(containerControl->getPosition());
	containerControl->setScale(startScale);
	i = 0;
	for (IElement2D* element : elements) {

		endPositions[i++] = element->getPosition();
		element->setPosition(containerControl->getPosition());
		element->setScale(startScale);
	}

	waitCount = 0;
}






TransitionEffects::TexturedTransition::TexturedTransition(unique_ptr<GraphicsAsset> texture) {
}

bool TransitionEffects::TexturedTransition::run(double deltaTime, GUIControl * control) {
	return false;
}

void TransitionEffects::TexturedTransition::reset(GUIControl * control) {
}
