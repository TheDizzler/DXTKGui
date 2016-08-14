#pragma once

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

	void run(double time, int fps);
	virtual void render(double time);

	void pause();
	void exit();
private:

	std::unique_ptr<GameManager> game;

	void update(double time);

	bool initStage();



	HWND hwnd;
};

