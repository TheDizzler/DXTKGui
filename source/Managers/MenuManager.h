#include "../pch.h"
#pragma once


#include "Screen.h"
#include "../Controls/Dialog.h"
#include "../Controls/ComboBox.h"
#include "../Controls/Button.h"
#include "../Controls/CheckBox.h"


class MenuScreen;
class MainScreen;
class ConfigScreen;

class DisplayModeItem : public ListItem {
public:
	DXGI_MODE_DESC modeDesc;

protected:
	virtual void setText();

};

class DisplayItem : public ListItem {
public:
	ComPtr<IDXGIOutput> adapterOutput;

protected:
	virtual void setText();
};

class AdapterItem : public ListItem {
public:
	ComPtr<IDXGIAdapter> adapter;

protected:
	virtual void setText();

};

class OnClickListenerAdapterList : public ListBox::OnClickListener {
public:
	OnClickListenerAdapterList(ConfigScreen* screen) : config(screen) {
	}
	virtual void onClick(ListBox* listbox, int selectedIndex) override;
private:
	ConfigScreen* config;
};

class OnClickListenerDisplayModeList : public ComboBox::OnClickListener {
public:
	OnClickListenerDisplayModeList(ConfigScreen* screen) : config(screen) {
	}
	virtual void onClick(ComboBox* combobox, int selectedIndex) override;
private:
	ConfigScreen* config;

};

class OnClickListenerFullScreenCheckBox : public CheckBox::OnClickListener {
public:
	OnClickListenerFullScreenCheckBox(ConfigScreen* screen) : config(screen) {
	}
	virtual void onClick(CheckBox* checkbox, bool isChecked) override;
private:
	ConfigScreen* config;
};

class OnClickListenerSettingsButton : public Button::OnClickListener {
public:
	OnClickListenerSettingsButton(MainScreen* screen) : main(screen) {
	}
	virtual void onClick(Button* button) override;
private:
	MainScreen* main;
};

class OnClickListenerDialogQuitButton : public Button::OnClickListener {
public:
	OnClickListenerDialogQuitButton(MainScreen* screen) : main(screen) {
	}
	virtual void onClick(Button * button) override;
private:
	MainScreen* main;
};

class OnClickListenerExitButton : public Button::OnClickListener {
public:
	OnClickListenerExitButton(MainScreen* screen) : main(screen) {
	}
	virtual void onClick(Button* button) override;
private:
	MainScreen* main;
};

//class OnClickListenerLaunchButton : public AnimatedButton::ButtonActionListener {
//public:
//	OnClickListenerLaunchButton(MainScreen* screen) : main(screen) {
//	}
//
//	virtual void onClick(AnimatedButton* button) override {
//	
//	
//	}
//	virtual void onPress(AnimatedButton* button) override {
//
//
//	}
//
//	virtual void onHover(AnimatedButton* button) override {
//
//		
//
//	}
//
//private:
//	MainScreen* main;
//
//};


class MenuManager : public Screen {
public:
	MenuManager();
	virtual ~MenuManager();


	virtual void setGameManager(GameManager* game);

	virtual bool initialize(ComPtr<ID3D11Device> device, MouseController* mouse);
	virtual void update(double deltaTime, KeyboardController* keys,
		MouseController* mouse);
	virtual void draw(SpriteBatch* batch);

	virtual void pause() override;

	void openMainMenu();
	void openConfigMenu();

private:

	Screen* currentScreen = NULL;
	Screen* switchTo = NULL;
	unique_ptr<MainScreen> mainScreen;
	unique_ptr<ConfigScreen> configScreen;


	GameManager* game;
};


/** This class is abstract. */
interface MenuScreen : public Screen {
	friend class OnClickListenerSettingsButton;
public:

	MenuScreen(MenuManager* manager);
	virtual ~MenuScreen();


	virtual void setGameManager(GameManager* game) override;

	virtual void pause() override;
protected:

	GameManager* game;
	MenuManager* menuManager;

	vector<GUIControl* > guiControls;

	bool escLastStateDown = false;

};


class ConfigScreen : public MenuScreen {
	friend class OnClickListenerAdapterList;
	friend class OnClickListenerDisplayModeList;
	friend class OnClickListenerFullScreenCheckBox;
public:
	ConfigScreen(MenuManager* manager);
	~ConfigScreen();

	// Inherited via MenuScreen
	virtual bool initialize(ComPtr<ID3D11Device> device,
		MouseController* mouse) override;

	virtual void update(double deltaTime,
		KeyboardController* keys, MouseController * mouse) override;
	virtual void draw(SpriteBatch * batch) override;

private:

	void populateDisplayList(vector<ComPtr<IDXGIOutput> > displays);
	void populateDisplayModeList(vector<DXGI_MODE_DESC> displayModes);
	TextLabel* adapterLabel;
	TextLabel* displayLabel;
	TextLabel* testLabel;


	ListBox* adapterListbox;
	ListBox* displayListbox;
	ComboBox* displayModeCombobox;


};

class MainScreen : public MenuScreen {
	friend class OnClickListenerExitButton;
	friend class OnClickListenerDialogQuitButton;
public:
	MainScreen(MenuManager* manager);
	~MainScreen();

	// Inherited via MenuScreen
	virtual bool initialize(ComPtr<ID3D11Device> device,
		MouseController * mouse) override;
	virtual void update(double deltaTime, KeyboardController* keys,
		MouseController* mouse) override;
	virtual void draw(SpriteBatch * batch) override;

	void confirmExit();
private:
	unique_ptr<Dialog> exitDialog;
	TextLabel* test;
	TextLabel* mouseLabel;


};

