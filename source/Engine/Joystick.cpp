#include "Joystick.h"



double hz = 0.0;
__int64 startTime = 0;
__int64 lastFrameStart = 0;
int frames = 0;
void start() {

	LARGE_INTEGER frequencyCount;

	QueryPerformanceCounter(&frequencyCount);
	startTime = frequencyCount.QuadPart;
	lastFrameStart = frequencyCount.QuadPart;
}

double getTimeSinceStart() {

	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return double(currentTime.QuadPart - startTime) / hz;
}



Joystick::Joystick(size_t controllerSlot) {

	socket = controllerSlot;

	ZeroMemory(bButtonStates, sizeof(bButtonStates));
}

Joystick::~Joystick() {
}


void Joystick::registerNewHandle(HANDLE hndl) {
	handle = hndl;

	LARGE_INTEGER frequencyCount;
	QueryPerformanceFrequency(&frequencyCount);
	hz = double(frequencyCount.QuadPart);
	start();
}

HANDLE Joystick::getHandle() {
	return handle;
}

//#include "../Managers/GameManager.h"
void Joystick::parseRawInput(PRAWINPUT pRawInput) {

	PHIDP_PREPARSED_DATA pPreparsedData = NULL;
	HIDP_CAPS            Caps;
	PHIDP_BUTTON_CAPS    pButtonCaps = NULL;
	PHIDP_VALUE_CAPS     pValueCaps = NULL;
	USHORT               capsLength;
	UINT                 bufferSize;
	HANDLE               hHeap;
	USAGE                usage[MAX_BUTTONS];
	ULONG                i, usageLength, value;

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

	//wostringstream wss;
	//wss << "Buttons: " << g_NumberOfButtons << "\n";
	//wss << "bButtonStates[0]: " << bButtonStates[0] << "\n";
	//wss << "bButtonStates[1]: " << bButtonStates[1] << "\n";
	//wss << "Controller: " << socket << endl;
	//wss << lAxisX << " " << lAxisY << "\n";
	//OutputDebugString(wss.str().c_str());



	HeapFree(hHeap, 0, pPreparsedData);
	HeapFree(hHeap, 0, pButtonCaps);
	HeapFree(hHeap, 0, pValueCaps);
	//pPreparsedData = NULL;
	//pButtonCaps = NULL;
	//pValueCaps = NULL;

	/*double fpsUpdateTime = getTimeSinceStart();
	++frames;
	if (fpsUpdateTime > 1.0f) {
		
		wostringstream wss;
		wss << "Joystick: " << endl;
		wss << "frameCount: " << frames << " fpsUpdateTime: " << fpsUpdateTime << endl;
		wss << "fps: " << frames / fpsUpdateTime;
		guiOverlay->fps2Label->setText(wss);
		frames = 0;
		start();
	}*/
}
