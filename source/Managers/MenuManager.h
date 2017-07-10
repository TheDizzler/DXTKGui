#pragma once

#include "../DXTKGui/GUIFactory.h"

//#include "../DXTKGui/BaseGraphics/Screen.h"
#include "../DXTKGUI/Effects/ScreenTransitions.h"

class MenuScreen;
class MainScreen;
class MenuManager;
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

class OnClickListenerAdapterList : public ListBox::ActionListener {
public:
	OnClickListenerAdapterList(ConfigScreen* screen) : config(screen) {
	}
	virtual void onClick(ListBox* listbox, int selectedIndex) override;
private:
	ConfigScreen* config;
};

class OnClickListenerDisplayModeList : public ComboBox::ActionListener {
public:
	OnClickListenerDisplayModeList(ConfigScreen* screen) : config(screen) {
	}
	virtual void onClick(ComboBox* combobox, int selectedIndex) override;
private:
	ConfigScreen* config;

};

class OnClickListenerFullScreenCheckBox : public CheckBox::ActionListener {
public:
	OnClickListenerFullScreenCheckBox(ConfigScreen* screen) : config(screen) {
	}
	virtual void onClick(CheckBox* checkbox, bool isChecked) override;
private:
	ConfigScreen* config;
};

class BackButtonListener : public Button::ActionListener {
public:
	BackButtonListener(ConfigScreen* screen) : configScreen(screen) {
	}
	virtual void onClick(Button* button) override;
	virtual void onPress(Button* button) override;
	virtual void onHover(Button* button) override;

private:
	ConfigScreen* configScreen;
};

class OnClickListenerSettingsButton : public Button::ActionListener {
public:
	OnClickListenerSettingsButton(MainScreen* screen) : main(screen) {
	}
	virtual void onClick(Button* button) override;
	virtual void onPress(Button * button) override;
	virtual void onHover(Button * button) override;
private:
	MainScreen* main;
};

class OnClickListenerDialogQuitButton : public Button::ActionListener {
public:
	OnClickListenerDialogQuitButton(MenuManager* screen) : main(screen) {
	}
	virtual void onClick(Button* button) override;
	virtual void onPress(Button* button) override;
	virtual void onHover(Button* button) override;
private:
	MenuManager* main;
};

class OnClickListenerExitButton : public Button::ActionListener {
public:
	OnClickListenerExitButton(MainScreen* screen) : main(screen) {
	}
	virtual void onClick(Button* button) override;
	virtual void onPress(Button* button) override;
	virtual void onHover(Button* button) override;
private:
	MainScreen* main;


};

class TestButtonActionListener : public Button::ActionListener {
public:
	TestButtonActionListener(DynamicDialog* dlg) : dialog(dlg){}
	virtual void onClick(Button* button) override;
	virtual void onPress(Button* button) override;
	virtual void onHover(Button* button) override;
private:
	DynamicDialog* dialog;
};


class MenuManager : public Screen {
	friend class OnClickListenerExitButton;
	friend class OnClickListenerDialogQuitButton;
public:
	MenuManager();
	virtual ~MenuManager();


	virtual void setGameManager(GameManager* game);

	virtual bool initialize(ComPtr<ID3D11Device> device, shared_ptr<MouseController> mouse);
	virtual void update(double deltaTime);
	virtual void draw(SpriteBatch* batch);

	virtual void pause() override;

	virtual void controllerRemoved(size_t controllerSlot);
	virtual void newController(HANDLE joyHandle);

	unique_ptr<ScreenTransitions::ScreenTransitionManager> transitionManager;

	void openMainMenu();
	void openConfigMenu();

	void confirmExit();

private:
	shared_ptr<MouseController> mouse;
	Screen* currentScreen = NULL;
	Screen* switchTo = NULL;
	unique_ptr<MainScreen> mainScreen;
	unique_ptr<ConfigScreen> configScreen;
	unique_ptr<PromptDialog> exitDialog;

	Vector2 position = Vector2::Zero;

	GameManager* game;

};


/** This class is abstract. */
interface MenuScreen : public Screen {
	friend class OnClickListenerSettingsButton;
	friend class OnClickListenerExitButton;
public:

	MenuScreen(MenuManager* manager);
	virtual ~MenuScreen();

	virtual void setGameManager(GameManager* game) override;
	virtual void pause() override;

	virtual void controllerRemoved(size_t controllerSlot);
	virtual void newController(HANDLE joyHandle);

protected:

	GameManager* game;
	MenuManager* menuManager;

	vector<GUIControl*> guiControls;


};


class ConfigScreen : public MenuScreen {
	friend class OnClickListenerAdapterList;
	friend class OnClickListenerDisplayModeList;
	friend class OnClickListenerFullScreenCheckBox;
	friend class BackButtonListener;
public:
	ConfigScreen(MenuManager* manager);
	virtual ~ConfigScreen();

	virtual bool initialize(ComPtr<ID3D11Device> device, shared_ptr<MouseController> mouse) override;
	virtual void update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;

private:

	void populateDisplayList(vector<ComPtr<IDXGIOutput> > displays);
	void populateDisplayModeList(vector<DXGI_MODE_DESC> displayModes);
	TextLabel* adapterLabel;
	TextLabel* displayLabel;
	TextLabel* testLabel;


	ListBox* adapterListbox;
	ListBox* displayListbox;
	ComboBox* displayModeCombobox;

	Spinner* testSpinner;

};

class MainScreen : public MenuScreen {
public:
	MainScreen(MenuManager* manager);
	virtual ~MainScreen();

	virtual bool initialize(ComPtr<ID3D11Device> device, shared_ptr<MouseController> mouse) override;
	virtual void update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;

private:
	shared_ptr<MouseController> mouse;
	//unique_ptr<PromptDialog> exitDialog;
	unique_ptr<DynamicDialog> dynamicDialog;
	TextLabel* mouseLabel;
};

