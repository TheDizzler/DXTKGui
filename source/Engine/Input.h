#pragma once

#include <Windows.h>
#include <Keyboard.h>


#include "../DXTKGui/Controllers/MouseController.h"
#include "../Managers/PlayerSlot.h"

extern unique_ptr<PlayerSlotManager> slotManager;



DWORD WINAPI waitForHUDThread(PVOID pVoid);
DWORD WINAPI waitForPlayerThread(PVOID pVoid);



class ControllerListener {
public:
	ControllerListener();
	virtual ~ControllerListener();


	void addJoysticks(vector<HANDLE> handles);

	void parseRawInput(PRAWINPUT pRawInput);

	virtual void newController(HANDLE joyHandle) = 0;
	virtual void controllerRemoved(size_t controllerSlot) = 0;


	void unpairedJoystickRemoved(JoyData* joyData);
	void playerAcceptedSlot(JoyData* joyData);

	bool matchFound(vector<HANDLE> newHandles, HANDLE joystickHandle);

	/** This is a virtual representation of physical controller ports. */
	shared_ptr<Joystick> joystickPorts[MAX_PLAYERS];
protected:
	bool gameInitialized = false;
	map<HANDLE, shared_ptr<Joystick>> joystickMap;
	deque<USHORT> availableControllerSockets;

	/* Thread Safe. */
	bool socketsAvailable();
	/** Thread safe. Does not check if any slots available.*/
	USHORT getNextAvailableControllerSocket();

	enum SharedResourceTask {
		CHECK_SOCKETS_AVAILBLE, GET_NEXT_AVAILABLE
	};
	USHORT sharedResource(size_t task);

	CRITICAL_SECTION cs_availableControllerSockets;


};


class Input : public ControllerListener {
public:
	Input();
	virtual ~Input();

	bool initRawInput(HWND hwnd);



protected:

	unique_ptr<Keyboard> keys;
	shared_ptr<MouseController> mouse;


};