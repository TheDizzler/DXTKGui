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


	Vector2 diff = newscale - endScale;
	if (diff.x > -threshold && diff.y > -threshold) {
		control->setScale(endScale);
		OutputDebugString(L"Finished!");
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

	Vector2 diff = newscale - endScale;
	if (diff.x < threshold && diff.y < threshold) {
		control->setScale(endScale);
		OutputDebugString(L"Finished!");
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

	Vector2 diff = newpos - endPosition;
	if (diff.x > -threshold && diff.y > -threshold) {
		control->setPosition(endPosition);
		OutputDebugString(L"Slide Finished!\n");
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

bool TransitionEffects::SlideAndGrowTransition::run(double deltaTime, GUIControl* control) {

	Vector2 newpos = Vector2::Lerp(
		control->getPosition(), endPosition, deltaTime*transitionSpeed);
	Vector2 newscale = Vector2::Lerp(control->getScale(), endScale,
		deltaTime*transitionSpeed);

	Vector2 diffPos = newpos - endPosition;
	if (diffPos.x > -threshold && diffPos.y > -threshold) {
		control->setPosition(endPosition);
		//OutputDebugString(L"Positioning Finished!");
		positioningDone = true;
	} else
		control->setPosition(newpos);

	Vector2 diffScale = newscale - endScale;
	if (diffScale.x > -threshold && diffScale.y > -threshold) {
		control->setScale(endScale);
		//OutputDebugString(L"Scale Finished!");
		scalingDone = true;
	} else
		control->setScale(newscale);

	return positioningDone && scalingDone;
}

void TransitionEffects::SlideAndGrowTransition::reset(GUIControl* control) {

	control->setPosition(startPosition);
	control->setScale(startScale);
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
		element->setPosition(containerControl->getPosition()/*Vector2::Zero*/);
		element->setScale(startScale);
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

		Vector2 diffPos = newpos - endPos;
		if (diffPos.x > -threshold * 1000 && diffPos.y > -threshold * 1000) {
			element->setPosition(endPos);
		} else {
			element->setPosition(newpos);
			allControlsDone = false;
		}
	}
	if (allControlsDone)
		OutputDebugString(L"Controls Finished!\n");

	Vector2 newscale = Vector2::Lerp(container->getScale(), endScale,
		deltaTime*transitionSpeed);


	Vector2 diffScale = newscale - endScale;
	if (diffScale.x > -threshold || diffScale.y > -threshold) {
		container->setScale(endScale);
		OutputDebugString(L"Container Finished!\n");
		containerDone = true;
	} else
		container->setScale(newscale);


	if (!(containerDone && allControlsDone))
		++waitCount;
	else {
		wostringstream wss;
		wss << "WaitCount: " << waitCount << "\n";
		OutputDebugString(wss.str().c_str());
	}

	return containerDone && allControlsDone;
	}

	void TransitionEffects::TrueGrowTransition::reset(GUIControl* containerControl) {

		containerDone = false;
		containerControl->setScale(startScale);
		for (IElement2D* element : elements) {
			element->setPosition(containerControl->getPosition());
			element->setScale(startScale);
		}

		waitCount = 0;
	}
