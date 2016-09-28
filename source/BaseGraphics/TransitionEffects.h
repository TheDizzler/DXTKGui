#include "../pch.h"
#pragma once

#include "../Controls/GUIControl.h"

class Dialog;
namespace TransitionEffects {

	class TransitionEffect {
	public:

		/* Returns if transition effect is finished. */
		virtual bool run(double deltaTime, GUIControl* control) = 0;
		virtual void reset(GUIControl* control) = 0;

		float transitionSpeed;

		/* Some effects will never actually conclude. Effects should check against
			this threshold to decide when it's logically finished, if not mathmatically
			finished. Adjust to feel. */
		float threshold = .001;
	};

	typedef bool (TransitionEffect::*Run) (double, GUIControl*);
	typedef void (TransitionEffect::*Reset) (GUIControl*);


	class GrowTransition : public TransitionEffect {
	public:
		GrowTransition(const Vector2& startScale, const Vector2& endScale,
			float transitionSpeed = 10);
		virtual bool run(double deltaTime, GUIControl* control) override;
		virtual void reset(GUIControl* control) override;
	protected:
		Vector2 startScale;
		Vector2 endScale;
	};

	class ShrinkTransition : public GrowTransition {
	public:
		ShrinkTransition(const Vector2& startScale, const Vector2& endScale,
			float transitionSpeed);
		virtual bool run(double deltaTime, GUIControl * control) override;
	};

	class SlideTransition : public TransitionEffect {
	public:
		SlideTransition(const Vector2& startPos, const Vector2& endPos,
			float transitionSpeed);
		virtual bool run(double deltaTime, GUIControl* control) override;
		virtual void reset(GUIControl * control) override;
	protected:
		Vector2 startPosition;
		Vector2 endPosition;

	};

	class SlideAndGrowTransition : public TransitionEffect {
	public:
		SlideAndGrowTransition(const Vector2& startPos, const Vector2& endPos,
			const Vector2& startScale, const Vector2& endScale,
			float transitionSpeed);
		virtual bool run(double deltaTime, GUIControl * control) override;
		virtual void reset(GUIControl * control) override;

	protected:
		Vector2 startPosition;
		Vector2 endPosition;
		Vector2 startScale;
		Vector2 endScale;

		bool positioningDone = false;
		bool scalingDone = false;
	};


	class TrueGrowTransition : public TransitionEffect {
	public:
		TrueGrowTransition(Dialog* containerControl,
			const Vector2& startScale, const Vector2& endScale,
			float transitionSpeed);
		virtual bool run(double deltaTime, GUIControl* control) override;
		virtual void reset(GUIControl * control) override;

	protected:
		Vector2 startScale;
		Vector2 endScale;
		vector<IElement2D*> elements;
		vector<Vector2> endPositions;
		//map<IElement2D*, Vector2> endPositions;

		bool containerDone = false;
	};
};