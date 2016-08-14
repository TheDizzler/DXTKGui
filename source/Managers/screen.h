#pragma once

#include <dinput.h>


#include "../Controls/TextLabel.h"
#include "../Engine/MouseController.h"
#include "../Engine/KeyboardController.h"


class GameManager;

/** An interface class for a game screen; menu screens,
	level screens, etc. */
interface Screen {
public:
	virtual bool initialize(ID3D11Device* device, MouseController* mouse) = 0;
	virtual void setGameManager(GameManager* game) = 0;
	virtual void update(double deltaTime, KeyboardController* keys,
		MouseController* mouse) = 0;
	virtual void draw(SpriteBatch* batch) = 0;
	virtual void pause() = 0;
};