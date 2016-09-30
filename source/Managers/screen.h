#pragma once

#include "../BaseGraphics/TransitionEffects.h"
#include "../Controls/TextLabel.h"
#include "../Engine/MouseController.h"
#include "../Engine/KeyboardController.h"


class GameManager;

/** An interface class for a game screen; menu screens,
	level screens, etc. */
interface Screen {
public:
	virtual bool initialize(ComPtr<ID3D11Device> device, MouseController* mouse) = 0;
	virtual void setGameManager(GameManager* game) = 0;
	virtual void pause() = 0;
	virtual void update(double deltaTime, MouseController* mouse) = 0;
	virtual void draw(SpriteBatch* batch) = 0;
	virtual void drawScreenTransition(SpriteBatch* batch) = 0;

	virtual bool openScreen(double deltaTime) = 0;
	virtual bool closeScreen(double deltaTime) = 0;

	virtual void setOpenTransition(TransitionEffects::ScreenTransition* effect) {
		if (openTransition != NULL)
			delete openTransition;
		else {
			runTransition = &TransitionEffects::ScreenTransition::run;
			resetTransition = &TransitionEffects::ScreenTransition::reset;
			drawTransition = &TransitionEffects::ScreenTransition::draw;
		}
		openTransition = effect;
	}
	virtual void setCloseTransition(TransitionEffects::ScreenTransition* effect) {
		if (closeTransition != NULL)
			delete closeTransition;
		else {
			runTransition = &TransitionEffects::ScreenTransition::run;
			resetTransition = &TransitionEffects::ScreenTransition::reset;
			drawTransition = &TransitionEffects::ScreenTransition::draw;
		}
		closeTransition = effect;
	}

	virtual void resetScreenTransition() {
		if (currentTransition != NULL)
			(currentTransition->*resetTransition)(this);
	};
protected:
	TransitionEffects::ScreenTransition* openTransition = NULL;
	TransitionEffects::ScreenTransition* closeTransition = NULL;
	TransitionEffects::RunScreen runTransition;
	TransitionEffects::ResetScreen resetTransition;
	TransitionEffects::DrawScreen drawTransition;

	TransitionEffects::ScreenTransition* currentTransition = NULL;
};