#include "Joystick.h"
#include <hidsdi.h>

Joystick::Joystick(ControllerSocketNumber controllerSocket) {
	socket = controllerSocket;
}

Joystick::~Joystick() {
}

void Joystick::registerNewHandle(HANDLE hndl) {
	handle = hndl;
}

void Joystick::setControllerSocketNumber(ControllerSocketNumber socketNumber) {
	socket = socketNumber;
}

ControllerSocketNumber Joystick::getControllerSockerNumber() {
	return socket;
}

void Joystick::setPlayerSlotNumber(PlayerSlotNumber slotNum) {
	playerSlotNumber = slotNum;
}

PlayerSlotNumber Joystick::getPlayerSlotNumber() {
	return playerSlotNumber;
}


HANDLE Joystick::getHandle() {
	return handle;
}
/** ********** BASE JOYSTICK CLASS END ********** **/



/** ********** RAW INPUT JOYSTICK ********** **/
RawInputJoystick::RawInputJoystick(ControllerSocketNumber controllerSocket)
	: Joystick(controllerSocket) {

	ZeroMemory(bButtonStates, sizeof(bButtonStates));
}

RawInputJoystick::~RawInputJoystick() {
}

SHORT RawInputJoystick::getXInputSlot() {
	return -1;
}


void RawInputJoystick::parseRawInput(PRAWINPUT pRawInput) {

	PHIDP_PREPARSED_DATA pPreparsedData = NULL;
	HIDP_CAPS            Caps;
	PHIDP_BUTTON_CAPS    pButtonCaps = NULL;
	PHIDP_VALUE_CAPS     pValueCaps = NULL;
	USHORT               capsLength;
	UINT                 bufferSize;
	HANDLE               hHeap;
	USAGE                usage[MAX_BUTTONS];
	ULONG                i, usageLength, value;

	memmove(lastButtonStates, bButtonStates, sizeof(bButtonStates));
	hHeap = GetProcessHeap();

	//
	// Get the preparsed data block
	//
	if (GetRawInputDeviceInfo(pRawInput->header.hDevice, RIDI_PREPARSEDDATA, NULL, &bufferSize) != 0) {
		return;
	}
	if (!(pPreparsedData = (PHIDP_PREPARSED_DATA) HeapAlloc(hHeap, 0, bufferSize))) {

		return;
	}
	if (!((int) GetRawInputDeviceInfo(
		pRawInput->header.hDevice, RIDI_PREPARSEDDATA, pPreparsedData, &bufferSize) >= 0)) {
		HeapFree(hHeap, 0, pPreparsedData);
		pPreparsedData = NULL;
		return;
	}

	//
	// Get the joystick's capabilities
	//

	// Button caps
	if (HidP_GetCaps(pPreparsedData, &Caps) != HIDP_STATUS_SUCCESS) {
		HeapFree(hHeap, 0, pPreparsedData);
		pPreparsedData = NULL;
		return;
	}
	if (!(pButtonCaps = (PHIDP_BUTTON_CAPS) HeapAlloc(hHeap, 0, sizeof(HIDP_BUTTON_CAPS) * Caps.NumberInputButtonCaps))) {
		HeapFree(hHeap, 0, pPreparsedData);
		pPreparsedData = NULL;
		return;
	}

	capsLength = Caps.NumberInputButtonCaps;
	if (HidP_GetButtonCaps(HidP_Input, pButtonCaps, &capsLength, pPreparsedData) != HIDP_STATUS_SUCCESS) {
		HeapFree(hHeap, 0, pPreparsedData);
		HeapFree(hHeap, 0, pButtonCaps);
		pPreparsedData = NULL;
		pButtonCaps = NULL;
		return;
	}
	g_NumberOfButtons = pButtonCaps->Range.UsageMax - pButtonCaps->Range.UsageMin + 1;

	// Value caps
	if (!(pValueCaps = (PHIDP_VALUE_CAPS) HeapAlloc(hHeap, 0, sizeof(HIDP_VALUE_CAPS) * Caps.NumberInputValueCaps))) {
		HeapFree(hHeap, 0, pPreparsedData);
		HeapFree(hHeap, 0, pButtonCaps);
		pPreparsedData = NULL;
		pButtonCaps = NULL;
		return;

	}
	capsLength = Caps.NumberInputValueCaps;
	if (HidP_GetValueCaps(HidP_Input, pValueCaps, &capsLength, pPreparsedData) != HIDP_STATUS_SUCCESS) {
		HeapFree(hHeap, 0, pPreparsedData);
		HeapFree(hHeap, 0, pButtonCaps);
		HeapFree(hHeap, 0, pValueCaps);
		pPreparsedData = NULL;
		pButtonCaps = NULL;
		pValueCaps = NULL;
		return;
	}

	//
	// Get the pressed buttons
	//
	usageLength = g_NumberOfButtons;
	if (HidP_GetUsages(HidP_Input, pButtonCaps->UsagePage, 0, usage, &usageLength, pPreparsedData,
		(PCHAR) pRawInput->data.hid.bRawData, pRawInput->data.hid.dwSizeHid) != HIDP_STATUS_SUCCESS) {

		HeapFree(hHeap, 0, pPreparsedData);
		HeapFree(hHeap, 0, pButtonCaps);
		HeapFree(hHeap, 0, pValueCaps);
		pPreparsedData = NULL;
		pButtonCaps = NULL;
		pValueCaps = NULL;
		return;
	}

	ZeroMemory(bButtonStates, sizeof(bButtonStates));

	for (i = 0; i < usageLength; i++)
		bButtonStates[usage[i] - pButtonCaps->Range.UsageMin] = TRUE;

	//
	// Get the state of discrete-valued-controls
	//

	for (i = 0; i < Caps.NumberInputValueCaps; i++) {
		if (HidP_GetUsageValue(HidP_Input, pValueCaps[i].UsagePage, 0, pValueCaps[i].Range.UsageMin,
			&value, pPreparsedData, (PCHAR) pRawInput->data.hid.bRawData, pRawInput->data.hid.dwSizeHid)
			!= HIDP_STATUS_SUCCESS) {
			HeapFree(hHeap, 0, pPreparsedData);
			HeapFree(hHeap, 0, pButtonCaps);
			HeapFree(hHeap, 0, pValueCaps);
			pPreparsedData = NULL;
			pButtonCaps = NULL;
			pValueCaps = NULL;
			return;
		}

		switch (pValueCaps[i].Range.UsageMin) {
			case 0x30:	// X-axis
				lAxisX = (LONG) value - 128;
				break;

			case 0x31:	// Y-axis
				lAxisY = (LONG) value - 128;
				break;

				//case 0x32: // Z-axis
				//	lAxisZ = (LONG) value - 128;
				//	break;

				//case 0x35: // Rotate-Z
				//	lAxisRz = (LONG) value - 128;
				//	break;

				//case 0x39:	// Hat Switch
				//	lHat = value;
				//	break;
		}
	}


	HeapFree(hHeap, 0, pPreparsedData);
	HeapFree(hHeap, 0, pButtonCaps);
	HeapFree(hHeap, 0, pValueCaps);
}

Vector2 RawInputJoystick::getDirection() {

	LONG x = 0, y = 0;
	if (lAxisX < -DEAD_ZONE || lAxisX > DEAD_ZONE)
		x = lAxisX;
	if (lAxisY < -DEAD_ZONE || lAxisY > DEAD_ZONE)
		y = lAxisY;
	auto dir = Vector2((float) x, (float) y);
	dir.Normalize();
	return dir;
}

bool RawInputJoystick::isLeftPressed() {
	return lAxisX < -DEAD_ZONE;
}

bool RawInputJoystick::isRightPressed() {
	return lAxisX > DEAD_ZONE;
}

bool RawInputJoystick::isUpPressed() {
	return lAxisY < -DEAD_ZONE;
}

bool RawInputJoystick::isDownPressed() {
	return lAxisY > DEAD_ZONE;
}

bool RawInputJoystick::yButtonPushed() {
	bool pushed = bButtonStates[yButton]
		&& !lastButtonStates[yButton];

	if (pushed) {
		bButtonStates[yButton] = true;
		lastButtonStates[yButton] = true;
	}
	return pushed;
}

bool RawInputJoystick::xButtonPushed() {
	bool pushed = bButtonStates[xButton]
		&& !lastButtonStates[xButton];

	if (pushed) {
		bButtonStates[xButton] = true;
		lastButtonStates[xButton] = true;
	}
	return pushed;
}

bool RawInputJoystick::aButtonPushed() {
	bool pushed = bButtonStates[aButton]
		&& !lastButtonStates[aButton];

	if (pushed) {
		bButtonStates[aButton] = true;
		lastButtonStates[aButton] = true;
	}
	return pushed;
}

bool RawInputJoystick::bButtonPushed() {
	bool pushed = bButtonStates[bButton]
		&& !lastButtonStates[bButton];

	if (pushed) {
		bButtonStates[bButton] = true;
		lastButtonStates[bButton] = true;
	}
	return pushed;
}

bool RawInputJoystick::lButtonPushed() {
	bool pushed = bButtonStates[leftShoulderButton]
		&& !lastButtonStates[leftShoulderButton];

	if (pushed) {
		bButtonStates[leftShoulderButton] = true;
		lastButtonStates[leftShoulderButton] = true;
	}
	return pushed;
}

bool RawInputJoystick::rButtonPushed() {
	bool pushed = bButtonStates[rightShoulderButton]
		&& !lastButtonStates[rightShoulderButton];

	if (pushed) {
		bButtonStates[rightShoulderButton] = true;
		lastButtonStates[rightShoulderButton] = true;
	}
	return pushed;
}

bool RawInputJoystick::startButtonPushed() {
	bool pushed = bButtonStates[startButton]
		&& !lastButtonStates[startButton];

	if (pushed) {
		bButtonStates[startButton] = true;
		lastButtonStates[startButton] = true;
	}
	return pushed;
}

bool RawInputJoystick::selectButtonPushed() {
	return bButtonStates[selectButton] && !lastButtonStates[selectButton];
}


bool RawInputJoystick::yButtonDown() {
	return bButtonStates[yButton];
}

bool RawInputJoystick::xButtonDown() {
	return bButtonStates[xButton];
}

bool RawInputJoystick::aButtonDown() {
	return bButtonStates[aButton];
}

bool RawInputJoystick::bButtonDown() {
	return bButtonStates[bButton];
}

bool RawInputJoystick::lButtonDown() {
	return bButtonStates[leftShoulderButton];
}

bool RawInputJoystick::rButtonDown() {
	return bButtonStates[rightShoulderButton];
}

bool RawInputJoystick::startButtonDown() {
	return bButtonStates[startButton];
}

bool RawInputJoystick::selectButtonDown() {
	return bButtonStates[selectButton];
}
/** ******** RAWINPUT JOYSTICK END ******** **/



/** ******* XINPUT GamePad ******* **/
GamePadJoystick::GamePadJoystick(ControllerSocketNumber controllerSocket, USHORT xINum)
	: Joystick(controllerSocket) {

	xInputNum = xINum;
	ZeroMemory(&state, sizeof(XINPUT_STATE));

	aButton = ControlButtons::GAMEPAD_A;
	bButton = ControlButtons::GAMEPAD_B;
	xButton = ControlButtons::GAMEPAD_X;
	yButton = ControlButtons::GAMEPAD_Y;
	leftShoulderButton = ControlButtons::GAMEPAD_LEFT_SHOULDER;
	rightShoulderButton = ControlButtons::GAMEPAD_RIGHT_SHOULDER;
	startButton = ControlButtons::GAMEPAD_START;
	selectButton = ControlButtons::GAMEPAD_SELECT;
}

GamePadJoystick::~GamePadJoystick() {
}

SHORT GamePadJoystick::getXInputSlot() {
	return xInputNum;
}


void GamePadJoystick::update() {

	lastState = state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	/*DWORD dwResult = */XInputGetState(xInputNum, &state);

	//if (dwResult == ERROR_SUCCESS) {
}

/** DO NOTHING */
void GamePadJoystick::parseRawInput(PRAWINPUT pRawInput) {
}

Vector2 GamePadJoystick::getDirection() {

	SHORT lAxisX = state.Gamepad.sThumbLX;
	SHORT lAxisY = state.Gamepad.sThumbLY;
	float x = 0, y = 0;
	if (lAxisX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
		|| lAxisX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		x = lAxisX / 32768.0f;
	if (lAxisY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
		|| lAxisY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		y = lAxisY / 32768.0f;
	auto dir = Vector2(x, -y);
	//dir.Normalize();
	return dir;
}

bool GamePadJoystick::isLeftPressed() {
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0)
		return true;

	SHORT lAxisX = state.Gamepad.sThumbLX;

	if (lAxisX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		return true;

	return false;
}

bool GamePadJoystick::isRightPressed() {
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0)
		return true;

	SHORT lAxisX = state.Gamepad.sThumbLX;

	if (lAxisX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		return true;

	return false;
}

bool GamePadJoystick::isUpPressed() {
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0)
		return true;

	SHORT lAxisY = state.Gamepad.sThumbLY;

	if (lAxisY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		return true;

	return false;
}

bool GamePadJoystick::isDownPressed() {
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0)
		return true;

	SHORT lAxisY = state.Gamepad.sThumbLY;

	if (lAxisY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		return true;

	return false;
}

bool GamePadJoystick::yButtonPushed() {
	return (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0
		&& (lastState.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0;
}

bool GamePadJoystick::xButtonPushed() {
	return (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0
		&& (lastState.Gamepad.wButtons & XINPUT_GAMEPAD_X) == 0;
}

bool GamePadJoystick::aButtonPushed() {
	return (state.Gamepad.wButtons & aButton) != 0
		&& (lastState.Gamepad.wButtons & aButton) == 0;
}

bool GamePadJoystick::bButtonPushed() {
	return (state.Gamepad.wButtons & bButton) != 0
		&& (lastState.Gamepad.wButtons & bButton) == 0;
}

bool GamePadJoystick::lButtonPushed() {
	return (state.Gamepad.wButtons & leftShoulderButton) != 0
		&& (lastState.Gamepad.wButtons & leftShoulderButton) == 0;
}

bool GamePadJoystick::rButtonPushed() {

	//float amount = state.Gamepad.bRightTrigger / 255;
	return (state.Gamepad.wButtons & rightShoulderButton) != 0
		&& (lastState.Gamepad.wButtons & rightShoulderButton) == 0;
}

bool GamePadJoystick::startButtonPushed() {
	return (state.Gamepad.wButtons & startButton) != 0
		&& (lastState.Gamepad.wButtons & startButton) == 0;
}

bool GamePadJoystick::selectButtonPushed() {
	return (state.Gamepad.wButtons & selectButton) != 0
		&& (lastState.Gamepad.wButtons & selectButton) == 0;
}


bool GamePadJoystick::yButtonDown() {
	return (state.Gamepad.wButtons & yButton) != 0;
}

bool GamePadJoystick::xButtonDown() {
	return (state.Gamepad.wButtons & xButton) != 0;
}

bool GamePadJoystick::aButtonDown() {
	return (state.Gamepad.wButtons & aButton) != 0;
}

bool GamePadJoystick::bButtonDown() {
	return (state.Gamepad.wButtons & bButton) != 0;
}

bool GamePadJoystick::lButtonDown() {
	return (state.Gamepad.wButtons & leftShoulderButton) != 0;
}

bool GamePadJoystick::rButtonDown() {
	return (state.Gamepad.wButtons & rightShoulderButton) != 0;
}

bool GamePadJoystick::startButtonDown() {
	return (state.Gamepad.wButtons & startButton) != 0;
}

bool GamePadJoystick::selectButtonDown() {
	return (state.Gamepad.wButtons & selectButton) != 0;
}