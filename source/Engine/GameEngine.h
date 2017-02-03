#pragma once

#include <sstream>
#include <comdef.h>


#include <WinSDKVer.h>		// these are necessary for XAudio2.8
#define _WIN32_WINNT 0x0602	// ^
#include <SDKDDKVer.h>		// ^

#include "Audio.h"



#include "GraphicsEngine.h"
#include "Input.h"

#include "../Managers/GameManager.h"

extern unique_ptr<GUIFactory> guiFactory;

/** The engine to connect higher level game code to the low level
	graphic and OS software. This class should be reusable for any 2D game,
	thus should not contain any game logic. */
class GameEngine : public GraphicsEngine, public Input {
public:

	GameEngine();
	~GameEngine();

	bool initEngine(HWND hwnd, HINSTANCE hInstance);
	void onAudioDeviceChange();

	void run(double time, int fps);
	virtual void render(double time);

	void suspend();
	void resume();
	void exit();


	static inline bool reportError(HRESULT hr,
		wstring failMessage = L"This is SRS Error",
		wstring failTitle = L"Fatal Error", bool dontUseGUI = false) {

		if (FAILED(hr)) {

			_com_error err(hr);
			wostringstream wss;
			wss << failMessage;
			wss << "\nHRESULT: " << err.ErrorMessage();
			if (GUIFactory::initialized && !dontUseGUI)
				GameEngine::showWarningDialog(wss.str(), failTitle);
			else if (!Globals::FULL_SCREEN)
				MessageBox(NULL, wss.str().c_str(), failTitle.c_str(), MB_OK | MB_ICONERROR);
			else
				OutputDebugString(wss.str().c_str());
			return true;
		}

		return false;
	}

	static void showErrorDialog(wstring message, wstring title) {
		errorDialog->open();
		errorDialog->setTitle(title);
		errorDialog->setText(message);
		showDialog = errorDialog.get();
	}

	static void showWarningDialog(wstring message, wstring title) {
		warningDialog->open();
		warningDialog->setTitle(title);
		warningDialog->setText(message);
		warningDialog->setTextTint(Color(1, 0, 0, 1));
		showDialog = warningDialog.get();
	}

private:

	unique_ptr<AudioEngine> audioEngine;
	unique_ptr<GameManager> game;

	void update(double time);

	bool initGFXAssets();
	bool initStage();



	HWND hwnd;
	bool retryAudio;

	unique_ptr<pugi::xml_document> docAssMan;


	/* Critical error dialog. Exits game when dismissed. */
	static unique_ptr<Dialog> errorDialog;
	/* Minor error dialog. Choice between exit game and continue. */
	static unique_ptr<Dialog> warningDialog;
	static Dialog* showDialog;
};

