#pragma once

#include "../Effects/TransitionEffects.h"
#include "../Controls/TextLabel.h"
#include "../Controllers/MouseController.h"
#include "../Controllers/KeyboardController.h"


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
};