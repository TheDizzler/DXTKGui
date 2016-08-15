#include "GameManager.h"


GameManager::GameManager(GameEngine* gmngn) {

	gameEngine = gmngn;
}

GameManager::~GameManager() {
}

bool GameManager::initializeGame(ID3D11Device* dvc, MouseController* ms) {

	device = dvc;
	mouse = ms;

	// get graphical assets from xml file
	docAssMan.reset(new pugi::xml_document());
	if (!docAssMan->load_file(Assets::assetManifestFile)) {
		MessageBox(0, L"Could not read AssetManifest file!", L"Fatal Read Error!", MB_OK;
		return false;
	}

	pugi::xml_node guiNode = docAssMan->child("root").child("gui");

	guiManager.reset(new GUIManager(guiNode));
	if (!guiManager->initialize(device)) {
		MessageBox(0, L"Failed to load GUIManager", L"Fatal Error", MB_OK);
		return false;
	}


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
// do nothing for this example
}

void GameManager::loadMainMenu() {

	lastScreen = currentScreen;
	currentScreen = menuScreen.get();

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






