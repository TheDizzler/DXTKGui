#pragma once

#include <list>

#include "../DXTKGui/Controllers/MouseController.h"
#include "../DXTKGui/Controllers/KeyboardController.h"
#include "PlayerSlot.h"

extern unique_ptr<PlayerSlotManager> slotManager;
extern unique_ptr<KeyboardController> keys;
extern unique_ptr<MouseController> mouse;

DWORD WINAPI waitForHUDThread(PVOID pVoid);
DWORD WINAPI waitForPlayerThread(PVOID pVoid);
DWORD WINAPI slotManagerThread(PVOID pVoid);

struct ControllerDevice {
	HANDLE handle;
	/** False is XInput. */
	bool isRawInput;
};

class ControllerListener {
public:
	ControllerListener();
	virtual ~ControllerListener();

	void addJoysticks(vector<ControllerDevice> controllerDevices);

	void parseRawInput(PRAWINPUT pRawInput);

	virtual void newController(shared_ptr<Joystick> newStick) = 0;
	virtual void controllerRemoved(ControllerSocketNumber controllerSocket,
		PlayerSlotNumber slotNumber) = 0;


	void unpairedJoystickRemoved(JoyData* joyData);
	void playerAcceptedSlot(JoyData* joyData);

	vector<shared_ptr<PlayerSlot>> getJoystickList();

	static bool gameInitialized;
protected:
	/** Should SlotManager wait for player input before connecting controller to slot? */
	bool waitForInput;
	
	USHORT numGamePads = 0;

	map<HANDLE, shared_ptr<Joystick>> joystickMap;
	list<ControllerSocketNumber> availableControllerSockets;

	void addGamePad(HANDLE handle);

	bool socketsAvailable();
	/** Does not check if any slots available.*/
	ControllerSocketNumber getNextAvailableControllerSocket();
	void makeControllerSocketAvailable(ControllerSocketNumber socketNumber);
	/** Searches through joysticks in use to see if "new" stick is actually new. */
	bool matchFound(vector<ControllerDevice> controllerDevices, HANDLE joystickHandle);
};


class Input : public ControllerListener {
public:
	Input();
	virtual ~Input();

	bool initRawInput(HWND hwnd);

protected:

};