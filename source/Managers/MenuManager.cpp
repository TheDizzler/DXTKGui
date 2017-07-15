#include "MenuManager.h"
#include "GameManager.h"
#include "../Engine/GameEngine.h"
#include "../globals.h"

#include <sstream>

MenuManager::MenuManager() {
}

MenuManager::~MenuManager() {

	mouse.reset();
	currentScreen = NULL;
	switchTo = NULL;
	mainScreen.reset();
	configScreen.reset();
	game = NULL;
}

void MenuManager::setGameManager(GameManager* gm) {

	game = gm;


}


bool MenuManager::initialize(ComPtr<ID3D11Device> device, shared_ptr<MouseController> mc) {

	mouse = mc;

	{
		Vector2 dialogPos, dialogSize;
		dialogSize = Vector2(Globals::WINDOW_WIDTH / 2, Globals::WINDOW_HEIGHT / 2);
		dialogPos = dialogSize;
		dialogPos.x -= dialogSize.x / 2;
		dialogPos.y -= dialogSize.y / 2;
		exitDialog.reset(guiFactory->createDialog(dialogPos, dialogSize, true, true));
		exitDialog->setTint(Color(0, .5, 1, 1));
		exitDialog->setTitle(L"Exit Test?", Vector2(1, 1), "BlackCloak");
		exitDialog->setText(L"Really Quit The Test Project?");
		unique_ptr<Button> quitButton;
		quitButton.reset(guiFactory->createImageButton("Button Up", "Button Down"));
		//quitButton.reset(guiFactory->createButton());
		quitButton->setActionListener(new OnClickListenerDialogQuitButton(this));
		quitButton->setText(L"Quit");
		exitDialog->setConfirmButton(move(quitButton), true, false);
		exitDialog->setCancelButton(L"Keep Testing!");

		exitDialog->setOpenTransition(
			//new TransitionEffects::SpinGrowTransition(exitDialog.get(), .5));
			//	new TransitionEffects::SplitTransition(exitDialog.get(), 25));
			//new TransitionEffects::BlindsTransition(exitDialog.get(), .5, false, true));
			/*new TransitionEffects::TrueGrowTransition(exitDialog.get(),
			Vector2(.001, .001), Vector2(1, 1)));*/
			/* new TransitionEffects::SlideAndGrowTransition(exitDialog.get(),
			Vector2(-200, -200), exitDialog->getPosition(),
			Vector2(.001, .001), Vector2(1, 1)));*/
			new TransitionEffects::GrowTransition(exitDialog.get(),
				Vector2(.0001, 0001), Vector2(1, 1)));
		/*new TransitionEffects::SlideTransition(exitDialog.get(),
		Vector2(-200, -200), exitDialog->getPosition()));*/

		exitDialog->setCloseTransition(
			new TransitionEffects::ShrinkTransition(exitDialog.get(),
				Vector2(1, 1), Vector2(.001, .001)));

	}

	mainScreen.reset(new MainScreen(this));
	mainScreen->setGameManager(game);
	if (!mainScreen->initialize(device, mouse))
		return false;

	configScreen.reset(new ConfigScreen(this));
	configScreen->setGameManager(game);
	if (!configScreen->initialize(device, mouse))
		return false;
	configScreen->update(0);

	currentScreen = mainScreen.get();

	transitionManager = make_unique<ScreenTransitions::ScreenTransitionManager>();
	transitionManager->initialize(guiFactory.get(), "Test BG");
	transitionManager->setTransition(
		//new ScreenTransitions::FlipScreenTransition(true));
		new ScreenTransitions::SquareFlipScreenTransition());
		//new ScreenTransitions::LineWipeScreenTransition());


	return true;
}

void MenuManager::reloadGraphicsAssets() {
	exitDialog->reloadGraphicsAsset();
	configScreen->reloadGraphicsAssets();
	mainScreen->reloadGraphicsAssets();
	transitionManager->reloadGraphicsAssets();
}


Keyboard::KeyboardStateTracker keyTracker;
void MenuManager::update(double deltaTime) {

	if (switchTo != NULL) {
		if (transitionManager->runTransition(deltaTime)) {
			currentScreen = switchTo;
			switchTo = NULL;
		}
	} else {
		auto state = Keyboard::Get().GetState();
		keyTracker.Update(state);
		if (keyTracker.IsKeyReleased(Keyboard::Escape)) {
			if (exitDialog->isOpen())
				exitDialog->hide();
			else
				exitDialog->show();
		}

		if (exitDialog->isOpen()) {
			exitDialog->update(deltaTime);
		} else
			currentScreen->update(deltaTime);
	}
}


void MenuManager::draw(SpriteBatch* batch) {

	if (switchTo != NULL) {
		transitionManager->drawTransition(batch);
	} else {
		currentScreen->draw(batch);
		exitDialog->draw(batch);
	}
}

void MenuManager::pause() {
	// do nothing?
}


void MenuManager::controllerRemoved(ControllerSocketNumber controllerSlot,
	PlayerSlotNumber slotNumber) {
}

void MenuManager::newController(shared_ptr<Joystick> newStick) {
}


void MenuManager::openMainMenu() {

	// switch screens at next frame so draw() doesn't freak out
	switchTo = mainScreen.get();
	transitionManager->transitionBetween(currentScreen, switchTo);
}

void MenuManager::openConfigMenu() {

	// switch screens at next frame
	switchTo = configScreen.get();
	transitionManager->transitionBetween(currentScreen, switchTo);
}

void MenuManager::confirmExit() {
	game->exit();
}
/** **** END MENUMANAGER **** */



/** **** MenuScreen abstract class **** */
MenuScreen::MenuScreen(MenuManager* mngr) {

	menuManager = mngr;
}

MenuScreen::~MenuScreen() {

	for (auto& control : guiControls)
		delete control;

	guiControls.clear();
	game = NULL;
	menuManager = NULL;
}


void MenuScreen::setGameManager(GameManager* gmMng) {

	game = gmMng;
}

void MenuScreen::reloadGraphicsAssets() {
	for (auto& control : guiControls)
		control->reloadGraphicsAsset();
}

void MenuScreen::pause() {
	// do nothing??
}

void MenuScreen::newController(HANDLE joyHandle) {
}

void MenuScreen::controllerRemoved(size_t controllerSlot) {
}




/** **** MainMenuScreen **** **/
MainScreen::MainScreen(MenuManager* mngr) : MenuScreen(mngr) {

}

MainScreen::~MainScreen() {

}


bool MainScreen::initialize(ComPtr<ID3D11Device> device, shared_ptr<MouseController> mc) {

	mouse = mc;

	/*dynamicDialog.reset(guiFactory->createDynamicDialog(
		"Dynamic Dialog", Vector2(10, 100), Vector2(500, 500)));
	dynamicDialog->setTextTint(Color(0, 0, 1, 1));
	dynamicDialog->setText(L"YAASDHHASDHASOHDIOASHDIO asd asd asd\nYasdYAASDHHASDHASOHDIOASHDIO asd asd asd\n");
	dynamicDialog->show();*/

	Vector2 buttonpos = Vector2(100, 100);

	/*AnimatedButton* animButton =
		guiFactory->createAnimatedButton("Launch Button");
	buttonpos = Vector2((Globals::WINDOW_WIDTH - animButton->getWidth()) / 2,
		Globals::WINDOW_HEIGHT / 3 - animButton->getHeight() / 2);
	animButton->setPosition(buttonpos);
	guiControls.push_back(animButton);*/


	Button* button;
	Vector2 size = Vector2::Zero;
	/*button = guiFactory->createButton(buttonpos, size, L"Test");
	buttonpos.x = (Globals::WINDOW_WIDTH - button->getScaledWidth()) / 2;
	button->setPosition(buttonpos);
	button->setActionListener(new TestButtonActionListener(dynamicDialog.get()));
	guiControls.push_back(button);*/


	buttonpos.y += 150;
	button = guiFactory->createButton(buttonpos, size, L"Settings");
	buttonpos.x = (Globals::WINDOW_WIDTH - button->getScaledWidth()) / 2;
	button->setPosition(buttonpos);
	button->setActionListener(new OnClickListenerSettingsButton(this));
	guiControls.push_back(button);


	button = guiFactory->createImageButton("Button Up", "Button Down");
	button->setActionListener(new OnClickListenerExitButton(this));
	button->setText(L"Exit");
	buttonpos.x = (Globals::WINDOW_WIDTH - button->getScaledWidth()) / 2;
	buttonpos.y += 150;
	button->setPosition(buttonpos);
	guiControls.push_back(button);


	mouseLabel = guiFactory->createTextLabel(
		Vector2(0, 0), L"Mouse Label", "Default Font", false);
	//mouseLabel->setAlpha(.1);
	mouseLabel->setHoverable(true);
	guiControls.push_back(mouseLabel);

	//mouse->setAlpha(.5);


	return true;
}

void MainScreen::reloadGraphicsAssets() {
	MenuScreen::reloadGraphicsAssets();
}


void MainScreen::update(double deltaTime) {

	wostringstream ws;
	ws << "Mouse: " << mouse->getPosition().x << ", " << mouse->getPosition().y;
	mouseLabel->setText(ws);
	//dynamicDialog->setText(ws.str());
	//dynamicDialog->update(deltaTime);
	for (auto const& control : guiControls)
		control->update(deltaTime);

}



void MainScreen::draw(SpriteBatch* batch) {
	for (auto const& control : guiControls)
		control->draw(batch);
}

void MainScreen::controllerRemoved(ControllerSocketNumber controllerSlot, PlayerSlotNumber slotNumber) {
}

void MainScreen::newController(shared_ptr<Joystick> newStick) {
}




/** **** ConfigScreen **** **/
ConfigScreen::ConfigScreen(MenuManager* mngr) : MenuScreen(mngr) {

}

ConfigScreen::~ConfigScreen() {
	adapterListbox = NULL;
	displayListbox = NULL;
	displayModeCombobox = NULL;
	testSpinner = NULL;
}

int MARGIN = 10;
int itemHeight = 32;
bool ConfigScreen::initialize(ComPtr<ID3D11Device> device, shared_ptr<MouseController> mouse) {

	Vector2 controlPos = Vector2(50, 50);
	 //Labels for displaying pressed info
	adapterLabel = guiFactory->createTextLabel(controlPos, L"Test");
	adapterLabel->setHoverable(true);
	guiControls.push_back(adapterLabel);

	controlPos.y += adapterLabel->getHeight() + MARGIN;

	// create listbox of gfx cards
	adapterListbox = guiFactory->createListBox(controlPos, 400, itemHeight);
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
	adapterListbox->setActionListener(onClickAdapterList);


	adapterLabel->setText(adapterListbox->getSelected()->toString());


	controlPos.y += adapterListbox->getHeight() + MARGIN * 2;
	displayLabel = guiFactory->createTextLabel(controlPos, L"A");
	guiControls.push_back(displayLabel);

	controlPos.y += displayLabel->getHeight() + MARGIN;

	// create listbox of monitors available to pressed gfx card
	displayListbox = guiFactory->createListBox(controlPos, 400, itemHeight);
	guiControls.push_back(displayListbox);
	// because only the one adapter has displays on my laptop
	// this has to be grab the first (and only) display.
	populateDisplayList(game->getDisplayListFor(0));
	displayListbox->setSelected(game->getSelectedDisplayIndex());

	displayLabel->setText(displayListbox->getSelected()->toString());
	displayLabel->setHoverable(true);
	// setup label for Display Mode

	controlPos.y += 50;

	testSpinner = guiFactory->createSpinner(controlPos, 25, itemHeight);
	vector<wstring> items;
	for (ComPtr<IDXGIAdapter> adap : game->getAdapterList()) {
		DXGI_ADAPTER_DESC desc;
		ZeroMemory(&desc, sizeof(DXGI_ADAPTER_DESC));
		adap->GetDesc(&desc);
		wostringstream wss;
		wss << desc.Description;
		items.push_back(wss.str());
	}

	testSpinner->addItems(items);
	guiControls.push_back(testSpinner);


	 //Setup display mode combobox
	controlPos.x += displayListbox->getWidth() + MARGIN * 2;
	controlPos.y = 50;
	 //custom scrollbar for combo list
	ScrollBarDesc scrollBarDesc;
	scrollBarDesc.upButtonImage = "ScrollBar Up Custom";
	scrollBarDesc.upPressedButtonImage = "ScrollBar Up Pressed Custom";
	//scrollBarDesc.downButtonImage = "ScrollBar Down";
	scrollBarDesc.trackImage = "ScrollBar Track Custom";
	scrollBarDesc.scrubberImage = "Scrubber Custom";

	displayModeCombobox =
		guiFactory->createComboBox(controlPos, 75, itemHeight, 10, true);
		//guiFactory->createListBox(controlPos, 75, itemHeight);

	populateDisplayModeList(game->getDisplayModeList(0));
	//displayModeCombobox->setScrollBar(scrollBarDesc);
	displayModeCombobox->setSelected(game->getSelectedDisplayModeIndex());
	OnClickListenerDisplayModeList* onClickDisplayMode =
		new OnClickListenerDisplayModeList(this);
	//displayModeCombobox->setActionListener(onClickDisplayMode);
	guiControls.push_back(displayModeCombobox);




	// Set up CheckBox for full-screen toggle
	CheckBox* check = guiFactory->createCheckBox(Vector2(50, 450), L"Full Screen");
	OnClickListenerFullScreenCheckBox* onClickFullScreen
		= new OnClickListenerFullScreenCheckBox(this);
	check->setActionListener(onClickFullScreen);
	check->setChecked(Globals::FULL_SCREEN);

	guiControls.push_back(check);

	testLabel = guiFactory->createTextLabel(
		Vector2(250, 450), L"Test Messages here");
	guiControls.push_back(testLabel);

	// Create Apply and Cancel Buttons
	ImageButton* button = (ImageButton*) guiFactory->
		createImageButton("Button Up", "Button Down");
	button->setText(L"Back");
	button->setPosition(
		Vector2(Globals::WINDOW_WIDTH / 2 - button->getWidth(),
			Globals::WINDOW_HEIGHT - button->getHeight() - 25));
	button->setActionListener(new BackButtonListener(this));
	guiControls.push_back(button);

	button = (ImageButton*) guiFactory->createImageButton("Button Up", "Button Down");
	button->setText(L"Apply");
	button->setPosition(
		Vector2(Globals::WINDOW_WIDTH / 2,
			Globals::WINDOW_HEIGHT - button->getHeight() - 25));
	guiControls.push_back(button);

	texturePanel.reset(guiFactory->createPanel());


	return true;
}

void ConfigScreen::reloadGraphicsAssets() {
	MenuScreen::reloadGraphicsAssets();
	refreshTexture = true;
}


void ConfigScreen::update(double deltaTime) {

	auto state = Keyboard::Get().GetState();
	keyTracker.Update(state);

	if (keyTracker.IsKeyReleased(Keyboard::Escape)) {
		menuManager->openMainMenu();
	}


	for (auto const& control : guiControls) {
		if (control->update(deltaTime))
			refreshTexture = true;
	}

	if (refreshTexture) {
		refreshTexture = false;
		texturePanel->setTexture(guiFactory->createTextureFromScreen(this));
	}
}

void ConfigScreen::draw(SpriteBatch* batch) {
	texturePanel->draw(batch);

}

void ConfigScreen::textureDraw(SpriteBatch* batch) {
	for (auto const& control : guiControls)
		control->draw(batch);
}


void ConfigScreen::controllerRemoved(ControllerSocketNumber controllerSlot, PlayerSlotNumber slotNumber) {
}

void ConfigScreen::newController(shared_ptr<Joystick> newStick) {
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
		/*if (!Globals::FULL_SCREEN && mode.Scaling == 0)
			continue;*/
		DisplayModeItem* item = new DisplayModeItem();
		item->modeDesc = mode;
		displayModeItems.push_back(item);
	}
	displayModeCombobox->addItems(displayModeItems);

	displayModeItems.clear();

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
	config->game->setAdapter(selectedIndex); // ALMOST WORKS :)
	config->game->reloadGraphicsAssets();
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

		config->populateDisplayModeList(config->game->getDisplayModeList(0));
		//config->displayModeCombobox->setSelected(config->game->getSelectedDisplayModeIndex());
	}
}

void OnClickListenerSettingsButton::onClick(Button* button) {

	main->menuManager->openConfigMenu();
}

void OnClickListenerSettingsButton::onPress(Button * button) {
}

void OnClickListenerSettingsButton::onHover(Button * button) {
}

void OnClickListenerDialogQuitButton::onClick(Button* button) {
	main->exitDialog->hide();
	main->confirmExit();
}

void OnClickListenerDialogQuitButton::onPress(Button * button) {
}

void OnClickListenerDialogQuitButton::onHover(Button * button) {
}

void OnClickListenerExitButton::onClick(Button* button) {
	main->menuManager->exitDialog->show();
}

void OnClickListenerExitButton::onPress(Button * button) {
}

void OnClickListenerExitButton::onHover(Button * button) {
}

void BackButtonListener::onClick(Button* button) {
	configScreen->menuManager->openMainMenu();
}

void BackButtonListener::onPress(Button * button) {
}

void BackButtonListener::onHover(Button * button) {
}


void TestButtonActionListener::onClick(Button* button) {
}

void TestButtonActionListener::onPress(Button* button) {
}

void TestButtonActionListener::onHover(Button* button) {
}
