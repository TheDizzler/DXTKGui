#pragma once


#include "MenuManager.h"
#include "../DXTKGui/GUIFactory.h"
#include "../Screens/GUIOverlay.h"
#include "../DXTKGui/Controllers/Joystick.h"

class GameEngine;

extern unique_ptr<GUIOverlay> guiOverlay;

/** The lowest level of class where game code should be included.
	Everything below this (GameEngine downward) should generally go unmodified. */
class GameManager {
public:
	GameManager(GameEngine* gameEngine);
	virtual ~GameManager();


	bool initializeGame(HWND hwnd, ComPtr<ID3D11Device> device);

	void reloadGraphicsAssets();

	void update(double deltaTime);
	void draw(SpriteBatch* batch);


	void loadLevel(const wchar_t* file);
	void loadMainMenu();

	void pause();
	void exit();

	void controllerRemoved(ControllerSocketNumber controllerSocket,
		PlayerSlotNumber slotNumber);
	void newController(shared_ptr<Joystick> newStick);

	vector<ComPtr<IDXGIAdapter> > getAdapterList();
	vector<ComPtr<IDXGIOutput> > getDisplayList();
	vector<ComPtr<IDXGIOutput> > getDisplayListFor(size_t displayIndex);
	vector<ComPtr<IDXGIOutput> > getDisplayListFor(ComPtr<IDXGIAdapter> adapter);
	vector<DXGI_MODE_DESC> getDisplayModeList(size_t displayIndex);
	vector<DXGI_MODE_DESC> getDisplayModeList(ComPtr<IDXGIOutput> display);

	bool setAdapter(size_t adapterIndex);
	bool setDisplayMode(size_t displayModeIndex);
	bool setFullScreen(bool isFullScreen);

	size_t getSelectedAdapterIndex();
	size_t getSelectedDisplayIndex();
	size_t getSelectedDisplayModeIndex();


private:

	Screen* currentScreen = 0;
	Screen* lastScreen = 0;
	unique_ptr<MenuManager> menuScreen;


	GameEngine* gameEngine;
	ComPtr<ID3D11Device> device;


};