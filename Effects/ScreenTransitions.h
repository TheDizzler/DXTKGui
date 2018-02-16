#pragma once

#include "../GUIFactory.h"

class Screen;
namespace ScreenTransitions {

	class ScreenTransition {
	public:
		virtual void setTransitionBetween(unique_ptr<GraphicsAsset> oldScreenAsset,
			unique_ptr<GraphicsAsset> newScreenAsset, float transitionTime);
		virtual ~ScreenTransition();

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


	class ScreenTransitionManager {
	public:

		ScreenTransitionManager();
		/*ScreenTransitionManager(GUIFactory* guiFactory,
			const char_t* bgName = "Default Transition BG");*/
		//ScreenTransitionManager& ScreenTransitionManager::operator=(ScreenTransitionManager&);
		virtual ~ScreenTransitionManager();

		void initialize(GUIFactory* guiFactory,
			const char_t* bgName = "Default Transition BG", bool resizeBGToFit = true);

		void reloadGraphicsAssets();

		void setTransition(ScreenTransition* effect);
		void transitionBetween(Screen* oldScreen, Screen* newScreen,
			float transitionTime = .5, bool autoBatchDraw = true);

		/** Return true when transition complete. */
		bool runTransition(double deltaTime);
		void drawTransition(SpriteBatch* batch);

		Screen* newScreen = NULL;
	private:
		ScreenTransition* transition = NULL;
		unique_ptr<Sprite> bg;

		GUIFactory* guiFactory;

		bool resizeBGToFit;
	};



	class FlipScreenTransition : public ScreenTransition {
	public:
		FlipScreenTransition(bool verticalFlip = false);
		/** Return true when transition complete. */
		virtual bool run(double deltaTime) override;
		virtual void draw(SpriteBatch* batch) override;
		virtual void reset() override;

	private:

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
		virtual ~SquareFlipScreenTransition();

		virtual void setTransitionBetween(unique_ptr<GraphicsAsset> oldScreenAsset,
			unique_ptr<GraphicsAsset> newScreenAsset, float transitionTime) override;

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

			double timer = 0;
		};
		vector<vector<Square*>> squares;
		vector<size_t> numInRowActive;

		Vector2 startScale;
		Color tint = Color(1, 1, 1, 1);
		float rotation = 0.0f;

		size_t maxJ = 1;
		double delay;
	};


	class LineWipeScreenTransition : public ScreenTransition {
	public:
		LineWipeScreenTransition(bool wipeToLeftFirst = true);
		virtual ~LineWipeScreenTransition();
		virtual void setTransitionBetween(unique_ptr<GraphicsAsset> oldScreenAsset,
			unique_ptr<GraphicsAsset> newScreenAsset, float transitionTime) override;
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

		bool wipeToLeft;
		Vector2 position = Vector2::Zero;
		Vector2 origin = Vector2::Zero;
		Vector2 scale = Vector2(1, 1);
		Color tint = Color(1, 1, 1, 1);
		float rotation = 0.0f;
	};


};