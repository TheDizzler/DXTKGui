#include "Input.h"

unique_ptr<PlayerSlotManager> slotManager;

bool endAllThreadsNow = false;


Input::Input() {
	int in = 9;
}


Input::~Input() {
	mouse.reset();
	ReleaseCapture();
	ShowCursor(true);
}

#include "../Engine/GameEngine.h"
bool Input::initRawInput(HWND hwnd) {

	keys = make_unique<Keyboard>();
	mouse = make_shared<MouseController>(hwnd);
	if (guiFactory != NULL)
		mouse->loadMouseIcon(guiFactory.get(), "Mouse Icon");

	return true;
}



ControllerListener::ControllerListener() {

	for (int i = 0; i < MAX_PLAYERS; ++i) {
		joystickPorts[i] = make_shared<Joystick>(i);
		availableControllerSockets.push_back(i);
	}

	slotManager = make_unique<PlayerSlotManager>();

	InitializeCriticalSection(&cs_availableControllerSockets);
}

ControllerListener::~ControllerListener() {

	for (int i = 0; i < MAX_PLAYERS; ++i)
		joystickPorts[i].reset();


	endAllThreadsNow = true;
	joystickMap.clear();

	deque<USHORT> empty;
	swap(availableControllerSockets, empty);

	DeleteCriticalSection(&cs_availableControllerSockets);
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
			break;
		}
		while (found) {


			map<HANDLE, shared_ptr<Joystick>>::iterator mapIt;
			mapIt = joystickMap.find(found);
			if (mapIt != joystickMap.end()) {

				joystickMap.erase(mapIt);

				availableControllerSockets.push_back(foundJoy->socket);
				foundJoy->registerNewHandle(NULL);

				OutputDebugString(L"Joystick removed\n");

				slotManager->controllerRemoved(foundJoy->playerSlotNumber);
				controllerRemoved(foundJoy->socket);

			}
			found = NULL;

			for (const auto& joyDev : joystickMap) {
				if (matchFound(handles, joyDev.first))
					continue;
				foundJoy = joyDev.second;
				found = joyDev.first;
				break;
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


				shared_ptr<Joystick> newJoy = joystickPorts[getNextAvailableControllerSocket()];
				newJoy->registerNewHandle(newHandle);

				joystickMap[newHandle] = newJoy;

				OutputDebugString(L"Attempting to link new Joystick!\n");

				JoyData* data = new JoyData(newJoy, this);
				DWORD id;
				CreateThread(NULL, 0, waitForPlayerThread, (PVOID) data, 0, &id);

			} else {

				// Controllers connected at launch
				shared_ptr<Joystick> newStick = joystickPorts[getNextAvailableControllerSocket()];
				newStick->registerNewHandle(newHandle);
				joystickMap[newHandle] = newStick;


				OutputDebugString(L"New joystick found!\n");

				// Create a thread to ping the HUD until it's ready
				JoyData* data = new JoyData(newStick, this);
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


USHORT ControllerListener::getNextAvailableControllerSocket() {

	USHORT nextAvailableSlot = sharedResource(GET_NEXT_AVAILABLE);

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
	newController(joyData->joystick->getHandle());
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

		if (joyData->joystick->playerSlotNumber == -1) {
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
	switch (joyData->joystick->playerSlotNumber) {
		case -1:
			// Too many players? Shouldn't come up?
			OutputDebugString(L"WARNING: this defitinely should not have been called");
			delete joyData;
			return 0;
		case 0:
			// first player auto-select
			break;
		default:
			while (!joyData->finishFlag) {
				// thread is now waiting for the player to confirm their Player Slot
				if (endAllThreadsNow) {

					delete joyData;
					return 0;
				}

				if (joyData->joystick->playerSlotNumber == -1) {
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




