#include "TransitionEffects.h"


TransitionEffects::GrowTransition::GrowTransition(const Vector2& strt,
	const Vector2& end, float speed) {

	startState = strt;
	endState = end;
	transitionSpeed = speed;
}

bool TransitionEffects::GrowTransition::run(double deltaTime, GUIControl* control) {

	Vector2 newscale = Vector2::Lerp(control->getScale(), endState,
		deltaTime*transitionSpeed);


	Vector2 diff = newscale - endState;
	if (diff.x > -.001 || diff.y > -.001) {
		control->setScale(endState);
		OutputDebugString(L"Finished!");
		return true;
	}

	control->setScale(newscale);
	return false;
}

void TransitionEffects::GrowTransition::reset(GUIControl* control) {

	control->setScale(startState);
}

TransitionEffects::SlideTransition::SlideTransition(const Vector2& startPos,
	const Vector2& endPos, float speed) {

	startState = startPos;
	endState = endPos;
	transitionSpeed = speed;
}

bool TransitionEffects::SlideTransition::run(double deltaTime,
	GUIControl* control) {

	Vector2 newpos = Vector2::Lerp(
		control->getPosition(), endState, deltaTime*transitionSpeed);

	Vector2 diff = newpos - endState;
	if (diff.x > -.001 || diff.y > -.001) {
		control->setPosition(endState);
		OutputDebugString(L"Finished!");
		return true;
	}
	control->setPosition(newpos);
	return false;
}

void TransitionEffects::SlideTransition::reset(GUIControl* control) {

	control->setPosition(startState);
}
