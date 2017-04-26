#pragma once

#include "../Controllers/MouseController.h"


class GameManager;

/** An interface class for a game screen; menu screens,
	level screens, etc. */
interface Screen {
public:
	virtual bool initialize(ComPtr<ID3D11Device> device, shared_ptr<MouseController> mouse) = 0;
	virtual void setGameManager(GameManager* game) = 0;
	virtual void update(double deltaTime) = 0;
	virtual void draw(SpriteBatch* batch) = 0;
	virtual void pause() = 0;
	/** Action to perform when a joystick has been deregistered.
		Controller socket is virtual, used to track controllers in some sort of list.*/
	virtual void controllerRemoved(size_t controllerSlot) = 0;
	virtual void newController(HANDLE joyHandle) = 0;
	
};