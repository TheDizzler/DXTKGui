#include "GameEngine.h"


GameEngine::GameEngine() {
}


GameEngine::~GameEngine() {
}


bool GameEngine::initEngine(HWND hw, HINSTANCE hInstance) {

	hwnd = hw;

	if (!initD3D(hwnd)) {
		MessageBox(0, L"Direct3D Initialization Failed", L"Error", MB_OK);
		return false;
	}

	if (!initRawInput(hwnd)) {
		MessageBox(0, L"Raw Input Init failed", L"Error", MB_OK);
		return false;
	}

	if (!initStage()) {
		MessageBox(0, L"Stage Initialization Failed", L"Error", MB_OK);
		return false;
	}


	return true;
}

bool GameEngine::initStage() {


	game.reset(new GameManager(this));
	if (!game->initializeGame(device, mouse.get())) {
		MessageBox(0, L"Game Manager failed to load.", L"Critical Failure", MB_OK);
		return false;
	}


	return true;
}



void GameEngine::run(double deltaTime, int fps) {

	update(deltaTime);
	render(deltaTime);

}


void GameEngine::update(double deltaTime) {

	game->update(deltaTime, keys.get(), mouse.get());
}



void GameEngine::render(double deltaTime) {

	deviceContext->ClearRenderTargetView(renderTargetView.Get(), Colors::PeachPuff);

	batch->Begin(SpriteSortMode_Deferred);
	{
		game->draw(batch.get());
		mouse->draw(batch.get());
	}
	batch->End();


	swapChain->Present(0, 0);
}

void GameEngine::pause() {
	if (game != NULL)
		game->pause();
}

void GameEngine::exit() {

	/*if (MessageBox(0, L"Are you sure you want to exit?",
		L"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)*/
	DestroyWindow(hwnd);
}

