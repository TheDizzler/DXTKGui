#include "../pch.h"
#pragma once

#include "../Controls/GUIControl.h"

namespace TransitionEffects {

	class TransitionEffect {
	public:
		/*virtual void set(const Vector2& startState, const Vector2& endState,
			float transitionSpeed) = 0;*/
		   /* Returns if transition effect is finished. */
		virtual bool run(double deltaTime, GUIControl* control) = 0;
		virtual void reset(GUIControl* control) = 0;

		Vector2 startState;
		Vector2 endState;
		float transitionSpeed;
	};
	/*typedef void (TransitionEffect::*SetTransition)
		(const Vector2&, const Vector2&, float);*/
	typedef bool (TransitionEffect::*Run) (double, GUIControl*);
	typedef void (TransitionEffect::*Reset) (GUIControl*);


	class GrowTransition : public TransitionEffect {
	public:
		GrowTransition(const Vector2& startScale, const Vector2& endScale,
			float transitionSpeed);
		virtual bool run(double deltaTime, GUIControl* control) override;
		virtual void reset(GUIControl* control) override;
	};

	class SlideTransition : public TransitionEffect {
	public:
		SlideTransition(const Vector2& startPos, const Vector2& endPos,
			float transitionSpeed);
		virtual bool run(double deltaTime, GUIControl* control) override;
		virtual void reset(GUIControl * control) override;

	};
};