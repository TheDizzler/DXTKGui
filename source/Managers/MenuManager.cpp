#include "MenuManager.h"
#include "GameManager.h"

MenuManager::MenuManager() {
}

MenuManager::~MenuManager() {
}

void MenuManager::setGameManager(GameManager* gm) {

	game = gm;


}


bool MenuManager::initialize(ComPtr<ID3D11Device> device, MouseController* mouse) {


	if (!mouse->loadMouseIcon(GameManager::guiFactory.get(), "Mouse Reticle"))
		return false;


	mainScreen.reset(new MainScreen(this));
	mainScreen->setGameManager(game);
	if (!mainScreen->initialize(device, mouse))
		return false;

	configScreen.reset(new ConfigScreen(this));
	configScreen->setGameManager(game);
	if (!configScreen->initialize(device, mouse))
		return false;



	currentScreen = mainScreen.get();
	return true;
}


#include "../globals.h"
void MenuManager::update(double deltaTime,
	KeyboardController* keys, MouseController* mouse) {

	if (switchTo != NULL) {
	// not so optimal, but should be ok for menus
		currentScreen = switchTo;
		switchTo = NULL;
	}
	currentScreen->update(deltaTime, keys, mouse);

}


void MenuManager::draw(SpriteBatch* batch) {

	currentScreen->draw(batch);

}

void MenuManager::pause() {
	// do nothing?
}

void MenuManager::openMainMenu() {

	// switch screens at next frame so draw() doesn't freak out
	switchTo = mainScreen.get();
}

void MenuManager::openConfigMenu() {

	//currentScreen = configScreen.get();

	// switch screens at next frame
	switchTo = configScreen.get();
}


/** **** MenuScreen abstract class **** */
MenuScreen::MenuScreen(MenuManager* mngr) {

	menuManager = mngr;
}

MenuScreen::~MenuScreen() {

	for each (GUIControl* control in guiControls)
		delete control;

	guiControls.clear();

}

void MenuScreen::setGameManager(GameManager* gmMng) {

	game = gmMng;
}

void MenuScreen::pause() {
	// do nothing??
}


/** **** MainMenuScreen **** **/
MainScreen::MainScreen(MenuManager* mngr) : MenuScreen(mngr) {

}

MainScreen::~MainScreen() {

}


bool MainScreen::initialize(ComPtr<ID3D11Device> device, MouseController* mouse) {

	Button* button;
	button = GameManager::guiFactory->createImageButton("Button Up", "Button Down");
	Vector2 buttonpos = Vector2((Globals::WINDOW_WIDTH - button->getWidth()) / 2,
		Globals::WINDOW_HEIGHT / 3 - button->getHeight() / 2);
	//button->action = GUIControl::PLAY;
	button->setText(L"Play");
	button->setPosition(buttonpos);
	guiControls.push_back(button);

	Vector2 size = Vector2(button->getWidth(), button->getHeight());
	buttonpos.y += 150;
	button = GameManager::guiFactory->createButton(buttonpos, size, L"Settings");
	//button->action = GUIControl::SETTINGS;
	OnClickListenerSettingsButton* settingsListener = new OnClickListenerSettingsButton(this);
	button->setOnClickListener(settingsListener);
	guiControls.push_back(button);

	buttonpos.y += 150;
	button = GameManager::guiFactory->createImageButton(
		"Button Up", "Button Down");
	//button->action = GUIControl::EXIT;
	button->setOnClickListener(new OnClickListenerExitButton(this));
	button->setText(L"Exit");
	button->setPosition(buttonpos);
	guiControls.push_back(button);


	test = GameManager::guiFactory->createTextLabel(Vector2(10, 10));
	guiControls.push_back(test);

	mouseLabel = GameManager::guiFactory->createTextLabel(Vector2(10, 100));
	guiControls.push_back(mouseLabel);

	{
		exitDialog.reset(GameManager::guiFactory->createDialog(true));
		Vector2 dialogPos, dialogSize;
		dialogSize = Vector2(Globals::WINDOW_WIDTH / 2, Globals::WINDOW_HEIGHT / 2);
		dialogPos = dialogSize;
		dialogPos.x -= dialogSize.x / 2;
		dialogPos.y -= dialogSize.y / 2;
		exitDialog->setDimensions(dialogPos, dialogSize);
		exitDialog->setTint(Color(0, 120, 207));
		exitDialog->setTitle(L"Exit Test?");
		//exitDialog->setTitleAreaDimensions(Vector2(0, 150));
		exitDialog->setText(L"Really Quit The Test Project?");
		unique_ptr<Button> quitButton;
		quitButton.reset(GameManager::guiFactory->createButton());
		quitButton->setOnClickListener(new OnClickListenerQuitButton(this));
		quitButton->setText(L"Quit");
		exitDialog->setConfirmButton(move(quitButton));
		exitDialog->setCancelButton(L"Keep Testing!");
	}



	return true;
}

int countDialog = 0;
void MainScreen::update(double deltaTime,
	KeyboardController* keys, MouseController* mouse) {

	wostringstream ws;
	ws << "Mouse: " << mouse->getPosition().x << ", " << mouse->getPosition().y;
	mouseLabel->setText(ws);



	if (keys->keyDown[KeyboardController::ESC] && !escLastStateDown) {
		if (exitDialog->isOpen)
			exitDialog->close();
		else
			exitDialog->open();
	}

	escLastStateDown = keys->keyDown[KeyboardController::ESC];


	if (exitDialog->isOpen) {
		exitDialog->update(deltaTime, mouse);
	} else {
		for (GUIControl* control : guiControls) {
			control->update(deltaTime, mouse);
		}
	}
}


void MainScreen::draw(SpriteBatch* batch) {

	for (GUIControl* control : guiControls)
		control->draw(batch);

	exitDialog->draw(batch);
}


void MainScreen::confirmExit() {

	//exitDialog->open();
	game->exit();
}



/** **** ConfigScreen **** **/
ConfigScreen::ConfigScreen(MenuManager* mngr) : MenuScreen(mngr) {

}

ConfigScreen::~ConfigScreen() {
}

int MARGIN = 10;
bool ConfigScreen::initialize(ComPtr<ID3D11Device> device, MouseController* mouse) {

	Vector2 controlPos = Vector2(50, 50);
	// Labels for displaying selected info
	adapterLabel =
		GameManager::guiFactory->createTextLabel(controlPos, L"Test");
	adapterLabel->setHoverable(true);
	guiControls.push_back(adapterLabel);

	controlPos.y += adapterLabel->getHeight() + MARGIN;

	// create listbox of gfx cards
	adapterListbox =
		GameManager::guiFactory->createListBox(controlPos, 400, 4, true);
	guiControls.push_back(adapterListbox);
	vector<ListItem*> adapterItems;
	for (ComPtr<IDXGIAdapter> adap : game->getAdapterList()) {
		AdapterItem* item = new AdapterItem();
		item->adapter = adap;
		adapterItems.push_back(item);
	}
	adapterListbox->addItems(adapterItems);
	adapterListbox->setSelected(game->getSelectedAdapterIndex());

	OnClickListenerAdapterList* onClickAdapterList = new OnClickListenerAdapterList(this);
	adapterListbox->setOnClickListener(onClickAdapterList);

	adapterLabel->setText(adapterListbox->getSelected()->toString());


	controlPos.y += adapterListbox->getHeight() + MARGIN * 2;
	displayLabel = GameManager::guiFactory->createTextLabel(controlPos, L"A");
	guiControls.push_back(displayLabel);

	controlPos.y += displayLabel->getHeight() + MARGIN;

	// create listbox of monitors available to selected gfx card
	displayListbox = GameManager::guiFactory->createListBox(controlPos, 400, 4, true);
	guiControls.push_back(displayListbox);
	// because only the one adapter has displays on my laptop
	// this has to be grab the first (and only) display.
	populateDisplayList(game->getDisplayListFor(0
	/*game->getSelectedAdapterIndex()*/));
	displayListbox->setSelected(game->getSelectedDisplayIndex());

	displayLabel->setText(displayListbox->getSelected()->toString());

	// setup label for Display Mode
	controlPos.x += displayListbox->getWidth() + MARGIN * 2;
	controlPos.y = 50;
	displayModeLabel =
		GameManager::guiFactory->createTextLabel(controlPos, L"Test2");
	guiControls.push_back(displayModeLabel);

	// Selected adapter display mode list
	controlPos.y += displayModeLabel->getHeight() + MARGIN;
	displayModeListbox =
		GameManager::guiFactory->createListBox(controlPos, 75, 8, true);
	populateDisplayModeList(
		game->getDisplayModeList(0 /*game->getSelectedAdapterIndex()*/));
	displayModeListbox->setSelected(game->getSelectedDisplayModeIndex());
	guiControls.push_back(displayModeListbox);

	OnClickListenerDisplayModeList* onClickDisplayMode =
		new OnClickListenerDisplayModeList(this);
	displayModeListbox->setOnClickListener(onClickDisplayMode);

	displayModeLabel->setText(displayModeListbox->getSelected()->toString());


	// Set up CheckBox for full-screen toggle
	CheckBox* check = GameManager::guiFactory->createCheckBox(
		Vector2(50, 450), L"Full Screen");
	OnClickListenerFullScreenCheckBox* onClickFullScreen
		= new OnClickListenerFullScreenCheckBox(this);
	check->setChecked(Globals::FULL_SCREEN);
	check->setOnClickListener(onClickFullScreen);
	guiControls.push_back(check);


	// Create Apply and Cancel Buttons
	ImageButton* button = (ImageButton*) GameManager::guiFactory->
		createImageButton("Button Up", "Button Down");
	button->action = Button::ClickAction::CANCEL;
	button->setText(L"Back");
	button->setPosition(
		Vector2(Globals::WINDOW_WIDTH / 2 - button->getWidth(),
			Globals::WINDOW_HEIGHT - button->getHeight() - 25));
	guiControls.push_back(button);

	button = (ImageButton*) GameManager::guiFactory->
		createImageButton("Button Up", "Button Down");
	button->action = Button::ClickAction::OK;
	button->setText(L"Apply");
	button->setPosition(
		Vector2(Globals::WINDOW_WIDTH / 2,
			Globals::WINDOW_HEIGHT - button->getHeight() - 25));
	guiControls.push_back(button);


	return true;
}


void ConfigScreen::update(double deltaTime, KeyboardController* keys,
	MouseController* mouse) {


	if (escLastStateDown && !keys->keyDown[KeyboardController::ESC]) {
		menuManager->openMainMenu();
	}

	escLastStateDown = keys->keyDown[KeyboardController::ESC];

	for (GUIControl* control : guiControls) {
		control->update(deltaTime, mouse);
		if (control->clicked()) {
			switch (control->action) {
				case GUIControl::CANCEL:
					menuManager->openMainMenu();
					break;
				case GUIControl::CONFIRM:
					// update graphics engine

					break;
			}
		}
	}

}

void ConfigScreen::draw(SpriteBatch* batch) {

	for (GUIControl* control : guiControls)
		control->draw(batch);
}

void ConfigScreen::populateDisplayList(vector<ComPtr<IDXGIOutput>> displays) {

	displayListbox->clear();
	vector<ListItem*> adapterOuts;
	for (ComPtr<IDXGIOutput> adap : displays) {

		DisplayItem* item = new DisplayItem();
		item->adapterOutput = adap;
		adapterOuts.push_back(item);
	}
	displayListbox->addItems(adapterOuts);
}

void ConfigScreen::populateDisplayModeList(vector<DXGI_MODE_DESC> displayModes) {

	displayModeListbox->clear();
	vector<ListItem*> displayModeItems;
	for (DXGI_MODE_DESC mode : displayModes) {
		DisplayModeItem* item = new DisplayModeItem();
		item->modeDesc = mode;
		displayModeItems.push_back(item);
	}
	displayModeListbox->addItems(displayModeItems);
}


void AdapterItem::setText() {

	DXGI_ADAPTER_DESC desc;
	ZeroMemory(&desc, sizeof(DXGI_ADAPTER_DESC));
	adapter->GetDesc(&desc);
	wostringstream wss;
	if (isEnumerated) {
		wss << listPosition << ": ";
	}
	wss << desc.Description;
	textLabel->setText(wss);
}

void DisplayItem::setText() {

	DXGI_OUTPUT_DESC desc;
	adapterOutput->GetDesc(&desc);
	wostringstream wss;
	if (isEnumerated) {
		wss << listPosition << ": ";
	}
	wss << desc.DeviceName;
	textLabel->setText(wss);
}

void DisplayModeItem::setText() {

	UINT width = modeDesc.Width;
	UINT height = modeDesc.Height;

	wostringstream wss;
	if (isEnumerated) {
		wss << listPosition << ": ";
	}
	wss << width << " x " << height;
	wss << " Fill: " << ((modeDesc.Scaling == 0) ? L"Yes" : L"No");
	textLabel->setText(wss.str());

}


void OnClickListenerAdapterList::onClick(ListBox* listbox, int selectedIndex) {

	AdapterItem* selectedItem = (AdapterItem*) listbox->getItem(selectedIndex);
	config->game->setAdapter(selectedIndex);
	config->populateDisplayList(config->game->getDisplayList());
	config->populateDisplayModeList(
		config->game->getDisplayModeList(0
		/*config->game->getSelectedAdapterIndex()*/));

	config->adapterLabel->setText(listbox->getSelected()->toString());
}

void OnClickListenerDisplayModeList::onClick(ListBox* listbox, int selectedIndex) {

	//DisplayModeItem* displayMode = (DisplayModeItem*) listbox->getItem(selectedIndex);
	//config->game->setDisplayModeList(displayMode->modeDesc);
	if (!config->game->setDisplayMode(selectedIndex)) {
		// change back to previous setting
	} else {
		// reconstruct display

	}

	config->displayModeLabel->setText(listbox->getItem(selectedIndex)->toString());
}

void OnClickListenerFullScreenCheckBox::onClick(CheckBox* checkbox, bool isChecked) {

	/*wostringstream wss;
	wss << "CheckBox is checked: " << isChecked;
	config->displayModeLabel->setText(wss);*/
	if (!config->game->setFullScreen(isChecked)) {
		// revert to old settings
	} else {
		// reconstruct display
	}
}

void OnClickListenerSettingsButton::onClick(Button* button) {

	main->menuManager->openConfigMenu();
}

void OnClickListenerQuitButton::onClick(Button* button) {
	main->confirmExit();
}

void OnClickListenerExitButton::onClick(Button* button) {
	main->exitDialog->open();
}
