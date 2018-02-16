#pragma once

#include <d3d11_1.h>
#include <SimpleMath.h>
#pragma comment(lib, "XInput.lib")
#include <Xinput.h>


#define MAX_BUTTONS		128

using namespace DirectX::SimpleMath;

enum PlayerSlotNumber {
	NONE = -1, SLOT_1, SLOT_2, SLOT_3, SLOT_4, SLOT_5, SLOT_6, SLOT_7, SLOT_8
};

/* Virtual Sockets for controllers. */
enum ControllerSocketNumber {
	CONTROLLER_SOCKET_ERROR = -1, SOCKET_1, SOCKET_2, SOCKET_3,
	SOCKET_4, SOCKET_5, SOCKET_6, SOCKET_7, SOCKET_8
};

enum ControlButtons {
	A = 0, B, X, Y, L, R, SELECT, START,
	GAMEPAD_A = XINPUT_GAMEPAD_A, GAMEPAD_B = XINPUT_GAMEPAD_B,
	GAMEPAD_X = XINPUT_GAMEPAD_X, GAMEPAD_Y = XINPUT_GAMEPAD_Y,
	GAMEPAD_LEFT_SHOULDER = XINPUT_GAMEPAD_LEFT_SHOULDER,
	GAMEPAD_RIGHT_SHOULDER = XINPUT_GAMEPAD_RIGHT_SHOULDER,
	GAMEPAD_START = XINPUT_GAMEPAD_START, GAMEPAD_SELECT = XINPUT_GAMEPAD_BACK
};


class Joystick {
public:

	Joystick(ControllerSocketNumber controllerSocket);
	virtual ~Joystick();

	virtual SHORT getXInputSlot() = 0;

	HANDLE getHandle();
	void registerNewHandle(HANDLE handle);

	virtual void parseRawInput(PRAWINPUT pRawInput) = 0;

	void setControllerSocketNumber(ControllerSocketNumber socketNumber);
	ControllerSocketNumber getControllerSockerNumber();

	void setPlayerSlotNumber(PlayerSlotNumber slotNum);
	PlayerSlotNumber getPlayerSlotNumber();

	/** Returns normalized direction magnitude (between -1 and 1). **/
	virtual Vector2 getDirection() = 0;
	virtual bool isLeftPressed() = 0;
	virtual bool isRightPressed() = 0;
	virtual bool isUpPressed() = 0;
	virtual bool isDownPressed() = 0;

	/** _ButtonPushed() is for one time check only (non-repeating). */
	virtual bool yButtonPushed() = 0;
	virtual bool xButtonPushed() = 0;
	virtual bool aButtonPushed() = 0;
	virtual bool bButtonPushed() = 0;
	virtual bool lButtonPushed() = 0;
	virtual bool rButtonPushed() = 0;
	virtual bool startButtonPushed() = 0;
	virtual bool selectButtonPushed() = 0;

	virtual bool yButtonDown() = 0;
	virtual bool xButtonDown() = 0;
	virtual bool aButtonDown() = 0;
	virtual bool bButtonDown() = 0;
	virtual bool lButtonDown() = 0;
	virtual bool rButtonDown() = 0;
	virtual bool startButtonDown() = 0;
	virtual bool selectButtonDown() = 0;

	ControlButtons aButton = ControlButtons::A;
	ControlButtons bButton = ControlButtons::B;
	ControlButtons xButton = ControlButtons::X;
	ControlButtons yButton = ControlButtons::Y;
	ControlButtons leftShoulderButton = ControlButtons::L;
	ControlButtons rightShoulderButton = ControlButtons::R;
	ControlButtons startButton = ControlButtons::START;
	ControlButtons selectButton = ControlButtons::SELECT;

protected:
	HANDLE handle = NULL;
	PlayerSlotNumber playerSlotNumber = PlayerSlotNumber::NONE;
	/* The virtual controller socket this joystick is plugged in to. */
	ControllerSocketNumber socket = CONTROLLER_SOCKET_ERROR;
};

/* A raw input joystick class with huge thanks to Alexander Bocken
https://www.codeproject.com/Articles/185522/Using-the-Raw-Input-API-to-Process-Joystick-Input */

class RawInputJoystick : public Joystick {
public:
	RawInputJoystick(ControllerSocketNumber controllerSocket);
	virtual ~RawInputJoystick();

	virtual SHORT getXInputSlot() override;

	virtual void parseRawInput(PRAWINPUT pRawInput) override;

	/** Returns normalized direction magnitude (between -1 and 1). **/
	virtual Vector2 getDirection() override;
	virtual bool isLeftPressed() override;
	virtual bool isRightPressed() override;
	virtual bool isUpPressed() override;
	virtual bool isDownPressed() override;

	virtual bool yButtonPushed() override;
	virtual bool xButtonPushed() override;
	virtual bool aButtonPushed() override;
	virtual bool bButtonPushed() override;
	virtual bool lButtonPushed() override;
	virtual bool rButtonPushed() override;
	virtual bool startButtonPushed() override;
	virtual bool selectButtonPushed() override;

	virtual bool yButtonDown() override;
	virtual bool xButtonDown() override;
	virtual bool aButtonDown() override;
	virtual bool bButtonDown() override;
	virtual bool lButtonDown() override;
	virtual bool rButtonDown() override;
	virtual bool startButtonDown() override;
	virtual bool selectButtonDown() override;


private:
	bool bButtonStates[MAX_BUTTONS];
	BOOL lastButtonStates[MAX_BUTTONS];
	LONG lAxisX = 0;
	LONG lAxisY = 0;
	LONG lAxisZ = 0;
	LONG lAxisRz = 0;
	LONG lHat = 0;
	INT  g_NumberOfButtons;

	USHORT DEAD_ZONE = 10;

};



class GamePadJoystick : public Joystick {
public:
	GamePadJoystick(ControllerSocketNumber controllerSocket, USHORT xInputNum);
	virtual ~GamePadJoystick();

	virtual SHORT getXInputSlot() override;

	void update();
	/** DO NOTHING */
	virtual void parseRawInput(PRAWINPUT pRawInput) override;

	/** Returns normalized direction magnitude (between -1 and 1). **/
	virtual Vector2 getDirection() override;

	virtual bool isLeftPressed() override;
	virtual bool isRightPressed() override;
	virtual bool isUpPressed() override;
	virtual bool isDownPressed() override;


	virtual bool yButtonPushed() override;
	virtual bool xButtonPushed() override;
	virtual bool aButtonPushed() override;
	virtual bool bButtonPushed() override;
	virtual bool lButtonPushed() override;
	virtual bool rButtonPushed() override;
	virtual bool startButtonPushed() override;
	virtual bool selectButtonPushed() override;


	virtual bool yButtonDown() override;
	virtual bool xButtonDown() override;
	virtual bool aButtonDown() override;
	virtual bool bButtonDown() override;
	virtual bool lButtonDown() override;
	virtual bool rButtonDown() override;
	virtual bool startButtonDown() override;
	virtual bool selectButtonDown() override;

private:
	USHORT xInputNum = 0;
	XINPUT_STATE state;
	XINPUT_STATE lastState;
};