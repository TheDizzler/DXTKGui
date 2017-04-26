#pragma once



#include "../DXTKGui/BaseGraphics/Screen.h"
#include "../DXTKGui/Controls/Dialog.h"
#include "../DXTKGui/Controls/ComboBox.h"
#include "../DXTKGui/Controls/Button.h"
#include "../DXTKGui/Controls/CheckBox.h"
#include "../DXTKGUI/Effects/ScreenTransitions.h"

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


class MenuManager : public Screen {
public:
	MenuManager();
	virtual ~MenuManager();


	virtual void setGameManager(GameManager* game);

	virtual bool initialize(ComPtr<ID3D11Device> device, shared_ptr<MouseController> mouse);
	virtual void update(double deltaTime, shared_ptr<MouseController> mouse);
	virtual void draw(SpriteBatch* batch);

	virtual void pause() override;


	unique_ptr<ScreenTransitions::ScreenTransitionManager> transitionManager;

	void openMainMenu();
	void openConfigMenu();

private:

	Screen* currentScreen = NULL;
	Screen* switchTo = NULL;
	unique_ptr<MainScreen> mainScreen;
	unique_ptr<ConfigScreen> configScreen;

	Vector2 position = Vector2::Zero;

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

	vector<GUIControl*> guiControls;
};


class BackButtonListener : public Button::OnClickListener {
public:
	BackButtonListener(ConfigScreen* screen) : configScreen(screen) {
	}
	virtual void onClick(Button * button) override;

private:
	ConfigScreen* configScreen;
};

class ConfigScreen : public MenuScreen {
	friend class OnClickListenerAdapterList;
	friend class OnClickListenerDisplayModeList;
	friend class OnClickListenerFullScreenCheckBox;
	friend class BackButtonListener;
public:
	ConfigScreen(MenuManager* manager);
	~ConfigScreen();

	virtual bool initialize(ComPtr<ID3D11Device> device, shared_ptr<MouseController> mouse) override;
	virtual void update(double deltaTime, shared_ptr<MouseController> mouse) override;
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
	friend class OnClickListenerExitButton;
	friend class OnClickListenerDialogQuitButton;
public:
	MainScreen(MenuManager* manager);
	~MainScreen();

	virtual bool initialize(ComPtr<ID3D11Device> device, shared_ptr<MouseController> mouse) override;
	virtual void update(double deltaTime, shared_ptr<MouseController> mouse) override;
	virtual void draw(SpriteBatch* batch) override;

	void confirmExit();
private:
	unique_ptr<Dialog> exitDialog;
	TextLabel* test;
	TextLabel* mouseLabel;
	TextLabel* fpsLabel;
};

