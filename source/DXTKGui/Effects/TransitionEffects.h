#pragma once

#include "../Controls/GUIControl.h"

class Dialog;
class Screen;
namespace TransitionEffects {

	/* Non-linear effects will never actually conclude. Effects should check against
	a threshold to decide when it's logically finished, if not mathmatically
	finished. Adjust to feel. */
	class TransitionEffect {
	public:
		TransitionEffect(IElement2D* cntrl) {
			control = cntrl;
		}
		virtual ~TransitionEffect();

		/** This must be run in SetTransition Function. */
		virtual void initializeEffect(Texturizable* cntrl) {
		};

/* Returns true when transition effect is finished. */
		virtual bool run(double deltaTime) = 0;
		virtual void reset() = 0;
		//virtual void reloadGraphicAsset();
		/** This must return false if not being used! */
		virtual bool draw(SpriteBatch* batch) {
			return false;
		}
		float transitionSpeed;

	protected:
		IElement2D* control;
	};

	/* Returns true when transition effect is finished. */
	typedef bool (TransitionEffect::*Run) (double);
	typedef void (TransitionEffect::*Reset) ();
	typedef bool (TransitionEffect::*Draw) (SpriteBatch*);

	class GrowTransition : public TransitionEffect {
	public:
		GrowTransition(IElement2D* cntrl, const Vector2& startScale, const Vector2& endScale,
			float transitionSpeed = 20);
		virtual bool run(double deltaTime) override;
		virtual void reset() override;

	protected:
		Vector2 startScale;
		Vector2 endScale;
	};

	class ShrinkTransition : public GrowTransition {
	public:
		ShrinkTransition(IElement2D* cntrl, const Vector2& startScale, const Vector2& endScale,
			float transitionSpeed = 20);
		virtual bool run(double deltaTime) override;
	};

	class SlideTransition : public TransitionEffect {
	public:
		SlideTransition(IElement2D* cntrl, const Vector2& startPos, const Vector2& endPos,
			float transitionSpeed = 20);
		virtual bool run(double deltaTime) override;
		virtual void reset() override;

	protected:
		Vector2 startPosition;
		Vector2 endPosition;

	};

	class SlideAndGrowTransition : public TransitionEffect {
	public:
		SlideAndGrowTransition(IElement2D* cntrl, const Vector2& startPos, const Vector2& endPos,
			const Vector2& startScale, const Vector2& endScale,
			float transitionSpeed = 20);
		virtual bool run(double deltaTime) override;
		virtual void reset() override;


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
		virtual ~TrueGrowTransition();

		virtual bool run(double deltaTime) override;
		virtual void reset() override;

	protected:
		Vector2 startScale;
		Vector2 endScale;
		vector<IElement2D*> elements;
		vector<Vector2> endPositions;

		bool containerDone = false;
		Dialog* containerControl;
	};


	/* A base class for transitions that require advanced graphical features.
	WARNING: Every Texturizable needs to be an IElement2D as well.
	(They should be anyway, but yeah, you know, I'm just saying). */
	class TexturedTransition : public TransitionEffect {
	public:
		TexturedTransition(IElement2D* cntrl, float transitionSpeed);
		virtual ~TexturedTransition();

		virtual void initializeEffect(Texturizable* control) override;
		virtual bool draw(SpriteBatch* batch) override;

	protected:
		unique_ptr<GraphicsAsset> gfxAsset;
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
		BlindsTransition(IElement2D* cntrl, float transitionTime = .5,
			bool vertical = true, bool horizontal = false);
		virtual ~BlindsTransition();

		virtual void initializeEffect(Texturizable* control) override;

		virtual bool run(double deltaTime) override;
		virtual void reset() override;
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
		SpinGrowTransition(IElement2D* cntrl, float transitionTime = .5);

		virtual void initializeEffect(Texturizable* control) override;

		virtual bool run(double deltaTime) override;
		virtual void reset() override;
	private:
		Vector2 startScale;
		Vector2 endScale;

		bool rotationDone = false;
		bool scaleDone = false;
		double timer = 0;
	};


	class SplitTransition : public TexturedTransition {
	public:
		SplitTransition(IElement2D* cntrl, int screenWidth, float transitionSpeed = 150.0);
		virtual ~SplitTransition();

		virtual void initializeEffect(Texturizable* control) override;

		virtual bool run(double deltaTime) override;
		virtual void reset() override;

		virtual bool draw(SpriteBatch* batch) override;

	private:
		Vector2 positionRight;
		RECT viewRectRight;
		int screenWidth;
	};

};