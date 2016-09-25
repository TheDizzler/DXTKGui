#include "../pch.h"
#include "Dbt.h"
#include "GameEngine.h"


LPCTSTR wndClassName = L"DirectX ToolKit GUI Framework Test App";
HWND hwnd;

unique_ptr<GameEngine> gameEngine;
HDEVNOTIFY newAudio = NULL;

double countsPerSecond = 0.0;
__int64 counterStart = 0;

int frameCount = 0;
int fps = 0;
__int64 frameTimeOld = 0;
double frameTime;



int messageLoop();
void startTimer();
double getTime();
double getFrameTime();

bool initWindow(HINSTANCE hInstance, int showWnd);
LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);




void releaseResources() {

	if (Globals::FULL_SCREEN)
		ChangeDisplaySettings(NULL, 0);

	/*if (gameEngine)
		delete gameEngine;*/

	if (newAudio)
		UnregisterDeviceNotification(newAudio);

	CoUninitialize();
}


/** Main windows function.
	@nShowWnd how window should be displayed. Examples: SW_SHOWMAXIMIZED, SW_SHOW, SW_SHOWMINIMIZED. */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	gameEngine.reset(new GameEngine());

	if (!initWindow(hInstance, nShowCmd)) {
		MessageBox(0, TEXT("Window Initialization - Failed"), L"Error", MB_OK);
		releaseResources();
		return 0;
	}

	if (!gameEngine->initEngine(hwnd, hInstance)) {
		MessageBox(0, L"Game Engine Initialization Failed", L"Error", MB_OK);
		releaseResources();
		return 0;
	}

	// listen for new audio devices
	DEV_BROADCAST_DEVICEINTERFACE filter = {0};
	filter.dbcc_size = sizeof(filter);
	filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	filter.dbcc_classguid = KSCATEGORY_AUDIO;

	newAudio = RegisterDeviceNotification(hwnd, &filter, DEVICE_NOTIFY_WINDOW_HANDLE);

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
			frameCount++;
			if (getTime() > 1.0f) {
				fps = frameCount;
				frameCount = 0;
				startTimer();
			}

			double frameTime = getFrameTime();


			gameEngine->run(frameTime, fps);

		}

	}

	return msg.wParam;

}

int Globals::WINDOW_WIDTH = 800;
int Globals::WINDOW_HEIGHT = 600;
int Globals::vsync_enabled = 1;
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

	//if (Globals::FULL_SCREEN) {

	//	// Determine the resolution of the clients desktop screen.
	//	windowX = GetSystemMetrics(SM_CXSCREEN);
	//	windowY = GetSystemMetrics(SM_CYSCREEN);
	//	Globals::WINDOW_WIDTH = windowX;
	//	Globals::WINDOW_HEIGHT = windowY;

	//	DEVMODE dmScreenSettings;
	//	memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
	//	dmScreenSettings.dmSize = sizeof(dmScreenSettings);
	//	dmScreenSettings.dmPelsWidth = (unsigned long) Globals::WINDOW_WIDTH;
	//	dmScreenSettings.dmPelsHeight = (unsigned long) Globals::WINDOW_HEIGHT;
	//	dmScreenSettings.dmBitsPerPel = 32;
	//	dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	//	ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	//	
	//	// Set the position of the window to the top left corner.

	//} else {
	// Make client dimensions WINDOW_WIDTHxWINDOW_HEIGHT
	// (as opposed to window dimensions)
	RECT winRect = {0, 0, Globals::WINDOW_WIDTH, Globals::WINDOW_HEIGHT};
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);
	windowX = winRect.right - winRect.left;
	windowY = winRect.bottom - winRect.top;
		// If windowed then set it to global default resolution
		// and place the window in the middle of the screen.
	posX = (GetSystemMetrics(SM_CXSCREEN) - Globals::WINDOW_WIDTH) / 2;
	posY = (GetSystemMetrics(SM_CYSCREEN) - Globals::WINDOW_HEIGHT) / 2;

//}

	DWORD windowStyle = (WS_OVERLAPPEDWINDOW&~WS_THICKFRAME | /*WS_CAPTION | WS_SYSMENU | */WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
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

	ShowWindow(hwnd, showWnd);
	UpdateWindow(hwnd);


	return true;

}



LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	LPBYTE lpb;
	UINT dwSize;

	switch (msg) {
		case WM_ACTIVATEAPP:
			Mouse::ProcessMessage(msg, wParam, lParam);
			Keyboard::ProcessMessage(msg, wParam, lParam);
			return 0;

		case WM_CREATE:
			return 0;
			/*case WM_KEYDOWN:
				if (wParam == VK_ESCAPE) {
				if (MessageBox(0, L"Are you sure you want to exit?",
				L"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)
				DestroyWindow(hwnd);
				}
				return 0;*/
		case WM_INPUT:
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
	
	/*	case WM_INPUT:

			GetRawInputData((HRAWINPUT) lParam, RID_INPUT, NULL,
				&dwSize, sizeof(RAWINPUTHEADER));
			lpb = new BYTE[dwSize];
			if (lpb == NULL)
				return 0;

			if (GetRawInputData((HRAWINPUT) lParam, RID_INPUT, lpb,
				&dwSize, sizeof(RAWINPUTHEADER)) != dwSize) {
				delete[] lpb;
				return 0;
			}

			gameEngine->setRawInput((RAWINPUT*) lpb);

			return 0;*/

		case WM_DEVICECHANGE:
			if (wParam == DBT_DEVICEARRIVAL) {
				auto pDev = reinterpret_cast<PDEV_BROADCAST_HDR>(lParam);
				if (pDev) {
					if (pDev->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
						auto pInter = reinterpret_cast<
							const PDEV_BROADCAST_DEVICEINTERFACE>(pDev);
						if (pInter->dbcc_classguid == KSCATEGORY_AUDIO) {
							if (gameEngine)
								gameEngine->onAudioDeviceChange();
						}
					}
				}
			}
			return 0;

		case WM_KILLFOCUS:
			gameEngine->suspend();
			OutputDebugString(L"Lost Focus\n");
			return 0;

		case WM_ACTIVATE:
			OutputDebugString(L"Got Focus\n");
			gameEngine->resume();
			return 0;

		case WM_DESTROY:	// top right x button pressed
			PostQuitMessage(0);
			return 0;

}

return DefWindowProc(hwnd, msg, wParam, lParam);
}



void startTimer() {

	LARGE_INTEGER frequencyCount;
	QueryPerformanceFrequency(&frequencyCount);

	countsPerSecond = double(frequencyCount.QuadPart);

	QueryPerformanceCounter(&frequencyCount);
	counterStart = frequencyCount.QuadPart;

}


double getTime() {

	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return double(currentTime.QuadPart - counterStart) / countsPerSecond;
}


double getFrameTime() {

	LARGE_INTEGER currentTime;
	__int64 tickCount;
	QueryPerformanceCounter(&currentTime);

	tickCount = currentTime.QuadPart - frameTimeOld;
	frameTimeOld = currentTime.QuadPart;

	if (tickCount < 0.0f)
		tickCount = 0.0f;

	return float(tickCount) / countsPerSecond;
}