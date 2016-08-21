#pragma once

#include <vector>

#include "Screen.h"
#include "../Controls/Dialog.h"
#include "../Controls/ListBox.h"
#include "../Controls/Button.h"


class MenuScreen;
class MainScreen;
class ConfigScreen;

class DisplayModeItem : public ListItem {
public:
	DXGI_MODE_DESC modeDesc;

protected:
	virtual void setText();

};

class AdapterItem : public ListItem {
public:
	IDXGIAdapter* adapter;

protected:
	virtual void setText();

};

class MenuManager : public Screen {
public:
	MenuManager();
	~MenuManager();


	virtual void setGameManager(GameManager* game);

	virtual bool initialize(ComPtr<ID3D11Device> device, MouseController* mouse);
	virtual void update(double deltaTime, KeyboardController* keys,
		MouseController* mouse);
	virtual void draw(SpriteBatch* batch);

	virtual void pause() override;

	void openMainMenu();
	void openConfigMenu();

	//unique_ptr<FontSet> menuFont;

private:

	Screen* currentScreen = NULL;
	unique_ptr<MainScreen> mainScreen;
	unique_ptr<ConfigScreen> configScreen;


	GameManager* game;

	
	//unique_ptr<GUIForm> gui;
};


/** This class is abstract. */
class MenuScreen : public Screen {
public:

	MenuScreen(MenuManager* manager);
	~MenuScreen();

	// Inherited via Screen
	virtual void setGameManager(GameManager* game) override;

	virtual void pause() override;
protected:

	GameManager* game;
	MenuManager* menuManager;
	shared_ptr<FontSet> menuFont;

	//vector<TextLabel*> textLabels;
	//vector<TextButton*> buttons;


	vector<GUIControl*> guiControls;

	//vector<ListBox*> listBoxes;

};


class ConfigScreen : public MenuScreen {
public:
	ConfigScreen(MenuManager* manager);
	~ConfigScreen();

	// Inherited via MenuScreen
	virtual bool initialize(ComPtr<ID3D11Device> device, MouseController * mouse) override;
	virtual void update(double deltaTime, KeyboardController* keys, MouseController * mouse) override;
	virtual void draw(SpriteBatch * batch) override;

private:

};

class MainScreen : public MenuScreen {
public:
	MainScreen(MenuManager* manager);
	~MainScreen();

	// Inherited via MenuScreen
	virtual bool initialize(ComPtr<ID3D11Device> device, MouseController * mouse) override;
	virtual void update(double deltaTime, KeyboardController* keys, MouseController * mouse) override;
	virtual void draw(SpriteBatch * batch) override;

private:
	unique_ptr<Dialog> exitDialog;
	TextLabel* test;
	TextLabel* mouseLabel;

	void confirmExit();

	bool lastStateDown = false;
};

