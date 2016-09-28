#include "../pch.h"
#pragma once

#include "../Controls/GUIControl.h"

class Dialog;
namespace TransitionEffects {

	class TransitionEffect {
	public:

		/* Returns true when transition effect is finished. */
		virtual bool run(double deltaTime, GUIControl* control) = 0;
		virtual void reset(GUIControl* control) = 0;
		virtual bool draw(SpriteBatch* batch) {
			return false;
		}
		float transitionSpeed;

		/* Some effects will never actually conclude. Effects should check against
			this threshold to decide when it's logically finished, if not mathmatically
			finished. Adjust to feel. */
		//float threshold = .9;
	};
	/* Returns true when transition effect is finished. */
	typedef bool (TransitionEffect::*Run) (double, GUIControl*);
	typedef void (TransitionEffect::*Reset) (GUIControl*);
	typedef bool (TransitionEffect::*Draw) (SpriteBatch*);

	class GrowTransition : public TransitionEffect {
	public:
		GrowTransition(const Vector2& startScale, const Vector2& endScale,
			float transitionSpeed = 20);
		virtual bool run(double deltaTime, GUIControl* control) override;
		virtual void reset(GUIControl* control) override;

	protected:
		Vector2 startScale;
		Vector2 endScale;
	};

	class ShrinkTransition : public GrowTransition {
	public:
		ShrinkTransition(const Vector2& startScale, const Vector2& endScale,
			float transitionSpeed = 20);
		virtual bool run(double deltaTime, GUIControl * control) override;
	};

	class SlideTransition : public TransitionEffect {
	public:
		SlideTransition(const Vector2& startPos, const Vector2& endPos,
			float transitionSpeed = 20);
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
			float transitionSpeed = 20);
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
			float transitionSpeed = 20);
		virtual bool run(double deltaTime, GUIControl* control) override;
		virtual void reset(GUIControl * control) override;

	protected:
		Vector2 startScale;
		Vector2 endScale;
		vector<IElement2D*> elements;
		vector<Vector2> endPositions;

		bool containerDone = false;
	};


	/* A base class for transitions that require advanced graphical features. */
	class TexturedTransition : public TransitionEffect {
	public:
		TexturedTransition(GUIControl* control, float transitionSpeed);

		virtual bool draw(SpriteBatch* batch) override;

	protected:
		std::unique_ptr<GraphicsAsset> gfxAsset;
		ComPtr<ID3D11ShaderResourceView> texture;

		Vector2 position = Vector2::Zero;
		Vector2 origin = Vector2::Zero;
		Vector2 scale = Vector2(1, 1);
		Color tint = Color(1, 1, 1, 1);
		float rotation = 0.0f;
		RECT viewRect;

	};


	class SpinTransition : public TexturedTransition {
	public:
		SpinTransition(GUIControl* control, float transitionSpeed = 5.0);
		virtual bool run(double deltaTime, GUIControl* control) override;
		virtual void reset(GUIControl * control) override;
	private:
		Vector2 startScale;
		Vector2 endScale;

		bool rotationDone = false;
		bool scaleDone = false;
	};


	class SplitTransition : public TexturedTransition {
	public:
		SplitTransition(GUIControl* control, float transitionSpeed = 50.0);
		virtual bool run(double deltaTime, GUIControl * control) override;
		virtual void reset(GUIControl * control) override;

		virtual bool draw(SpriteBatch* batch) override;

	private:
		Vector2 positionRight;
		RECT viewRectRight;
	};

};