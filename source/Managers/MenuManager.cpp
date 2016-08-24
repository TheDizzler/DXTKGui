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

	//currentScreen = mainScreen.get();
	// switch screens at next frame
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

}

void MenuScreen::setGameManager(GameManager* gmMng) {

	game = gmMng;
}

void MenuScreen::pause() {
	// do nothing??
}


/** **** MainMenuScreen **** **/
MainScreen::MainScreen(MenuManager* mngr) : MenuScreen(mngr) {

	//menuFont = game->guiFactory->getFont("Arial");
}

MainScreen::~MainScreen() {

}


bool MainScreen::initialize(ComPtr<ID3D11Device> device, MouseController* mouse) {

	Button* button;
	button = GameManager::guiFactory->createImageButton("Button Up", "Button Down");
	Vector2 buttonpos = Vector2((Globals::WINDOW_WIDTH - button->getWidth()) / 2,
		Globals::WINDOW_HEIGHT / 3 - button->getHeight() / 2);
	button->action = GUIControl::PLAY;
	button->setText(L"Play");
	button->setPosition(buttonpos);
	guiControls.push_back(button);

	Vector2 size = Vector2(button->getWidth(), button->getHeight());
	buttonpos.y += 150;
	button = GameManager::guiFactory->createButton(buttonpos, size, L"Settings");
	button->action = GUIControl::SETTINGS;
	guiControls.push_back(button);

	buttonpos.y += 150;
	button = GameManager::guiFactory->createImageButton(
		"Button Up", "Button Down");
	button->action = GUIControl::EXIT;
	button->setText(L"Exit");
	button->setPosition(buttonpos);
	guiControls.push_back(button);


	test = GameManager::guiFactory->createTextLabel(Vector2(10, 10));
	guiControls.push_back(test);

	mouseLabel = GameManager::guiFactory->createTextLabel(Vector2(10, 100));
	guiControls.push_back(mouseLabel);

	{
		exitDialog.reset(GameManager::guiFactory->createDialog("Arial"));
		Vector2 dialogPos, dialogSize;
		dialogSize = Vector2(Globals::WINDOW_WIDTH / 2, Globals::WINDOW_HEIGHT / 2);
		dialogPos = dialogSize;
		dialogPos.x -= dialogSize.x / 2;
		dialogPos.y -= dialogSize.y / 2;
		exitDialog->setDimensions(dialogPos, dialogSize);
		exitDialog->setTint(Color(0, 120, 207));
		exitDialog->setTitle(L"Exit Test?");
		exitDialog->setText(L"Really Quit The Test Project?");

		exitDialog->setConfirmButton(L"Quit");
		exitDialog->setCancelButton(L"Keep Testing!");
	}



	return true;
}


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
		switch (exitDialog->getResult()) {
			case GUIControl::CONFIRM:
				game->exit();
				break;
			case GUIControl::CANCEL:
				exitDialog->close();
				break;
		}

	} else {
		for (GUIControl* control : guiControls) {
			control->update(deltaTime, mouse);
			if (control->clicked()) {
				switch (control->action) {
					case GUIControl::EXIT:
						confirmExit();
						//test->setText("Exit!");
						break;
					case GUIControl::PLAY:
						test->setText("Play!");
						break;
					case GUIControl::SETTINGS:
						menuManager->openConfigMenu();
						//test->setText("Settings!!");
						break;
				}
			}
		}
	}
}


void MainScreen::draw(SpriteBatch* batch) {

	for (GUIControl* control : guiControls)
		control->draw(batch);

	if (exitDialog->isOpen)
		exitDialog->draw(batch);

}


void MainScreen::confirmExit() {

	//exitDialog->open();
	game->exit();
}



/** **** ConfigScreen **** **/
ConfigScreen::ConfigScreen(MenuManager* mngr) : MenuScreen(mngr) {

	//menuFont = game->guiFactory->getFont("BlackCloak");
}

ConfigScreen::~ConfigScreen() {
}


bool ConfigScreen::initialize(ComPtr<ID3D11Device> device, MouseController* mouse) {

	// Labels for displaying selected info
	adapterLabel =
		GameManager::guiFactory->createTextLabel(Vector2(50, 50), L"Test");
	adapterLabel->setHoverable(true);
	guiControls.push_back(adapterLabel);

	displayModeLabel =
		GameManager::guiFactory->createTextLabel(Vector2(475, 50), L"Test2");
	guiControls.push_back(displayModeLabel);


	ListBox* listbox =
		GameManager::guiFactory->createListBox(Vector2(50, 100), 400);

	vector<ListItem*> adapterItems;
	for (ComPtr<IDXGIAdapter> adap : game->getAdapterList()) {
		AdapterItem* item = new AdapterItem();
		item->adapter = adap.Get();
		adapterItems.push_back(item);
	}

	listbox->addItems(adapterItems);
	listbox->setSelected(game->getSelectedAdapterIndex());

	OnClickTest* onClickTest = new OnClickTest();
	onClickTest->config = this;
	listbox->setOnClickFunction(onClickTest);

	guiControls.push_back(listbox);



	adapterLabel->setText(listbox->getSelected()->toString());



	// Selected adapter display mode list
	listbox =
		GameManager::guiFactory->createListBox(Vector2(475, 100), 175);

	vector<ListItem*> displayModeItems;
	for (DXGI_MODE_DESC mode
		: game->getDisplayModeList(game->getSelectedAdapterIndex())) {
		DisplayModeItem* item = new DisplayModeItem();
		item->modeDesc = mode;
		displayModeItems.push_back(item);
	}
	listbox->addItems(displayModeItems);
	listbox->setSelected(game->getSelectedDisplayMode());
	guiControls.push_back(listbox);

	displayModeLabel->setText(listbox->getSelected()->toString());


	ImageButton* button = (ImageButton*) GameManager::guiFactory->
		createImageButton("Button Up", "Button Down");
	button->action = Button::ClickAction::CANCEL;
	button->setText(L"Back");
	button->setPosition(
		Vector2(Globals::WINDOW_WIDTH / 2 - button->getWidth(),
			Globals::WINDOW_HEIGHT - button->getHeight()));
	guiControls.push_back(button);

	button = (ImageButton*) GameManager::guiFactory->
		createImageButton("Button Up", "Button Down");
	button->action = Button::ClickAction::OK;
	button->setText(L"Apply");
	button->setPosition(
		Vector2(Globals::WINDOW_WIDTH / 2 /*+ button->getWidth()*/,
			Globals::WINDOW_HEIGHT - button->getHeight()));
	guiControls.push_back(button);


	return true;
}

void ConfigScreen::testMe() {
	adapterLabel->setText(L"Test Successful!!");
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
				case GUIControl::SELECTION_CHANGED:
					ListBox* listbox = (ListBox*) control;
					//const wchar_t* text = listbox->getSelected()->toString();
					//adapterLabel->setText(text);
					listbox->triggerOnClick();
					
					break;
			}
		}
	}

}

void ConfigScreen::draw(SpriteBatch* batch) {


	for (GUIControl* control : guiControls)
		control->draw(batch);
}



void AdapterItem::setText() {

	DXGI_ADAPTER_DESC desc;
	ZeroMemory(&desc, sizeof(DXGI_ADAPTER_DESC));
	adapter->GetDesc(&desc);
	textLabel->setText(desc.Description);
}

static int num = 0;
void DisplayModeItem::setText() {

	UINT width = modeDesc.Width;
	UINT height = modeDesc.Height;

	wostringstream mode;
	//mode << "Format: " << displayModeList[i].Format;
	mode << num++ << ": " << width << " x " << height;

	textLabel->setText(mode.str());

}

void OnClickTest::onClick(ListItem* selectedItem) {

	wostringstream ws;
	ws << selectedItem->toString() << "\n";
	OutputDebugString(ws.str().c_str());
}
