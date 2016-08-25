#include "GameManager.h"

unique_ptr<GUIFactory> GameManager::guiFactory;
GameManager::GameManager(GameEngine* gmngn) {

	gameEngine = gmngn;
}

GameManager::~GameManager() {
}

bool GameManager::initializeGame(ComPtr<ID3D11Device> dvc, MouseController* ms) {

	device = dvc;
	mouse = ms;

	// get graphical assets from xml file
	docAssMan.reset(new pugi::xml_document());
	if (!docAssMan->load_file(Assets::assetManifestFile)) {
		MessageBox(0, L"Could not read AssetManifest file!",
			L"Fatal Read Error!", MB_OK);
		return false;
	}

	pugi::xml_node guiNode = docAssMan->child("root").child("gui");

	guiFactory.reset(new GUIFactory(guiNode));
	if (!guiFactory->initialize(device)) {
		MessageBox(0, L"Failed to load GUIFactory", L"Fatal Error", MB_OK);
		return false;
	}


	menuScreen.reset(new MenuManager());
	menuScreen->setGameManager(this);
	if (!menuScreen->initialize(device, mouse))
		return false;


	currentScreen = menuScreen.get();

	ShowCursor(false);

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


void GameManager::pause() {

	if (currentScreen != NULL)
		currentScreen->pause();
}

#include "../Engine/GameEngine.h"
void GameManager::exit() {
	gameEngine->exit();
}


vector<ComPtr<IDXGIAdapter>> GameManager::getAdapterList() {
	return gameEngine->getAdapterList();
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


size_t GameManager::getSelectedAdapterIndex() {
	return gameEngine->getSelectedAdapterIndex();
}

size_t GameManager::getSelectedDisplayIndex() {
	return gameEngine->getSelectedDisplayIndex();
}

size_t GameManager::getSelectedDisplayModeIndex() {
	return gameEngine->getSelectedDisplayModeIndex();
}




//vector<wstring> GameManager::getAdapterListDescriptions() {
//	return gameEngine->getAdapterListDescriptions();
//}
//vector<wstring> GameManager::getDisplayModeListDescriptions(size_t adapterIndex) {
//	return gameEngine->getDisplayModeListDescriptions(adapterIndex);
//}

