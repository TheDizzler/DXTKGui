#include "MenuManager.h"
#include "GameManager.h"

MenuManager::MenuManager() {
}

MenuManager::~MenuManager() {
}

void MenuManager::setGameManager(GameManager* gm) {

	game = gm;


}


bool MenuManager::initialize(ID3D11Device* device, MouseController* mouse) {


	const char_t* spriteName = "Mouse Reticle";
	shared_ptr<Sprite> mouseSprite =
		GameManager::guiManager->getSprite(spriteName);
	if (mouseSprite == NULL) {
		wostringstream ws;
		ws << "Cannot find sprite file: " << spriteName;
		MessageBox(0, ws.str().c_str(), L"Critical failure", MB_OK);

		return false;
	}
	mouse->loadTexture(mouseSprite->getTexture(), mouseSprite->getResource());

	mainScreen.reset(new MainScreen(this));
	mainScreen->setGameManager(game);
	if (!mainScreen->initialize(device, mouse))
		return false;

	/*configScreen.reset(new ConfigScreen(this));
	configScreen->setGameManager(game);
	if (!configScreen->initialize(device, mouse))
		return false;*/



	currentScreen = mainScreen.get();
	return true;
}


#include "../globals.h"
void MenuManager::update(double deltaTime,
	KeyboardController* keys, MouseController* mouse) {

	ShowCursor(false);
	Vector2 mousePos = mouse->getPosition();
	if (mousePos.x > Globals::WINDOW_WIDTH) {
		mousePos.x = Globals::WINDOW_WIDTH;
		ShowCursor(true);
	}
	if (mousePos.y > Globals::WINDOW_HEIGHT) {
		mousePos.y = Globals::WINDOW_HEIGHT;
		ShowCursor(true);
	}
	if (mousePos.x < 0) {
		mousePos.x = 0;
		ShowCursor(true);
	}
	if (mousePos.y < 0) {
		mousePos.y = 0;
		ShowCursor(true);
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

	currentScreen = mainScreen.get();
}

void MenuManager::openConfigMenu() {

	currentScreen = configScreen.get();
}


/** **** MenuScreen abstract class **** */
MenuScreen::MenuScreen(MenuManager* mngr) {

	menuManager = mngr;
}

MenuScreen::~MenuScreen() {

// textlabels are unique_ptrs
	/*for each (TextLabel* label in textLabels)
		delete label;

	for (TextButton* button : buttons)
		delete button;

	for (ListBox* list : listBoxes)
		delete list;*/
}

void MenuScreen::setGameManager(GameManager* gmMng) {

	game = gmMng;
}

void MenuScreen::pause() {
	// do nothing??
}


/** **** MainMenuScreen **** **/
MainScreen::MainScreen(MenuManager* mngr) : MenuScreen(mngr) {

	menuFont = game->guiManager->getFont("Arial");
}

MainScreen::~MainScreen() {

	for (GUIControl* cntrl : guiControls)
		delete cntrl;
}


bool MainScreen::initialize(ID3D11Device* device, MouseController* mouse) {


	Button* button;
	button = game->guiManager->createImageButton("Arial", "Button Up", "Button Down");
	button->action = GUIControl::PLAY;
	button->setText("Play");
	button->setPosition(Vector2(Globals::WINDOW_WIDTH / 2, 200));
	guiControls.push_back(button);

	button = game->guiManager->createButton("Bookshelf");
	button->action = GUIControl::SETTINGS;
	button->setText("Settings");
	button->setPosition(Vector2(Globals::WINDOW_WIDTH / 2, 350));
	guiControls.push_back(button);



	button = game->guiManager->createImageButton("Arial", "Button Up", "Button Down");
	button->action = GUIControl::EXIT;
	button->setText("Exit");
	button->setPosition(Vector2(Globals::WINDOW_WIDTH / 2, 500));
	guiControls.push_back(button);


	test = new TextLabel(Vector2(10, 10), menuFont);
	guiControls.push_back(test);

	mouseLabel = new TextLabel(Vector2(10, 100), menuFont);
	guiControls.push_back(mouseLabel);


	/*exitDialog.reset(new Dialog(
		Vector2(Globals::WINDOW_WIDTH / 2, Globals::WINDOW_HEIGHT / 2)));
	if (!exitDialog->initialize(device, Assets::arialFontFile)) {
		MessageBox(0, L"Dialog init failed", L"Error", MB_OK);
		return false;
	}*/




	return true;
}


void MainScreen::update(double deltaTime,
	KeyboardController* keys, MouseController* mouse) {

	wostringstream ws;
	ws << "Mouse: " << mouse->getPosition().x << ", " << mouse->getPosition().y;
	mouseLabel->setText(ws);



	//if (keyboardState[DIK_ESCAPE] && !lastStateDown) {
	if (keys->keyDown[KeyboardController::ESC]
		&& !keys->lastDown[KeyboardController::ESC]) {
		/*if (exitDialog->isOpen)
			exitDialog->close();
		else
			confirmExit();*/
	}

	//lastStateDown = keyboardState[DIK_ESCAPE];


	//if (exitDialog->isOpen) {
	//	exitDialog->update(deltaTime, mouse);
	//	switch (exitDialog->getResult()) {
	//		case Dialog::CONFIRM:
	//			game->exit();
	//			break;
	//		case Dialog::CANCEL:
	//			exitDialog->close();
	//			break;
	//	}

	//} else {
	for (GUIControl* control : guiControls) {
		control->update(deltaTime, mouse);
		if (control->clicked()) {
			//test->setText("Clicked!");
			switch (control->action) {
				case Button::EXIT:
					confirmExit();
					//test->setText("Exit!");
					break;
				case Button::PLAY:
					//test->setText("Play!");
					break;
				case Button::SETTINGS:
					menuManager->openConfigMenu();
					//test->setText("Settings!!");
					break;
			}
		}
	}
//}
}


void MainScreen::draw(SpriteBatch* batch) {

	for (GUIControl* control : guiControls)
		control->draw(batch);

	/*for (auto const& label : textLabels)
		label->draw(batch);

	if (exitDialog->isOpen)
		exitDialog->draw(batch);*/

}


void MainScreen::confirmExit() {

	exitDialog->open();
}



/** **** ConfigScreen **** **/
ConfigScreen::ConfigScreen(MenuManager* mngr) : MenuScreen(mngr) {

	menuFont = game->guiManager->getFont("BlackCloak");
}

ConfigScreen::~ConfigScreen() {
}


bool ConfigScreen::initialize(ID3D11Device* device, MouseController* mouse) {

	// Labels for displaying selected info
	//TextLabel* label = new TextLabel(Vector2(50, 50), menuFont);
	//label->setText(L"test");
	//textLabels.push_back(label);

	//label = new TextLabel(Vector2(475, 50), menuFont);
	//label->setText(L"test 2");
	//textLabels.push_back(label);

	//ListBox* listbox = new ListBox(Vector2(50, 100), 400);
	//listbox->initialize(device, Assets::arialFontFile);

	//vector<ListItem*> adapterItems;
	//for (ComPtr<IDXGIAdapter> adap : game->getAdapterList()) {
	//	AdapterItem* item = new AdapterItem();
	//	item->adapter = adap.Get();
	//	adapterItems.push_back(item);
	//}

	//listbox->addItems(adapterItems);
	//listbox->setSelected(game->getSelectedAdapterIndex());
	//listBoxes.push_back(listbox);

	//textLabels[0]->setText(listBoxes[0]->getSelected()->toString());
	////listItems.clear();


	//// Selected adapter display mode list
	//listbox = new ListBox(Vector2(475, 100), 175);
	//listbox->initialize(device, Assets::arialFontFile);

	//vector<ListItem*> displayModeItems;
	//for (DXGI_MODE_DESC mode : game->getDisplayModeList(game->getSelectedAdapterIndex())) {
	//	DisplayModeItem* item = new DisplayModeItem();
	//	item->modeDesc = mode;
	//	displayModeItems.push_back(item);
	//}
	//listbox->addItems(displayModeItems);
	//listbox->setSelected(game->getSelectedDisplayMode());
	//listBoxes.push_back(listbox);

	//textLabels[1]->setText(listBoxes[1]->getSelected()->toString());


	//TextButton* button = new TextButton();
	//if (!button->load(device, Assets::arialFontFile,
	//	Assets::buttonUpFile, Assets::buttonDownFile))
	//	return false;
	//button->action = Button::ClickAction::CANCEL;
	//button->setText("Back");
	//button->setPosition(
	//	Vector2(Globals::WINDOW_WIDTH / 2 - button->getWidth(),
	//		Globals::WINDOW_HEIGHT - button->getHeight()));
	//buttons.push_back(button);

	//button = new TextButton();
	//if (!button->load(device, Assets::arialFontFile,
	//	Assets::buttonUpFile, Assets::buttonDownFile))
	//	return false;
	//button->action = Button::ClickAction::OK;
	//button->setText("Apply");
	//button->setPosition(
	//	Vector2(Globals::WINDOW_WIDTH / 2 + button->getWidth(),
	//		Globals::WINDOW_HEIGHT - button->getHeight()));
	//buttons.push_back(button);


	return true;
}


void ConfigScreen::update(double deltaTime, KeyboardController* keys,
	MouseController* mouse) {

	//for (TextButton* button : buttons) {
	//	button->update(deltaTime, mouse);
	//	if (button->clicked()) {
	//		//test->setText("Clicked!");
	//		switch (button->action) {
	//			case Button::CANCEL:
	//				menuManager->openMainMenu();
	//				break;
	//		}
	//	}
	//}

	////for (ListBox* listbox : listBoxes) {
	//for (int i = 0; i < listBoxes.size(); ++i) {

	//	if (listBoxes[i]->update(deltaTime, mouse)) {

	//		textLabels[i]->setText(listBoxes[i]->getSelected()->toString());
	//	}
	//}


}

void ConfigScreen::draw(SpriteBatch* batch) {

	/*for (TextButton* button : buttons)
		button->draw(batch);

	for (ListBox* listbox : listBoxes)
		listbox->draw(batch);

	for (TextLabel* label : textLabels)
		label->draw(batch);*/
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
