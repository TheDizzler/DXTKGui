#pragma once


#define ErrorDialog(message, title) \
	errorDialog->setTitle(title); \
	errorDialog->setText(message); \
	errorDialog->open(); \
	showDialog = errorDialog.get();

#define WarningDialog(message, title) \
	warningDialog->setTitle(title); \
	warningDialog->setText(message); \
	warningDialog->open(); \
	showDialog = warningDialog.get();

#include "GraphicsEngine.h"
#include "Input.h"

#include "../Managers/GameManager.h"


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

	/* Critical error dialog. Exits game when dismissed. */
	unique_ptr<Dialog> errorDialog;
	/* Minor error dialog. Choice between exit game and continue. */
	unique_ptr<Dialog> warningDialog;

	Dialog* showDialog = NULL;
private:

	unique_ptr<AudioEngine> audioEngine;
	unique_ptr<GameManager> game;

	void update(double time);

	bool initStage();



	HWND hwnd;
	bool retryAudio;
};

