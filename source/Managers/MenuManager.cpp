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
void MenuManager::update(double deltaTime, MouseController* mouse) {

	if (switchTo != NULL) {
	// not so optimal, but should be ok for menus
		currentScreen = switchTo;
		switchTo = NULL;
	}
	currentScreen->update(deltaTime, mouse);

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

const Vector2 & MenuManager::getPosition() const {
	return position;
}


/** **** MenuScreen abstract class **** */
MenuScreen::MenuScreen(MenuManager* mngr) {

	menuManager = mngr;
}

MenuScreen::~MenuScreen() {

	for (GUIControl* control : guiControls)
		delete control;

	guiControls.clear();
}

void MenuScreen::setGameManager(GameManager* gmMng) {

	game = gmMng;
}

void MenuScreen::pause() {
	// do nothing??
}

const Vector2& MenuScreen::getPosition() const {
	return position;
}


/** **** MainMenuScreen **** **/
MainScreen::MainScreen(MenuManager* mngr) : MenuScreen(mngr) {

}

MainScreen::~MainScreen() {
	guiControls.clear();
}


bool MainScreen::initialize(ComPtr<ID3D11Device> device, MouseController* mouse) {

	AnimatedButton* animButton =
		GameManager::guiFactory->createAnimatedButton("Launch Button");
	Vector2 buttonpos = Vector2((Globals::WINDOW_WIDTH - animButton->getWidth()) / 2,
		Globals::WINDOW_HEIGHT / 3 - animButton->getHeight() / 2);
	animButton->setPosition(buttonpos);
	guiControls.push_back(animButton);


	Button* button;
	//button = GameManager::guiFactory->createImageButton("Launch Button");
	////button->setText(L"Play");
	//button->setPosition(buttonpos);

	Vector2 size = Vector2(animButton->getWidth() / 2, animButton->getHeight() / 4);
	//guiControls.push_back(button);


	buttonpos.y += 150;
	button = GameManager::guiFactory->createButton(buttonpos, size, L"Settings");
	buttonpos.x = (Globals::WINDOW_WIDTH - button->getScaledWidth()) / 2;
	button->setPosition(buttonpos);
	OnClickListenerSettingsButton* settingsListener =
		new OnClickListenerSettingsButton(this);
	button->setOnClickListener(settingsListener);
	guiControls.push_back(button);


	button = GameManager::guiFactory->createImageButton(
		"Button Up", "Button Down");
	button->setOnClickListener(new OnClickListenerExitButton(this));
	button->setText(L"Exit");
	buttonpos.x = (Globals::WINDOW_WIDTH - button->getScaledWidth()) / 2;
	buttonpos.y += 150;
	button->setPosition(buttonpos);
	guiControls.push_back(button);


	test = GameManager::guiFactory->createTextLabel(Vector2(10, 10));
	guiControls.push_back(test);

	mouseLabel = GameManager::guiFactory->createTextLabel(Vector2(10, 100));
	mouseLabel->setAlpha(.1);
	guiControls.push_back(mouseLabel);

	{
		exitDialog.reset(GameManager::guiFactory->createDialog(true));
		Vector2 dialogPos, dialogSize;
		dialogSize = Vector2(Globals::WINDOW_WIDTH / 2, Globals::WINDOW_HEIGHT / 2);
		dialogPos = dialogSize;
		dialogPos.x -= dialogSize.x / 2;
		dialogPos.y -= dialogSize.y / 2;
		exitDialog->setDimensions(dialogPos, dialogSize);
		exitDialog->setTint(Color(0, .5, 1, 1));
		exitDialog->setTitle(L"Exit Test?", Vector2(1, 1), "BlackCloak");
		//exitDialog->setTitleAreaDimensions(Vector2(0, 150));
		exitDialog->setText(L"Really Quit The Test Project?");
		unique_ptr<Button> quitButton;
		quitButton.reset(GameManager::guiFactory->createButton());
		quitButton->setOnClickListener(new OnClickListenerDialogQuitButton(this));
		quitButton->setText(L"Quit");
		exitDialog->setConfirmButton(move(quitButton));
		exitDialog->setCancelButton(L"Keep Testing!");

		exitDialog->setOpenTransition(
			new TransitionEffects::TrueGrowTransition(exitDialog.get(),
				Vector2(.001, .001), Vector2(1, 1), 1));
			/*new TransitionEffects::SlideAndGrowTransition(
				Vector2(-200, -200), exitDialog->getPosition(),
				Vector2(.001, .001), Vector2(1, 1)));*/
			/*new TransitionEffects::GrowTransition(
				Vector2(.0001, 0001), Vector2(1, 1)));*/
			/*new TransitionEffects::SlideTransition(
				Vector2(-200, -200), exitDialog->getPosition()));*/
		exitDialog->setCloseTransition(
			new TransitionEffects::ShrinkTransition(
				Vector2(1, 1), Vector2(.001, .001)));
	}



	return true;
}

Keyboard::KeyboardStateTracker keyTracker;
void MainScreen::update(double deltaTime, MouseController* mouse) {

	wostringstream ws;
	ws << "Mouse: " << mouse->getPosition().x << ", " << mouse->getPosition().y;
	mouseLabel->setText(ws);


	auto state = Keyboard::Get().GetState();
	keyTracker.Update(state);
	//if (keys->keyDown[KeyboardController::ESC] && !escLastStateDown) {
	//if (state.Escape && !escLastStateDown) {
	if (keyTracker.IsKeyReleased(Keyboard::Escape)) {
		if (exitDialog->isOpen)
			exitDialog->close();
		else
			exitDialog->open();
	}

	//escLastStateDown = keys->keyDown[KeyboardController::ESC];
	//escLastStateDown = state.Escape;

	if (exitDialog->isOpen) {
		exitDialog->update(deltaTime, mouse);
	} else {
		for (auto const& control : guiControls)
			control->update(deltaTime, mouse);
	}
}



void MainScreen::draw(SpriteBatch* batch) {

	for (auto const& control : guiControls)
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
int itemHeight = 32;
bool ConfigScreen::initialize(ComPtr<ID3D11Device> device, MouseController* mouse) {

	Vector2 controlPos = Vector2(50, 50);
	// Labels for displaying pressed info
	adapterLabel =
		GameManager::guiFactory->createTextLabel(controlPos, L"Test");
	adapterLabel->setHoverable(true);
	guiControls.push_back(adapterLabel);

	controlPos.y += adapterLabel->getHeight() + MARGIN;

	// create listbox of gfx cards
	adapterListbox =
		GameManager::guiFactory->createListBox(controlPos, 400, itemHeight);
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

	// create listbox of monitors available to pressed gfx card
	displayListbox = GameManager::guiFactory->createListBox(controlPos, 400, itemHeight);
	guiControls.push_back(displayListbox);
	// because only the one adapter has displays on my laptop
	// this has to be grab the first (and only) display.
	populateDisplayList(game->getDisplayListFor(0));
	displayListbox->setSelected(game->getSelectedDisplayIndex());

	displayLabel->setText(displayListbox->getSelected()->toString());

	// setup label for Display Mode
	controlPos.x += displayListbox->getWidth() + MARGIN * 2;
	controlPos.y = 50;


	// Setup display mode combobox

	// custom scrollbar for combo list
	ScrollBarDesc scrollBarDesc;
	scrollBarDesc.upButtonImage = "ScrollBar Up Custom";
	scrollBarDesc.upPressedButtonImage = "ScrollBar Up Pressed Custom";
	//scrollBarDesc.downButtonImage = "ScrollBar Down";
	scrollBarDesc.trackImage = "ScrollBar Track Custom";
	scrollBarDesc.scrubberImage = "Scrubber Custom";

	displayModeCombobox =
		GameManager::guiFactory->createComboBox(controlPos, 75, itemHeight, 10, true);

	populateDisplayModeList(
		game->getDisplayModeList(0));
	displayModeCombobox->setScrollBar(scrollBarDesc);
	displayModeCombobox->setSelected(game->getSelectedDisplayModeIndex());
	guiControls.push_back(displayModeCombobox);

	OnClickListenerDisplayModeList* onClickDisplayMode =
		new OnClickListenerDisplayModeList(this);
	displayModeCombobox->setOnClickListener(onClickDisplayMode);


	// Set up CheckBox for full-screen toggle
	CheckBox* check = GameManager::guiFactory->createCheckBox(
		Vector2(50, 450), L"Full Screen");
	OnClickListenerFullScreenCheckBox* onClickFullScreen
		= new OnClickListenerFullScreenCheckBox(this);
	check->setChecked(Globals::FULL_SCREEN);
	check->setOnClickListener(onClickFullScreen);
	guiControls.push_back(check);

	testLabel = GameManager::guiFactory->createTextLabel(
		Vector2(250, 450), L"Test Messages here");
	guiControls.push_back(testLabel);

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


void ConfigScreen::update(double deltaTime, MouseController* mouse) {

	auto state = Keyboard::Get().GetState();
	keyTracker.Update(state);
	//if (escLastStateDown && !keys->keyDown[KeyboardController::ESC]) {
	//if (!state.Escape && escLastStateDown) {
	if (keyTracker.IsKeyReleased(Keyboard::Escape)) {
		menuManager->openMainMenu();
	}

	//escLastStateDown = keys->keyDown[KeyboardController::ESC];
	escLastStateDown = state.Escape;

	for (auto const& control : guiControls) {
		control->update(deltaTime, mouse);
		//if (control->clicked()) {
		//	switch (control->action) {
		//		case GUIControl::CANCEL:
		//			menuManager->openMainMenu();
		//			break;
		//		case GUIControl::CONFIRM:
		//			// update graphics engine

		//			break;
		//	}
		//}
	}

}

void ConfigScreen::draw(SpriteBatch* batch) {

	for (auto const& control : guiControls)
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

	displayModeCombobox->clear();
	vector<ListItem*> displayModeItems;
	for (DXGI_MODE_DESC mode : displayModes) {
		if (!Globals::FULL_SCREEN && mode.Scaling == 0)
			continue;
		DisplayModeItem* item = new DisplayModeItem();
		item->modeDesc = mode;
		displayModeItems.push_back(item);
	}
	displayModeCombobox->addItems(displayModeItems);
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

void OnClickListenerDisplayModeList::onClick(ComboBox* combobox, int selectedIndex) {

	if (!config->game->setDisplayMode(selectedIndex)) {
		// change back to previous setting
		config->testLabel->setText("Display mode switch failed!");
	} else {
		// reconstruct display
		config->testLabel->setText(combobox->getItem(selectedIndex)->toString());
	}

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
	config->populateDisplayModeList(config->game->getDisplayModeList(0));
}

void OnClickListenerSettingsButton::onClick(Button* button) {

	main->menuManager->openConfigMenu();
}

void OnClickListenerDialogQuitButton::onClick(Button* button) {
	main->exitDialog->close();
	main->confirmExit();
}

void OnClickListenerExitButton::onClick(Button* button) {
	main->exitDialog->open();
}
