#include "Input.h"

unique_ptr<PlayerSlotManager> slotManager;

bool endAllThreadsNow = false;
bool slotManagerThreadRunning = false;


Input::Input() {
}


Input::~Input() {
	ReleaseCapture();
	ShowCursor(true);
}

#include "../Engine/GameEngine.h"
bool Input::initRawInput(HWND hwnd) {

	keys = make_unique<Keyboard>();
	mouse = make_shared<MouseController>(hwnd);

	return true;
}



ControllerListener::ControllerListener() {

	for (int i = 0; i < MAX_PLAYERS; ++i) {
		availableControllerSockets.push_back(
			ControllerSocketNumber(ControllerSocketNumber::SOCKET_1 + i));
	}

	slotManager = make_unique<PlayerSlotManager>();

	InitializeCriticalSection(&cs_availableControllerSockets);
}

ControllerListener::~ControllerListener() {


	endAllThreadsNow = true;
	slotManager.reset();
	joystickMap.clear();


	DeleteCriticalSection(&cs_availableControllerSockets);
}

void ControllerListener::addGamePad(HANDLE handle) {

	if (!socketsAvailable() || numGamePads >= 3) {
		// too many players
		return;
	}


	DWORD dwResult;
	for (USHORT xInputSlotNum = 0; xInputSlotNum < XUSER_MAX_COUNT; ++xInputSlotNum) {
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		// Simply get the state of the controller from XInput.
		dwResult = XInputGetState(xInputSlotNum, &state);

		if (dwResult == ERROR_SUCCESS && !slotManager->checkXInputSlotNumber(xInputSlotNum)) {
			OutputDebugString(L"Attempting to link new GamePad!\n");

			shared_ptr<GamePadJoystick> newPad = make_shared<GamePadJoystick>(
				getNextAvailableControllerSocket(), xInputSlotNum);
			newPad->registerNewHandle(handle);
			slotManager->addGamePad(newPad);

			numGamePads++;


			JoyData* data = new JoyData(newPad, this);

			DWORD id;
			if (gameInitialized)
				CreateThread(NULL, 0, waitForPlayerThread, (PVOID) data, 0, &id);
			else
				CreateThread(NULL, 0, waitForHUDThread, (PVOID) data, 0, &id);
			joystickMap[handle] = move(newPad);
			break;
		}
	}
}
void ControllerListener::addJoysticks(vector<HANDLE> handles) {

	if (handles.size() < joystickMap.size()) {
		// joystick was removed it - find it!
		HANDLE found = NULL;
		shared_ptr<Joystick> foundJoy;
		for (const auto& joyDev : joystickMap) {
			if (matchFound(handles, joyDev.first))
				continue;

			foundJoy = joyDev.second;
			found = joyDev.first;

			if (foundJoy->getXInputSlot() >= 0) {
				XINPUT_STATE state;
				ZeroMemory(&state, sizeof(XINPUT_STATE));
				DWORD dwResult = XInputGetState(foundJoy->getXInputSlot(), &state);

				if (dwResult == ERROR_DEVICE_NOT_CONNECTED) {
					// it's disconnected....so remove it!
					map<HANDLE, shared_ptr<Joystick>>::iterator mapIt;
					mapIt = joystickMap.find(found);
					if (mapIt != joystickMap.end()) {

						joystickMap.erase(mapIt);

						availableControllerSockets.push_back(foundJoy->socket);
						foundJoy->registerNewHandle(NULL);

						OutputDebugString(L"GamePad removed\n");
						--numGamePads;
					}
					PlayerSlotNumber slotNumber = foundJoy->getPlayerSlotNumber();
					slotManager->gamePadRemoved(foundJoy);
					controllerRemoved(foundJoy->socket, slotNumber);

					foundJoy = NULL;
					found = NULL;
					found = false;
					break;
				}

				foundJoy = NULL;
				found = NULL;
				found = false;
			} else {
				break;
			}
		}
		while (found) {

			map<HANDLE, shared_ptr<Joystick>>::iterator mapIt;
			mapIt = joystickMap.find(found);
			if (mapIt != joystickMap.end()) {

				joystickMap.erase(mapIt);

				availableControllerSockets.push_back(foundJoy->socket);
				foundJoy->registerNewHandle(NULL);

				OutputDebugString(L"Joystick removed\n");

				PlayerSlotNumber slotNumber = foundJoy->getPlayerSlotNumber();
				if (slotNumber != PlayerSlotNumber::NONE) {
					slotManager->controllerRemoved(foundJoy);
				}
				controllerRemoved(foundJoy->socket, slotNumber);
			}
			found = NULL;

			for (const auto& joyDev : joystickMap) {
				if (matchFound(handles, joyDev.first))
					continue;
				foundJoy = joyDev.second;
				found = joyDev.first;
				if (foundJoy->getXInputSlot() >= 0) {
					XINPUT_STATE state;
					ZeroMemory(&state, sizeof(XINPUT_STATE));
					DWORD dwResult = XInputGetState(foundJoy->getXInputSlot(), &state);

					if (dwResult == ERROR_DEVICE_NOT_CONNECTED) {
						// it's disconnected....so remove it!
						map<HANDLE, shared_ptr<Joystick>>::iterator mapIt;
						mapIt = joystickMap.find(found);
						if (mapIt != joystickMap.end()) {

							joystickMap.erase(mapIt);

							availableControllerSockets.push_back(foundJoy->socket);
							foundJoy->registerNewHandle(NULL);

							OutputDebugString(L"GamePad removed\n");
							--numGamePads;
						}
						PlayerSlotNumber slotNumber = foundJoy->getPlayerSlotNumber();
						slotManager->gamePadRemoved(foundJoy);
						controllerRemoved(foundJoy->socket, slotNumber);

						foundJoy = NULL;
						found = NULL;
						found = false;
						break;
					}

					foundJoy = NULL;
					found = NULL;
					found = false;
				} else {
					break;
				}
			}
		}

	} else if (handles.size() > joystickMap.size()) {
		for (const auto& newHandle : handles) {
			if (joystickMap.find(newHandle) != joystickMap.end()) {
				//OutputDebugString(L"That joystick already registered.\n");
			} else if (gameInitialized) {
				if (!socketsAvailable()) {
					// too many players
					return;
				}

				shared_ptr<RawInputJoystick> newJoy
					= make_shared<RawInputJoystick>(getNextAvailableControllerSocket());
				newJoy->registerNewHandle(newHandle);

				joystickMap[newHandle] = newJoy;

				OutputDebugString(L"Attempting to link new Joystick!\n");

				JoyData* data = new JoyData(newJoy, this);
				DWORD id;
				CreateThread(NULL, 0, waitForPlayerThread, (PVOID) data, 0, &id);


			} else {

				// Controllers connected at launch
				shared_ptr<RawInputJoystick> newJoy
					= make_shared<RawInputJoystick>(getNextAvailableControllerSocket());
				newJoy->registerNewHandle(newHandle);
				joystickMap[newHandle] = newJoy;


				OutputDebugString(L"New joystick found!\n");

				// Create a thread to ping the HUD until it's ready
				JoyData* data = new JoyData(newJoy, this);
				DWORD id;
				CreateThread(NULL, 0, waitForHUDThread, (PVOID) data, 0, &id);
			}
		}
	}
}



void ControllerListener::parseRawInput(PRAWINPUT pRawInput) {

	HANDLE handle = pRawInput->header.hDevice;
	if (joystickMap.find(handle) != joystickMap.end())
		joystickMap[handle]->parseRawInput(pRawInput);

}

bool ControllerListener::matchFound(vector<HANDLE> newHandles, HANDLE joystickHandle) {

	for (HANDLE newHandle : newHandles)
		if (newHandle == joystickHandle)
			return true;
	return false;
}



bool ControllerListener::socketsAvailable() {

	USHORT anySlots = sharedResource(CHECK_SOCKETS_AVAILBLE);
	return anySlots > 0;
}


USHORT ControllerListener::sharedResource(size_t task) {

	USHORT returnValue;
	EnterCriticalSection(&cs_availableControllerSockets);
	{

		switch (task) {
			case CHECK_SOCKETS_AVAILBLE:
				returnValue = availableControllerSockets.size();
				break;
			case GET_NEXT_AVAILABLE:
				returnValue = availableControllerSockets[0];
				availableControllerSockets.pop_front();
				break;
		}
	}
	LeaveCriticalSection(&cs_availableControllerSockets);

	return returnValue;
}


ControllerSocketNumber ControllerListener::getNextAvailableControllerSocket() {

	ControllerSocketNumber nextAvailableSlot
		= (ControllerSocketNumber) sharedResource(GET_NEXT_AVAILABLE);

	return nextAvailableSlot;
}


void ControllerListener::unpairedJoystickRemoved(JoyData* joyData) {

	map<HANDLE, shared_ptr<Joystick>>::iterator mapIt;
	mapIt = joystickMap.find(joyData->joystick->getHandle());
	if (mapIt != joystickMap.end())
		joystickMap.erase(mapIt);
}


void ControllerListener::playerAcceptedSlot(JoyData* joyData) {
	slotManager->finalizePair(joyData);
	newController(joyData->joystick);
}


DWORD WINAPI slotManagerThread(PVOID pVoid) {

	if (slotManagerThreadRunning)
		return 0;
	slotManagerThreadRunning = true;

	wostringstream wss;
	wss << L"SlotManager Thread #" << GetCurrentThreadId() << endl;
	OutputDebugString(wss.str().c_str());

	while (waitingSlots.size() > 0) {
		slotManager->waiting();
		if (endAllThreadsNow) {
			slotManagerThreadRunning = false;
			return 0;
		}
		//Sleep(100);
	}

	wss.clear();
	wss << L"SlotManager Thread #" << GetCurrentThreadId() << L" finishing." << endl;
	OutputDebugString(wss.str().c_str());

	slotManagerThreadRunning = false;
	return 0;
}

DWORD WINAPI waitForPlayerThread(PVOID pVoid) {

	JoyData* joyData = (JoyData*) pVoid;

	slotManager->controllerTryingToPair(joyData);
	while (!joyData->finishFlag) {
		// thread is now waiting for the player to confirm their Player Slot
		if (endAllThreadsNow) {

			delete joyData;
			return 0;
		}

		if (joyData->joystick->getPlayerSlotNumber() == PlayerSlotNumber::NONE) {
			joyData->listener->unpairedJoystickRemoved(joyData);
			delete joyData;
			return 0;
		}

		Sleep(250);
	}

	joyData->listener->playerAcceptedSlot(joyData);

	delete joyData;
	return 0;

}

/** PlayerSlots need to pair with HUD dialog boxes. */
DWORD WINAPI waitForHUDThread(PVOID pVoid) {

	JoyData* joyData = (JoyData*) pVoid;

	while (guiOverlay == NULL) { // Jus hol up
		//wostringstream wss;
		//wss << L"Test Thread #" << GetCurrentThreadId() << endl;
		//wss << data->joystick->socket << " waiting for HUD." << endl;
		//OutputDebugString(wss.str().c_str());

		if (endAllThreadsNow) {
			delete joyData;
			return 0;
		}
		Sleep(500);
	}


	slotManager->controllerTryingToPair(joyData);
	switch (joyData->joystick->getPlayerSlotNumber()) {
		case PlayerSlotNumber::NONE:
			// Too many players? Shouldn't come up?
			OutputDebugString(L"WARNING: this defitinely should not have been called");
			delete joyData;
			return 0;
		case PlayerSlotNumber::SLOT_1:
			// first player auto-select
			break;
		default:
			while (!joyData->finishFlag) {
				// thread is now waiting for the player to confirm their Player Slot
				if (endAllThreadsNow) {
					delete joyData;
					return 0;
				}

				if (joyData->joystick->getPlayerSlotNumber() == PlayerSlotNumber::NONE) {
					joyData->listener->unpairedJoystickRemoved(joyData);
					delete joyData;
					return 0;
				}

				Sleep(500);
			}
			break;
	}

	joyData->listener->playerAcceptedSlot(joyData);
	delete joyData;
	return 0;
}




