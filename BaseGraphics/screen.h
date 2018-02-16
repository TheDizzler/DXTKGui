#pragma once

#include "../Controllers/MouseController.h"
#include "../Controllers/Joystick.h"

class GameManager;

/** An interface class for a game screen; menu screens,
	level screens, etc. */
class Screen {
public:
	virtual bool initialize(ComPtr<ID3D11Device> device) = 0;
	virtual void setGameManager(GameManager* game) = 0;
	virtual void update(double deltaTime) = 0;
	virtual void draw(SpriteBatch* batch) = 0;
	virtual void pause() = 0;
	/** Action to perform when a joystick has been deregistered.
		Controller socket is virtual, used to track controllers in some sort of list.*/
	virtual void controllerRemoved(ControllerSocketNumber controllerSlot,
		PlayerSlotNumber slotNumber) = 0;
		/** Actionto perform when a new joystick has been registered. */
	virtual void newController(shared_ptr<Joystick> newStick) = 0;

	/** Override this function to get funky with your screen transitions. */
	virtual void textureDraw(SpriteBatch* batch) {
		draw(batch);
	};

};