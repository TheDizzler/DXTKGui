#pragma once

#include "../Controllers/MouseController.h"
#include "../Controllers/KeyboardController.h"


class GameManager;

/** An interface class for a game screen; menu screens,
	level screens, etc. */
interface Screen {
public:
	virtual bool initialize(ComPtr<ID3D11Device> device, shared_ptr<MouseController> mouse) = 0;
	virtual void setGameManager(GameManager* game) = 0;
	virtual void update(double deltaTime,
		/*KeyboardController* keys, */shared_ptr<MouseController> mouse) = 0;
	virtual void draw(SpriteBatch* batch) = 0;
	virtual void pause() = 0;
	
};