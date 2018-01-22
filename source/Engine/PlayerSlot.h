#pragma once


#include <memory>
#include <deque>
#include <vector>
#include <sstream>
#include "../globals.h"
#include "../DXTKGui/Controllers/Joystick.h"

using namespace std;

extern CRITICAL_SECTION cs_activeSlotsAccess;

class JoyData;

/** This class is how the game communicates with the player input.
	The windows message pump sends data to the joysticks and the game queries
	the playerslots for needed data. */
class PlayerSlot {
public:
	PlayerSlot(PlayerSlotNumber slotNum);
	virtual ~PlayerSlot();

	void waiting();
	void finishInit();


	/** Returns true if pair was succesful. Returns false if slot already paired. */
	bool pairWithSocket(JoyData* joyData);
	void unpairSocket();

	JoyData* getJoyData();

	PlayerSlotNumber getPlayerSlotNumber();
	/** If no joy, no player :( */
	bool hasJoystick();
	Joystick* getStick();


private:
	Joystick* joystick = NULL;
	PlayerSlotNumber slotNumber;

	/* For temporary initialization purposes only! Do not use! */
	JoyData* _threadJoystickData;
};

extern vector<shared_ptr<PlayerSlot>> activeSlots;
extern deque<shared_ptr<PlayerSlot>> waitingSlots;

class PlayerSlotManager {
public:
	PlayerSlotManager();
	virtual ~PlayerSlotManager();

	bool checkXInputSlotNumber(USHORT inputSlotNum);
	/* GamePads are added as soon as they are discovered. */
	void addGamePad(shared_ptr<GamePadJoystick> newPad);
	void updateGamePads();

	void waiting();

	void gamePadRemoved(shared_ptr<Joystick> joystick);
	void controllerRemoved(shared_ptr<Joystick> joystick);
	void controllerTryingToPair(JoyData* joyData);
	/** Called from ControllerListener::playerAcceptedSlot(). */
	void finalizePair(JoyData* joyData);

	/** Ordered by PlayerSlotNumber. Always equals MAX_PLAYERS, but
		not all are always in use. */
	shared_ptr<PlayerSlot>  playerSlots[MAX_PLAYERS];

private:
	CRITICAL_SECTION cs_waitingJoysticks;

	enum WaitingForInputTast {
		ADD_TO_LIST, REMOVE_FROM_LIST, CHECK_FOR_CONFIRM
	};
	void accessWaitingSlots(size_t task, PVOID pvoid);
	void accessActiveSlots(size_t task, PVOID pvoid);

	vector<shared_ptr<GamePadJoystick>> gamepads;
};

class ControllerListener;
/** This class is used for passing awaiting joysticks around threads. */
struct JoyData {

	JoyData(shared_ptr<Joystick> joy, ControllerListener* conListener, bool waitForInpt)
		: joystick(joy), listener(conListener), waitForInput(waitForInpt) {
	}
	virtual ~JoyData() {
		wostringstream wss;
		wss << "Slot " << joystick->getControllerSockerNumber() << " data deleting" << endl;
		OutputDebugString(wss.str().c_str());
	}


	ControllerListener* listener;
	shared_ptr<Joystick> joystick;

	// Should the Slot Manager wait for player input or connect instantly? */
	bool waitForInput;
	bool finishFlag = false;
};