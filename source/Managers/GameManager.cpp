#include "GameManager.h"
#include "../Engine/GameEngine.h"

unique_ptr<GUIOverlay> guiOverlay;


GameManager::GameManager(GameEngine* gmngn) {

	gameEngine = gmngn;
}

GameManager::~GameManager() {
	mouse.reset();
	menuScreen.reset();
	currentScreen = NULL;
}


bool GameManager::initializeGame(HWND hwnd, ComPtr<ID3D11Device> dvc) {

	device = dvc;


	guiOverlay = make_unique<GUIOverlay>();


	menuScreen.reset(new MenuManager());
	menuScreen->setGameManager(this);
	if (!menuScreen->initialize(device))
		return false;

	currentScreen = menuScreen.get();

	ShowCursor(false);

	return true;
}

void GameManager::reloadGraphicsAssets() {
	mouse->reloadGraphicsAsset(guiFactory.get());
	menuScreen->reloadGraphicsAssets();
	guiOverlay->reloadGraphicsAssets();
}


void GameManager::update(double deltaTime) {
	currentScreen->update(deltaTime);
	guiOverlay->update(deltaTime);
}


void GameManager::draw(SpriteBatch* batch) {

	currentScreen->draw(batch);
	guiOverlay->draw(batch);
}


void GameManager::loadLevel(const wchar_t* file) {
// do nothing for this example
}

void GameManager::loadMainMenu() {

	lastScreen = currentScreen;
	currentScreen = menuScreen.get();

}


void GameManager::pause() {

	if (currentScreen != NULL)
		currentScreen->pause();
}


void GameManager::exit() {
	gameEngine->exit();
}



void GameManager::controllerRemoved(ControllerSocketNumber controllerSocket,
	PlayerSlotNumber slotNumber) {
	currentScreen->controllerRemoved(controllerSocket, slotNumber);
}

void GameManager::newController(shared_ptr<Joystick> newStick) {
	currentScreen->newController(newStick);
}



vector<ComPtr<IDXGIAdapter>> GameManager::getAdapterList() {
	return gameEngine->getAdapterList();
}

vector<ComPtr<IDXGIOutput>> GameManager::getDisplayList() {
	return gameEngine->getDisplayList();
}

vector<ComPtr<IDXGIOutput>> GameManager::getDisplayListFor(size_t displayIndex) {
	return gameEngine->getDisplayListFor(displayIndex);
}

vector<ComPtr<IDXGIOutput>> GameManager::getDisplayListFor(
	ComPtr<IDXGIAdapter> adapter) {
	return gameEngine->getDisplayListFor(adapter);
}

vector<DXGI_MODE_DESC> GameManager::getDisplayModeList(size_t displayIndex) {
	return gameEngine->getDisplayModeList(displayIndex);
}

vector<DXGI_MODE_DESC> GameManager::getDisplayModeList(ComPtr<IDXGIOutput> display) {
	return gameEngine->getDisplayModeList(display);
}

//void GameManager::setDisplayMode(DXGI_MODE_DESC displayMode) {
//	gameEngine->setDisplayMode(displayMode);
//}

bool GameManager::setAdapter(size_t adapterIndex) {
	return gameEngine->setAdapter(adapterIndex);
}

bool GameManager::setDisplayMode(size_t displayModeIndex) {
	return gameEngine->changeDisplayMode(displayModeIndex);
}

bool GameManager::setFullScreen(bool isFullScreen) {
	return gameEngine->setFullScreen(isFullScreen);
}


size_t GameManager::getSelectedAdapterIndex() {
	return gameEngine->getSelectedAdapterIndex();
}

size_t GameManager::getSelectedDisplayIndex() {
	return gameEngine->getSelectedDisplayIndex();
}

size_t GameManager::getSelectedDisplayModeIndex() {
	return gameEngine->getSelectedDisplayModeIndex();
}
