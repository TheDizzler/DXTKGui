#pragma once
#define WIN32_LEAN_AND_MEAN

#include <WinSDKVer.h>		// these are necessary for XAudio2.8
#define _WIN32_WINNT 0x0602	// ^
#include <SDKDDKVer.h>		// ^

#include <Windows.h>
#include <dbt.h>

#include <guiddef.h>

#include <Mmsystem.h>
#include <Setupapi.h>


#include "GameEngine.h"





LPCTSTR wndClassName = L"DirectXTK GUI Demo";
HWND hwnd;

unique_ptr<GameEngine> gameEngine;
HDEVNOTIFY newInterface = NULL;

GUID guidHid;

double countsPerSecond = 0.0;
__int64 counterStart = 0;


/** If using joysticks this is required. */
int registerControllers();
/** Creates a list of all attached USB devices. */
int getInputDeviceInfo(bool writeToFile, wstring filename = L"USB Devices.txt");
int messageLoop();
void startTimer();
double getSecondsSinceStart();
double getFrameTime();

bool initWindow(HINSTANCE hInstance, int showWnd);
LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);





void releaseResources() {

	if (Globals::FULL_SCREEN)
		ChangeDisplaySettings(NULL, 0);

	if (newInterface)
		UnregisterDeviceNotification(newInterface);

	CoUninitialize();
}


/** Main windows function.
@nShowWnd how window should be displayed. Examples: SW_SHOWMAXIMIZED, SW_SHOW, SW_SHOWMINIMIZED. */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	guidHid = GUID();
	guidHid.Data1 = 0x4D1E55B2L;
	guidHid.Data2 = 0xF16F;
	guidHid.Data3 = 0x11CF;
	guidHid.Data4[0] = 0x88;
	guidHid.Data4[1] = 0xCB;
	guidHid.Data4[2] = 0x00;
	guidHid.Data4[3] = 0x11;
	guidHid.Data4[4] = 0x11;
	guidHid.Data4[5] = 0x00;
	guidHid.Data4[6] = 0x00;
	guidHid.Data4[7] = 0x30;

	gameEngine.reset(new GameEngine());

	if (!initWindow(hInstance, nShowCmd)) {
		MessageBox(0, L"Window Initialization - Failed", L"Error", MB_OK);
		releaseResources();
		return 0;
	}

	if (!gameEngine->initEngine(hwnd, hInstance)) {
		GameEngine::errorMessage(L"Game Engine Initialization Failed", L"Error", true);
		releaseResources();
		return 0;
	}

	// listen for new devices
	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
	ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
	NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	newInterface = RegisterDeviceNotification(hwnd, &NotificationFilter,
		DEVICE_NOTIFY_ALL_INTERFACE_CLASSES);


	LARGE_INTEGER frequencyCount;
	QueryPerformanceFrequency(&frequencyCount);
	countsPerSecond = double(frequencyCount.QuadPart);
	startTimer();

	messageLoop(); /* Main program loop */
	releaseResources();


	return 0;
}


int messageLoop() {

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (true) {

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { // if it's a windows message
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {	// game code

			//double frameTime = getFrameTime();

			gameEngine->run(getFrameTime());

		}

	}

	return msg.wParam;

}

int Globals::WINDOW_WIDTH = 800;
int Globals::WINDOW_HEIGHT = 600;
int Globals::vsync_enabled = 0;
bool Globals::FULL_SCREEN = false;

bool initWindow(HINSTANCE hInstance, int showWnd) {


	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;			// mo' styles http://msdn.microsoft.com/en-us/library/ms633574(VS.85).aspx#class_styles
	wc.lpfnWndProc = wndProc;
	wc.cbClsExtra = NULL;						// extra bytes after wc structure
	wc.cbWndExtra = NULL;						// extra bytes after windows instance
	wc.hInstance = hInstance;					// instance of current app
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);		// title bar icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);	// mo' mouse cursors http://msdn.microsoft.com/en-us/library/ms648391(VS.85).aspx
	wc.hbrBackground = 0/*(HBRUSH) (COLOR_WINDOW)*/;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = wndClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);	// taskbar icon

	if (!RegisterClassEx(&wc)) {

		MessageBox(NULL, L"Error registering class", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	int posX, posY;
	posX = posY = 0;
	int windowX, windowY;
	windowX = windowY = 0;

	RECT winRect = {0, 0, Globals::WINDOW_WIDTH, Globals::WINDOW_HEIGHT};
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);
	windowX = winRect.right - winRect.left;
	windowY = winRect.bottom - winRect.top;
	// If windowed then set it to global default resolution
	// and place the window in the middle of the screen.
	posX = (GetSystemMetrics(SM_CXSCREEN) - Globals::WINDOW_WIDTH) / 2;
	posY = (GetSystemMetrics(SM_CYSCREEN) - Globals::WINDOW_HEIGHT) / 2;


	DWORD windowStyle = (WS_OVERLAPPEDWINDOW&~WS_THICKFRAME | /*WS_CAPTION | WS_SYSMENU | */
		WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
	hwnd = CreateWindowEx(
		NULL,					// extended style, check em out here https://msdn.microsoft.com/en-us/library/61fe4bte(v=vs.140).aspx
		wndClassName,
		wndClassName,			// title bar text
		windowStyle,			// window style, mo' styles http://msdn.microsoft.com/zh-cn/library/czada357.aspx
		posX, posY,				// top left of window
		windowX, windowY,
		NULL,					// handle to parent window
		NULL,					// handle to a menu
		hInstance,				// instance of current program
		NULL);					// used for MDI client window

	if (!hwnd) {

		MessageBox(NULL, L"Error creating window", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// Makes window transparent. Interesting! But useless?
	/*SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_LAYERED);
	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 100, LWA_ALPHA);*/

	ShowWindow(hwnd, showWnd);
	UpdateWindow(hwnd);


	return true;

}


LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {


	switch (msg) {
		case WM_ACTIVATEAPP:
			Mouse::ProcessMessage(msg, wParam, lParam);
			Keyboard::ProcessMessage(msg, wParam, lParam);
			return 0;

		case WM_CREATE:

			//getInputDeviceInfo(true, L"Devices.txt");

			RAWINPUTDEVICE rid[1];
			rid[0].usUsagePage = 1;
			rid[0].usUsage = 4; // joystick
			rid[0].dwFlags = 0;
			rid[0].hwndTarget = hwnd;

			if (!RegisterRawInputDevices(rid, 1, sizeof(RAWINPUTDEVICE))) {
				OutputDebugString(L"On no!! Can't find Raw Input Device??");
				return -1;
			}


			registerControllers();

			return 0;

		case WM_INPUT:
		{

			/** If not using joysticks this can be removed  */
			PRAWINPUT pRawInput;
			UINT bufferSize;
			HANDLE hHeap;

			GetRawInputData((HRAWINPUT) lParam, RID_INPUT, NULL, &bufferSize, sizeof(RAWINPUTHEADER));
			hHeap = GetProcessHeap();
			pRawInput = (PRAWINPUT) HeapAlloc(hHeap, 0, bufferSize);
			if (!pRawInput)
				return 0;

			GetRawInputData((HRAWINPUT) lParam, RID_INPUT,
				pRawInput, &bufferSize, sizeof(RAWINPUTHEADER));
			if (pRawInput->header.dwType == RIM_TYPEHID)
				gameEngine->parseRawInput(pRawInput);

			HeapFree(hHeap, 0, pRawInput);
			/** Joystick end */
		}
		return 0;
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MOUSEWHEEL:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_MOUSEHOVER:
			Mouse::ProcessMessage(msg, wParam, lParam);
			return 0;

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			Keyboard::ProcessMessage(msg, wParam, lParam);
			return 0;


		case WM_DEVICECHANGE:
		{

			PDEV_BROADCAST_HDR pDev = reinterpret_cast<PDEV_BROADCAST_HDR>(lParam);

			if (!pDev)
				break;


			if (pDev->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
				PDEV_BROADCAST_DEVICEINTERFACE deviceInterface
					= reinterpret_cast<const PDEV_BROADCAST_DEVICEINTERFACE>(pDev);
				switch (wParam) {

					case DBT_DEVICEARRIVAL:
					{
						if (deviceInterface->dbcc_classguid == KSCATEGORY_AUDIO) {
							OutputDebugString(L"Audio interface added!\n");
							if (gameEngine)
								gameEngine->onAudioDeviceChange();
							return 0;
						}


						if (deviceInterface->dbcc_classguid == guidHid) {
							OutputDebugString(L"Found a controller!\n");
							registerControllers();
							return 0;
						}
					}
					break;

					case DBT_DEVICEREMOVECOMPLETE:

						if (deviceInterface->dbcc_classguid == KSCATEGORY_AUDIO) {

							OutputDebugString(L"Audio interface removed!\n");
							return 0;
						}
						if (deviceInterface->dbcc_classguid == guidHid) {
							// remove joysticks
							registerControllers();
							return 0;
						}
						break;

				}

			}
		}
		return 0;

		case WM_NCLBUTTONDOWN:
			gameEngine->suspend();
			break;
		case WM_KILLFOCUS:
			//OutputDebugString(L"Lost Focus\n");
			gameEngine->suspend();
			return 0;

		case WM_ACTIVATE:
			//OutputDebugString(L"Got Focus\n");
			gameEngine->resume();
			return 0;

		case WM_DESTROY:	// top right x button pressed or DestroyWindow(HWND) called
			PostQuitMessage(0);
			return 0;

	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


/** If using joysticks this is required. */
int registerControllers() {

	UINT nDevices;
	PRAWINPUTDEVICELIST pRawInputDeviceList;
	if (GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0) {
		OutputDebugString(L"On no!! Can't find Raw Input Device??");
		return -1;
	}

	if ((pRawInputDeviceList = (PRAWINPUTDEVICELIST) malloc(sizeof(RAWINPUTDEVICELIST) * nDevices)) == NULL) {
		OutputDebugString(L"Initialization failed...");
		return -1;
	}

	int nNoOfDevices = 0;
	if ((nNoOfDevices = GetRawInputDeviceList(
		pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST))) == ((UINT) -1)) {
		OutputDebugString(L"No devices found");
		return -1;
	}

	RID_DEVICE_INFO rdi;
	rdi.cbSize = sizeof(RID_DEVICE_INFO);

	vector<ControllerDevice> controllerDevices;
	for (int i = 0; i < nNoOfDevices; i++) {
		UINT size = 256;
		TCHAR tBuffer[256] = {0};

		if (GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice,
			RIDI_DEVICENAME, tBuffer, &size) < 0) {
				// Error in reading device name
			continue;
		}

		UINT cbSize = rdi.cbSize;
		if (GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice,
			RIDI_DEVICEINFO, &rdi, &cbSize) < 0) {
				// Error in reading information
			wostringstream wss;
			wss << L"Device Name: " << tBuffer << "\n";
			wss << "Error reading information" << endl;
			OutputDebugString(wss.str().c_str());
			continue;
		}


		if (rdi.dwType == RIM_TYPEHID) {
			if (rdi.hid.usUsagePage == 1) {
				if (rdi.hid.usUsage == 4) {
					ControllerDevice cd;
					cd.handle = pRawInputDeviceList[i].hDevice;
					cd.isRawInput = true;
					controllerDevices.push_back(cd);
				} else if (rdi.hid.usUsage == 5) {
					ControllerDevice cd;
					cd.handle = pRawInputDeviceList[i].hDevice;
					cd.isRawInput = false;
					controllerDevices.push_back(cd);
				}

			}
		}
	}

	gameEngine->addJoysticks(controllerDevices);


	free(pRawInputDeviceList);
	return 1;
}

#include <fstream>
/* Finds and list all HID devices. For device finding, debugging, etc. */
int getInputDeviceInfo(bool writeToFile, wstring filename) {



	UINT nDevices;
	PRAWINPUTDEVICELIST pRawInputDeviceList;
	if (GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0) {
		OutputDebugString(L"On no!! Can't find Raw Input Device??");
		return -1;
	}

	if ((pRawInputDeviceList = (PRAWINPUTDEVICELIST) malloc(sizeof(RAWINPUTDEVICELIST) * nDevices)) == NULL) {
		OutputDebugString(L"Initialization failed...");
		return -1;
	}

	int nNoOfDevices = 0;
	if ((nNoOfDevices = GetRawInputDeviceList(pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST))) == ((UINT) -1)) {
		OutputDebugString(L"No devices found");
		return -1;
	}

	RID_DEVICE_INFO rdi;
	rdi.cbSize = sizeof(RID_DEVICE_INFO);
	wostringstream wss;
	wss << "Number of devices: " << nNoOfDevices << endl << "***********************" << endl;
	for (int i = 0; i < nNoOfDevices; i++) {
		UINT size = 256;
		TCHAR tBuffer[256] = {0};

		if (GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, RIDI_DEVICENAME, tBuffer, &size) < 0) {
			// Error in reading device name
			wss << "Error reading device name" << endl;
			//continue;
		}


		wss << L"Device Name: " << tBuffer << "\n";
		UINT cbSize = rdi.cbSize;
		if (GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, RIDI_DEVICEINFO, &rdi, &cbSize) < 0) {
			// Error in reading information
			wss << "Error reading information" << endl;
		}

		if (rdi.dwType == RIM_TYPEMOUSE) {
			wss << "ID for Mouse:" << rdi.mouse.dwId << endl;
			wss << "Number of Buttons:" << rdi.mouse.dwNumberOfButtons << endl;
			wss << "Sample rate(Number of data points):" << rdi.mouse.dwSampleRate << endl;
		} else if (rdi.dwType == RIM_TYPEKEYBOARD) {
			wss << "Keyboard Mode:" << rdi.keyboard.dwKeyboardMode << endl;
			wss << "Number of function keys:" << rdi.keyboard.dwNumberOfFunctionKeys << endl;
			wss << "Number of indicators:" << rdi.keyboard.dwNumberOfIndicators << endl;
			wss << "Number of keys total: " << rdi.keyboard.dwNumberOfKeysTotal << endl;
			wss << "Type of the keyboard: " << rdi.keyboard.dwType << endl;
			wss << "Subtype of the keyboard: " << rdi.keyboard.dwSubType << endl;
		} else if (rdi.dwType == RIM_TYPEHID) {
			wss << "Vendor Id:" << rdi.hid.dwVendorId << endl;
			wss << "Product Id:" << rdi.hid.dwProductId << endl;
			wss << "Version No:" << rdi.hid.dwVersionNumber << endl;
			wss << "Usage for the device: " << rdi.hid.usUsage << endl;
			wss << "Usage Page for the device: " << rdi.hid.usUsagePage << endl;


		}
		wss << "***********************" << endl << endl;
	}

	if (writeToFile) {
		wofstream file;
		file = wofstream(filename);
		file << wss.str();
		file.close();

	} else
		OutputDebugString(wss.str().c_str());

	free(pRawInputDeviceList);
	return 0;
}

__int64 lastFrameStartTime = 0;
void startTimer() {

	LARGE_INTEGER frequencyCount;
	/*QueryPerformanceFrequency(&frequencyCount);
	countsPerSecond = double(frequencyCount.QuadPart);*/

	QueryPerformanceCounter(&frequencyCount);
	counterStart = frequencyCount.QuadPart;
	lastFrameStartTime = frequencyCount.QuadPart;
}


double getSecondsSinceStart() {

	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return double(currentTime.QuadPart - counterStart) / countsPerSecond;
}


double getFrameTime() {

	LARGE_INTEGER currentTime;
	__int64 tickCount;
	QueryPerformanceCounter(&currentTime);

	tickCount = currentTime.QuadPart - lastFrameStartTime;
	lastFrameStartTime = currentTime.QuadPart;

	//if (tickCount < 0.0f) // why would this ever be negative?
	//	tickCount = 0.0f;

	return double(tickCount) / countsPerSecond;
}