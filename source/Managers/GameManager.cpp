#include "GameManager.h"

GameManager::GameManager(GameEngine* gmngn) {

	gameEngine = gmngn;
}

GameManager::~GameManager() {
}


bool GameManager::initializeGame(ID3D11Device* dvc, MouseController* ms) {

	device = dvc;
	mouse = ms;


	menuScreen.reset(new MenuManager());
	menuScreen->setGameManager(this);
	if (!menuScreen->initialize(device, mouse))
		return false;


	currentScreen = menuScreen.get();


	return true;
}



void GameManager::update(double deltaTime, KeyboardController* keys,
	MouseController* mouse) {


	currentScreen->update(deltaTime, keys, mouse);

}




void GameManager::draw(SpriteBatch * batch) {

	currentScreen->draw(batch);

}

void GameManager::loadLevel(const wchar_t* file) {

	lastScreen = currentScreen;



}

void GameManager::loadMainMenu() {

	/*if (lastScreen)
		delete lastScreen;*/
	lastScreen = currentScreen;
	currentScreen = menuScreen.get();

	/*if (!currentScreen->initialize(device, mouse)) {
		MessageBox(NULL, L"Failed to load main menu", L"ERROR", MB_OK);
		exit();
	}
	currentScreen->setGameManager(this);*/
}


#include "../Engine/GameEngine.h"

void GameManager::pause() {

	currentScreen->pause();
}

void GameManager::exit() {


	//dialogs.push_back(exitDialog.get());

	gameEngine->exit();
}

vector<wstring> GameManager::getAdapterListDescriptions() {
	return gameEngine->getAdapterListDescriptions();
}

vector<ComPtr<IDXGIAdapter>> GameManager::getAdapterList() {
	return gameEngine->getAdapterList();
}

vector<wstring> GameManager::getDisplayModeListDescriptions(size_t adapterIndex) {
	return gameEngine->getDisplayModeListDescriptions(adapterIndex);
}

vector<DXGI_MODE_DESC> GameManager::getDisplayModeList(size_t adapterIndex) {
	return gameEngine->getDisplayModeList(adapterIndex);
}


size_t GameManager::getSelectedAdapterIndex() {
	return gameEngine->getSelectedAdapterIndex();
}

size_t GameManager::getSelectedDisplayMode() {
	return gameEngine->getSelectedDisplayModeIndex();
}






