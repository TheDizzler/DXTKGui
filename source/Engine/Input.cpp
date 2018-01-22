#include "Input.h"
#include "../Engine/GameEngine.h"

unique_ptr<PlayerSlotManager> slotManager;
unique_ptr<KeyboardController> keys;
unique_ptr<MouseController> mouse;

bool endAllThreadsNow = false;
bool slotManagerThreadRunning = false;
bool Input::gameInitialized = false;

Input::Input() {

	waitForInput = false;
}

Input::~Input() {
	endAllThreadsNow = true;
	ReleaseCapture();
	ShowCursor(true);
}

bool Input::initRawInput(HWND hwnd) {

	keys = make_unique<KeyboardController>();
	mouse = make_unique<MouseController>(hwnd);

	return true;
}



ControllerListener::ControllerListener() {

	for (int i = 0; i < MAX_PLAYERS; ++i) {
		availableControllerSockets.push_back(
			ControllerSocketNumber(ControllerSocketNumber::SOCKET_1 + i));
	}

	slotManager = make_unique<PlayerSlotManager>();
}

ControllerListener::~ControllerListener() {

	slotManager.reset();
	joystickMap.clear();

	availableControllerSockets.clear();
}


void ControllerListener::addJoysticks(vector<ControllerDevice> controllerDevices) {

	if (controllerDevices.size() < joystickMap.size()) {
		// joystick was removed it - find it!
		HANDLE found = NULL;
		shared_ptr<Joystick> foundJoy;
		for (const auto& joyDev : joystickMap) {
			if (matchFound(controllerDevices, joyDev.first))
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

						makeControllerSocketAvailable(foundJoy->getControllerSockerNumber());
						foundJoy->registerNewHandle(NULL);

						OutputDebugString(L"GamePad removed\n");
						--numGamePads;
					}
					PlayerSlotNumber slotNumber = foundJoy->getPlayerSlotNumber();
					ControllerSocketNumber socket = foundJoy->getControllerSockerNumber();
					slotManager->gamePadRemoved(foundJoy);
					controllerRemoved(socket, slotNumber);

					foundJoy.reset();
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

				makeControllerSocketAvailable(foundJoy->getControllerSockerNumber());
				foundJoy->registerNewHandle(NULL);

				OutputDebugString(L"Joystick removed\n");

				PlayerSlotNumber slotNumber = foundJoy->getPlayerSlotNumber();
				ControllerSocketNumber socket = foundJoy->getControllerSockerNumber();
				if (slotNumber != PlayerSlotNumber::NONE) {
					slotManager->controllerRemoved(foundJoy);
				}
				controllerRemoved(socket, slotNumber);
			}
			found = NULL;

			for (const auto& joyDev : joystickMap) {
				if (matchFound(controllerDevices, joyDev.first))
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

							makeControllerSocketAvailable(foundJoy->getControllerSockerNumber());
							foundJoy->registerNewHandle(NULL);

							OutputDebugString(L"GamePad removed\n");
							--numGamePads;
						}
						ControllerSocketNumber socket
							= foundJoy->getControllerSockerNumber();
						PlayerSlotNumber slotNumber = foundJoy->getPlayerSlotNumber();
						slotManager->gamePadRemoved(foundJoy);
						controllerRemoved(socket, slotNumber);

						foundJoy.reset();
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

	} else if (controllerDevices.size() > joystickMap.size()) {
		for (const auto& newDevice : controllerDevices) {
			if (joystickMap.find(newDevice.handle) != joystickMap.end()) {
				//OutputDebugString(L"That joystick already registered.\n");
			} else if (gameInitialized) {
				if (!socketsAvailable()) {
					// too many players
					return;
				}
				if (!newDevice.isRawInput) {
					addGamePad(newDevice.handle);
					continue;
				}

				shared_ptr<RawInputJoystick> newJoy
					= make_shared<RawInputJoystick>(getNextAvailableControllerSocket());
				newJoy->registerNewHandle(newDevice.handle);

				joystickMap[newDevice.handle] = newJoy;

				OutputDebugString(L"Attempting to link new Joystick!\n");

				JoyData* data = new JoyData(newJoy, this, waitForInput);
				if (waitForInput) {
					DWORD id;
					CreateThread(NULL, 0, waitForPlayerThread, (PVOID) data, 0, &id);
				} else {
					slotManager->controllerTryingToPair(data);
					playerAcceptedSlot(data);
				}

			} else {

				if (!newDevice.isRawInput) {
					addGamePad(newDevice.handle);
					continue;
				}
				// Controllers connected at launch
				shared_ptr<RawInputJoystick> newJoy
					= make_shared<RawInputJoystick>(getNextAvailableControllerSocket());
				newJoy->registerNewHandle(newDevice.handle);
				joystickMap[newDevice.handle] = newJoy;


				OutputDebugString(L"New joystick found!\n");

				// Create a thread to ping the HUD until it's ready
				JoyData* data = new JoyData(newJoy, this, waitForInput);
				DWORD id;
				CreateThread(NULL, 0, waitForHUDThread, (PVOID) data, 0, &id);
			}
		}
	}
}

void ControllerListener::addGamePad(HANDLE handle) {

	if (numGamePads >= XUSER_MAX_COUNT) {
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


			JoyData* data = new JoyData(newPad, this, waitForInput);

			DWORD id;
			if (gameInitialized) {
				if (waitForInput) {
					CreateThread(NULL, 0, waitForPlayerThread, (PVOID) data, 0, &id);
				} else {
					slotManager->controllerTryingToPair(data);
					playerAcceptedSlot(data);
				}
			} else
				CreateThread(NULL, 0, waitForHUDThread, (PVOID) data, 0, &id);
			joystickMap[handle] = move(newPad);
			break;
		}
	}
}

void ControllerListener::parseRawInput(PRAWINPUT pRawInput) {

	HANDLE handle = pRawInput->header.hDevice;
	if (joystickMap.find(handle) != joystickMap.end())
		joystickMap[handle]->parseRawInput(pRawInput);

}

bool ControllerListener::matchFound(vector<ControllerDevice> controllerDevices,
	HANDLE joystickHandle) {

	for (ControllerDevice device : controllerDevices)
		if (device.handle == joystickHandle)
			return true;
	return false;
}



bool ControllerListener::socketsAvailable() {

	//USHORT anySlots = sharedResource(CHECK_SOCKETS_AVAILABLE);
	//return anySlots > 0;
	return availableControllerSockets.size() > 0;
}

//
//USHORT ControllerListener::sharedResource(size_t task, ControllerSocketNumber socketNum) {
//
//	USHORT returnValue;
//	EnterCriticalSection(&cs_availableControllerSockets);
//	{
//
//		switch (task) {
//			case CHECK_SOCKETS_AVAILABLE:
//				returnValue = availableControllerSockets.size();
//				break;
//			case GET_NEXT_AVAILABLE:
//				returnValue = availableControllerSockets[0];
//				availableControllerSockets.pop_front();
//				break;
//			case ADD_AVAILABLE:
//				if (socketNum == ControllerSocketNumber::NONE) {
//					GameEngine::errorMessage(
//				} else
//						availableControllerSockets.push_back(socketNum);
//					break;
//		}
//	}
//	LeaveCriticalSection(&cs_availableControllerSockets);
//
//	return returnValue;
//}


ControllerSocketNumber ControllerListener::getNextAvailableControllerSocket() {

	ControllerSocketNumber nextAvailableSlot
		//= (ControllerSocketNumber) sharedResource(GET_NEXT_AVAILABLE);
		//= availableControllerSockets[0];
		= availableControllerSockets.front();
	availableControllerSockets.pop_front();

	return nextAvailableSlot;
}

void ControllerListener::makeControllerSocketAvailable(
	ControllerSocketNumber socketNumber) {

	if (socketNumber == ControllerSocketNumber::CONTROLLER_SOCKET_ERROR) {
		GameEngine::errorMessage(L"Unknown socket number attempting to release.",
			L"Controller Socket Number release Error");
	} else {
		list<ControllerSocketNumber>::iterator itr;
		//list<ControllerSocketNumber>::iterator last = availableControllerSockets.begin();
		for (itr = availableControllerSockets.begin();
			itr != availableControllerSockets.end(); ++itr) {
			if (*itr > socketNumber) {
				availableControllerSockets.insert(itr, socketNumber);
				break;
			}
			//last = itr;
		}
		//availableControllerSockets.push_front(socketNumber);
	}
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
	delete joyData;
}

vector<shared_ptr<PlayerSlot>> ControllerListener::getJoystickList() {
	return activeSlots;
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

	return 0;

}

/** PlayerSlots need to pair with HUD dialog boxes. */
DWORD WINAPI waitForHUDThread(PVOID pVoid) {

	JoyData* joyData = (JoyData*) pVoid;
	
	while (Input::gameInitialized == false) { // Jus hol up
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

	if (!joyData->waitForInput) {
		slotManager->controllerTryingToPair(joyData);
		joyData->listener->playerAcceptedSlot(joyData);
		return 0;
	}
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
			//while (!joyData->finishFlag) {
			//	// thread is now waiting for the player to confirm their Player Slot
			//	if (endAllThreadsNow) {
			//		delete joyData;
			//		return 0;
			//	}

			//	if (joyData->joystick->getPlayerSlotNumber() == PlayerSlotNumber::NONE) {
			//		joyData->listener->unpairedJoystickRemoved(joyData);
			//		delete joyData;
			//		return 0;
			//	}

			//	Sleep(500);
			//}
			//break;

			DWORD id;
			CreateThread(NULL, 0, waitForPlayerThread, (PVOID) joyData, 0, &id);

			return 0;
	}

	return 0;
}




