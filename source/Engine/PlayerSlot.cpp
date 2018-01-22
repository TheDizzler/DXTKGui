#include "PlayerSlot.h"
#include "GameEngine.h"
#include <algorithm>

vector<shared_ptr<PlayerSlot>> activeSlots;
deque<shared_ptr<PlayerSlot>> waitingSlots;
CRITICAL_SECTION cs_activeSlotsAccess;


PlayerSlot::PlayerSlot(PlayerSlotNumber slotNum) : slotNumber(slotNum) {
}

PlayerSlot::~PlayerSlot() {
}

void PlayerSlot::waiting() {

	if (joystick == NULL) {
		GameEngine::errorMessage(L"Joystick non-existant");
	} else if (joystick->aButtonPushed() && _threadJoystickData) {
		_threadJoystickData->finishFlag = true;
		// after this the waiting thread will execute ControllerListener->playerAcceptedSlot(joyData)
		_threadJoystickData = NULL;
	}
}

void PlayerSlot::finishInit() {
	if (_threadJoystickData)
		_threadJoystickData->finishFlag = true;
	_threadJoystickData = NULL;
}

bool PlayerSlot::pairWithSocket(JoyData* joyData) {
	if (joystick != NULL) {
		wostringstream wss;
		wss << L"Player Slot " << slotNumber << " already connected" << endl;
		OutputDebugString(wss.str().c_str());
		return false;
	}
	joystick = joyData->joystick.get();
	joystick->setPlayerSlotNumber(slotNumber);


	_threadJoystickData = joyData;

	wostringstream wss;
	wss << L"Connecting Socket " << joystick->getControllerSockerNumber()
		<< L" to Player Slot " << slotNumber << endl;
	OutputDebugString(wss.str().c_str());

	if (!joyData->waitForInput) {
		_threadJoystickData->finishFlag = true;
	}
	return true;
}

void PlayerSlot::unpairSocket() {
	wostringstream wss;
	wss << L"Socket " << joystick->getControllerSockerNumber()
		<< L" and Player Slot " << slotNumber;
	wss << L" unpaired." << endl;
	OutputDebugString(wss.str().c_str());

	joystick = NULL;
	_threadJoystickData = NULL;
}

JoyData* PlayerSlot::getJoyData() {
	return _threadJoystickData;
}

PlayerSlotNumber PlayerSlot::getPlayerSlotNumber() {
	return slotNumber;
}

bool PlayerSlot::hasJoystick() {
	return joystick != NULL;
}

Joystick* PlayerSlot::getStick() {
	return joystick;
}
/** ******** PLAYERSLOT END ******** **/




/** ******** PLAYERSLOT MANAGER START ******** **/
PlayerSlotManager::PlayerSlotManager() {
	InitializeCriticalSection(&cs_waitingJoysticks);
	InitializeCriticalSection(&cs_activeSlotsAccess);

	for (int i = 0; i < MAX_PLAYERS; ++i)
		playerSlots[PlayerSlotNumber(PlayerSlotNumber::SLOT_1 + i)]
		= make_shared<PlayerSlot>(PlayerSlotNumber(PlayerSlotNumber::SLOT_1 + i));
}

PlayerSlotManager::~PlayerSlotManager() {
	activeSlots.clear();
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		playerSlots[i].reset();
	}

	waitingSlots.clear();

	DeleteCriticalSection(&cs_waitingJoysticks);
	DeleteCriticalSection(&cs_activeSlotsAccess);
}

bool PlayerSlotManager::checkXInputSlotNumber(USHORT inputSlotNum) {
	for (const auto& pad : gamepads)
		if (pad->getXInputSlot() == inputSlotNum)
			return true;
	return false;
}

void PlayerSlotManager::addGamePad(shared_ptr<GamePadJoystick> newPad) {
	gamepads.push_back(newPad);
}

void PlayerSlotManager::updateGamePads() {
	for (const auto& pad : gamepads) {
		pad->update();
	}
}

void PlayerSlotManager::waiting() {
	accessWaitingSlots(CHECK_FOR_CONFIRM, NULL);
}

void PlayerSlotManager::gamePadRemoved(shared_ptr<Joystick> gamePad) {

	wstringstream wss;
	wss << "GamePad in PlayerSlot " << gamePad->getPlayerSlotNumber() << " removed" << endl;
	OutputDebugString(wss.str().c_str());


	JoyData* joydata = playerSlots[gamePad->getPlayerSlotNumber()]->getJoyData();
	if (joydata && joydata->waitForInput)
		accessWaitingSlots(REMOVE_FROM_LIST, joydata);

	PlayerSlotNumber slotNum = gamePad->getPlayerSlotNumber();
	accessActiveSlots(REMOVE_FROM_LIST, (PVOID) &slotNum);
	playerSlots[gamePad->getPlayerSlotNumber()]->unpairSocket();

	gamepads.erase(remove(gamepads.begin(), gamepads.end(),
		gamePad), gamepads.end());

}


void PlayerSlotManager::controllerRemoved(shared_ptr<Joystick> joystick) {

	wstringstream wss;
	wss << "Controller PlayerSlot " << joystick->getPlayerSlotNumber() << " removed" << endl;
	OutputDebugString(wss.str().c_str());

	JoyData* joydata = playerSlots[joystick->getPlayerSlotNumber()]->getJoyData();
	if (joydata && joydata->waitForInput)
		accessWaitingSlots(REMOVE_FROM_LIST, joydata);

	PlayerSlotNumber slotNum = joystick->getPlayerSlotNumber();
	accessActiveSlots(REMOVE_FROM_LIST, (PVOID) &slotNum);
	playerSlots[joystick->getPlayerSlotNumber()]->unpairSocket();
}

void PlayerSlotManager::controllerTryingToPair(JoyData* joyData) {

	accessWaitingSlots(ADD_TO_LIST, joyData);

}

/** Called from ControllerListener::playerAcceptedSlot(). */
void PlayerSlotManager::finalizePair(JoyData* joyData) {

	if (joyData->waitForInput)
		accessWaitingSlots(REMOVE_FROM_LIST, joyData);

	PlayerSlotNumber plyrSltNum = joyData->joystick->getPlayerSlotNumber();
	accessActiveSlots(ADD_TO_LIST, &plyrSltNum);

	playerSlots[joyData->joystick->getPlayerSlotNumber()]->finishInit();
}

void PlayerSlotManager::accessWaitingSlots(size_t task, PVOID pvoid) {

	JoyData* joyData = (JoyData*) pvoid;

	EnterCriticalSection(&cs_waitingJoysticks);
	//OutputDebugString(L"\nEntering CS -> ");
	switch (task) {
		case ADD_TO_LIST:
			for (int i = 0; i < MAX_PLAYERS; ++i) {
				if (playerSlots[i]->pairWithSocket(joyData)) {
					if (joyData->waitForInput) {
						waitingSlots.push_back(playerSlots[i]);
						DWORD id;
						CreateThread(NULL, 0, slotManagerThread, (PVOID) 0, 0, &id);
					}
					break;
				}
			}
			break;

		case REMOVE_FROM_LIST:
			waitingSlots.erase(remove(waitingSlots.begin(), waitingSlots.end(),
				playerSlots[joyData->joystick->getPlayerSlotNumber()]));
			break;

		case CHECK_FOR_CONFIRM:
			for (const auto& slot : waitingSlots)
				slot->waiting();
			break;

	}
	//OutputDebugString(L"Exiting CS\n");
	LeaveCriticalSection(&cs_waitingJoysticks);
}

void PlayerSlotManager::accessActiveSlots(size_t task, PVOID pvoid) {

	PlayerSlotNumber playerSlotNumber = *((PlayerSlotNumber*) pvoid);

	EnterCriticalSection(&cs_activeSlotsAccess);
	//OutputDebugString(L"\nEntering CS -> ");
	switch (task) {
		case ADD_TO_LIST:
			activeSlots.push_back(playerSlots[playerSlotNumber]);
			break;
		case REMOVE_FROM_LIST:
			activeSlots.erase(remove(activeSlots.begin(), activeSlots.end(),
				playerSlots[playerSlotNumber]), activeSlots.end());
			break;

	}
	//OutputDebugString(L"Exiting CS\n");
	LeaveCriticalSection(&cs_activeSlotsAccess);
}