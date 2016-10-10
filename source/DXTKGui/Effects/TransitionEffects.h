#pragma once

//#include "../../globals.h"


//#include <vector>
#include "../Controls/GUIControl.h"

class Dialog;
class Screen;
namespace TransitionEffects {

	/* Non-linear effects will never actually conclude. Effects should check against
	a threshold to decide when it's logically finished, if not mathmatically
	finished. Adjust to feel. */
	class TransitionEffect {
	public:

		/* Returns true when transition effect is finished. */
		virtual bool run(double deltaTime, IElement2D* control) = 0;
		virtual void reset(IElement2D* control) = 0;
		/** This must return false if not being used! */
		virtual bool draw(SpriteBatch* batch) {
			return false;
		}
		float transitionSpeed;


	};
	/* Returns true when transition effect is finished. */
	typedef bool (TransitionEffect::*Run) (double, IElement2D*);
	typedef void (TransitionEffect::*Reset) (IElement2D*);
	typedef bool (TransitionEffect::*Draw) (SpriteBatch*);

	class GrowTransition : public TransitionEffect {
	public:
		GrowTransition(const Vector2& startScale, const Vector2& endScale,
			float transitionSpeed = 20);
		virtual bool run(double deltaTime, IElement2D* control) override;
		virtual void reset(IElement2D* control) override;

	protected:
		Vector2 startScale;
		Vector2 endScale;
	};

	class ShrinkTransition : public GrowTransition {
	public:
		ShrinkTransition(const Vector2& startScale, const Vector2& endScale,
			float transitionSpeed = 20);
		virtual bool run(double deltaTime, IElement2D* control) override;
	};

	class SlideTransition : public TransitionEffect {
	public:
		SlideTransition(const Vector2& startPos, const Vector2& endPos,
			float transitionSpeed = 20);
		virtual bool run(double deltaTime, IElement2D* control) override;
		virtual void reset(IElement2D* control) override;

	protected:
		Vector2 startPosition;
		Vector2 endPosition;

	};

	class SlideAndGrowTransition : public TransitionEffect {
	public:
		SlideAndGrowTransition(const Vector2& startPos, const Vector2& endPos,
			const Vector2& startScale, const Vector2& endScale,
			float transitionSpeed = 20);
		virtual bool run(double deltaTime, IElement2D* control) override;
		virtual void reset(IElement2D* control) override;


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
		virtual bool run(double deltaTime, IElement2D* control) override;
		virtual void reset(IElement2D* control) override;

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


	class BlindsTransition : public TexturedTransition {
	public:
		BlindsTransition(GUIControl* control, float transitionTime = .5,
			bool vertical = true, bool horizontal = false);
		virtual bool run(double deltaTime, IElement2D* control) override;
		virtual void reset(IElement2D* control) override;
		virtual bool draw(SpriteBatch* batch) override;
	private:
		vector<vector<RECT>> squareRects;
		int squareSize = 32;
		Vector2 startScale = Vector2(1, 1);
		Vector2 endScale;

		double timer = 0;
	};


	class SpinGrowTransition : public TexturedTransition {
	public:
	/** Time for transition to complete. */
		SpinGrowTransition(GUIControl* control, float transitionTime = .5);
		virtual bool run(double deltaTime, IElement2D* control) override;
		virtual void reset(IElement2D* control) override;
	private:
		Vector2 startScale;
		Vector2 endScale;

		bool rotationDone = false;
		bool scaleDone = false;
		double timer = 0;
	};


	class SplitTransition : public TexturedTransition {
	public:
		SplitTransition(GUIControl* control, int screenWidth, float transitionSpeed = 50.0);
		virtual bool run(double deltaTime, IElement2D* control) override;
		virtual void reset(IElement2D* control) override;

		virtual bool draw(SpriteBatch* batch) override;

	private:
		Vector2 positionRight;
		RECT viewRectRight;
		int screenWidth;
	};

};