#pragma once

#include "../Controls/GUIFactory.h"

class Screen;
namespace ScreenTransitions {

	class ScreenTransition {
	public:
		virtual void setTransitionBetween(GraphicsAsset* oldScreenAsset,
			GraphicsAsset* newScreenAsset, float transitionTime);

		/* Returns true when transition effect is finished. */
		virtual bool run(double deltaTime) = 0;
		virtual void draw(SpriteBatch* batch) = 0;
		virtual void reset() = 0;

		float transitionTime;

	protected:
		unique_ptr<GraphicsAsset> oldScreenAsset;
		unique_ptr<GraphicsAsset> newScreenAsset;
		ComPtr<ID3D11ShaderResourceView> oldTexture;
		ComPtr<ID3D11ShaderResourceView> newTexture;

		double timer = 0;

		/* SourceRect for whole screen. */
		RECT screenRect;
	};
	/** Return true when transition complete. */
	/*typedef bool (ScreenTransition::*RunScreen) (double, Screen*);
	typedef void (ScreenTransition::*DrawScreen) (SpriteBatch*);
	typedef void (ScreenTransition::*ResetScreen) (Screen*);*/


	class ScreenTransitionManager {
	public:
		ScreenTransitionManager(GUIFactory* guiFactory, const char_t* bgName);
		~ScreenTransitionManager();

		void setTransition(ScreenTransition* effect);
		void transitionBetween(Screen* oldScreen, Screen* newScreen,
			float transitionTime = .5);

		/** Return true when transition complete. */
		bool runTransition(double deltaTime);
		void drawTransition(SpriteBatch* batch);
	private:
		ScreenTransition* transition = NULL;
		unique_ptr<Sprite> bg;

		GUIFactory* guiFactory;
	};



	class FlipScreenTransition : public ScreenTransition {
	public:
		FlipScreenTransition(bool verticalFlip = false);
		/** Return true when transition complete. */
		virtual bool run(double deltaTime) override;
		virtual void draw(SpriteBatch* batch) override;
		virtual void reset() override;

	private:
		//SpriteEffects currentOrientation;
		//SpriteEffects startOrientation;

		Vector2 position = Vector2::Zero;
		Vector2 origin = Vector2::Zero;
		Vector2 scale = Vector2(1, 1);
		Color tint = Color(1, 1, 1, 1);
		float rotation = 0.0f;
		Vector2 startScale;

		ComPtr<ID3D11ShaderResourceView> texture;

	};


	class SquareFlipScreenTransition : public ScreenTransition {
	public:
		~SquareFlipScreenTransition();

		virtual void setTransitionBetween(GraphicsAsset* oldScreenAsset,
			GraphicsAsset* newScreenAsset, float transitionTime) override;

		virtual bool run(double deltaTime) override;
		virtual void draw(SpriteBatch* batch) override;
		virtual void reset() override;

	private:
		struct Square {

			RECT rect;
			SpriteEffects orientation;
			Vector2 scale;
			Vector2 position;
			Vector2 origin;
			ComPtr<ID3D11ShaderResourceView> texture;

			float timer = 0;
		};
		vector<vector<Square*>> squares;
		vector<int> numInRowActive;

		Vector2 startScale;
		Color tint = Color(1, 1, 1, 1);
		float rotation = 0.0f;

		int maxJ = 1;
		double delay;
	};


	class LineWipeScreenTransition : public ScreenTransition {
	public:
		~LineWipeScreenTransition();
		virtual void setTransitionBetween(GraphicsAsset* oldScreenAsset,
			GraphicsAsset* newScreenAsset, float transitionTime) override;
		virtual bool run(double deltaTime) override;
		virtual void draw(SpriteBatch * batch) override;
		virtual void reset() override;

	private:
		struct Line {
			RECT rect;
			Vector2 position;
			Vector2 start;
			Vector2 end;
		};
		double timer = 0;
		double delay;
		vector<Line*> lines;

		Vector2 position = Vector2::Zero;
		Vector2 origin = Vector2::Zero;
		Vector2 scale = Vector2(1, 1);
		Color tint = Color(1, 1, 1, 1);
		float rotation = 0.0f;
	};


};