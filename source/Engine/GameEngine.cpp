#include "GameEngine.h"

unique_ptr<Dialog> GameEngine::errorDialog;
unique_ptr<Dialog> GameEngine::warningDialog;
Dialog* GameEngine::showDialog = NULL;

GameEngine::GameEngine() {
}


GameEngine::~GameEngine() {

	if (audioEngine != NULL)
		audioEngine->Suspend();
}


bool GameEngine::initEngine(HWND hw, HINSTANCE hInstance) {

	hwnd = hw;

	if (!initD3D(hwnd)) {
		MessageBox(0, L"Direct3D Initialization Failed", L"Error", MB_OK);
		return false;
	}

	if (!initRawInput(hwnd)) {
		MessageBox(0, L"Raw Input Init failed", L"Error", MB_OK);
		return false;
	}

	// Initialize Audio Engine
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	AUDIO_ENGINE_FLAGS audioFlags = AudioEngine_Default;
//#ifdef _DEBUG
//	audioFlags = audioFlags | AudioEngine_Debug;
//#endif
	audioEngine.reset(new AudioEngine(audioFlags));
	retryAudio = false;

	if (!audioEngine->IsAudioDevicePresent()) {
		// no audio device found. Operating in silent mode.

	}

	if (!initStage()) {
		MessageBox(0, L"Stage Initialization Failed", L"Error", MB_OK);
		return false;
	}


	return true;
}

void GameEngine::onAudioDeviceChange() {
	retryAudio = true;
}


class QuitButtonListener : public Button::OnClickListener {
public:
	QuitButtonListener(GameEngine* eng) : engine(eng) {
	}
	virtual void onClick(Button * button) override {
		engine->exit();
	}

	GameEngine* engine;
};

bool GameEngine::initStage() {


	game.reset(new GameManager(this));
	if (!game->initializeGame(hwnd, device, mouse.get())) {
		MessageBox(0, L"Game Manager failed to load.", L"Critical Failure", MB_OK);
		return false;
	}

	errorDialog.reset(GameManager::guiFactory->createDialog());
	Vector2 dialogPos, dialogSize;
	dialogSize = Vector2(Globals::WINDOW_WIDTH / 2, Globals::WINDOW_HEIGHT / 2);
	dialogPos = dialogSize;
	dialogPos.x -= dialogSize.x / 2;
	dialogPos.y -= dialogSize.y / 2;
	errorDialog->setDimensions(dialogPos, dialogSize);
	errorDialog->setTint(Color(0, 120, 207));
	unique_ptr<Button> quitButton;
	quitButton.reset(GameManager::guiFactory->createButton());
	quitButton->setText(L"Exit Program");
	quitButton->setOnClickListener(new QuitButtonListener(this));
	errorDialog->setCancelButton(move(quitButton));

	ScrollBarDesc scrollBarDesc;
	scrollBarDesc.upButtonImage = "ScrollBar Up Custom";
	scrollBarDesc.upPressedButtonImage = "ScrollBar Up Pressed Custom";
	scrollBarDesc.trackImage = "ScrollBar Track Custom";
	scrollBarDesc.scrubberImage = "Scrubber Custom";
	warningDialog.reset(GameManager::guiFactory->createDialog(true));

	warningDialog->setDimensions(dialogPos, dialogSize);
	warningDialog->setScrollBar(scrollBarDesc);
	warningDialog->setTint(Color(0, 120, 207));
	warningDialog->setCancelButton(L"Continue");
	unique_ptr<Button> quitButton2;
	quitButton2.reset(GameManager::guiFactory->createButton());
	quitButton2->setText(L"Exit Program");
	quitButton2->setOnClickListener(new QuitButtonListener(this));
	warningDialog->setConfirmButton(move(quitButton2));

	showDialog = warningDialog.get();
	return true;
}

bool warningCanceled = false;
void GameEngine::run(double deltaTime, int fps) {

	update(deltaTime);
	render(deltaTime);
	if (!audioEngine->IsAudioDevicePresent() && !warningCanceled) {
		// no audio device found. Operating in silent mode.
		showWarningDialog(L"No audio device found. Operating in Silent Mode.\nEnd Message...",
			L"Audio Engine failure");
		warningCanceled = true;
	}

	if (retryAudio) {
		retryAudio = false;
		if (audioEngine->Reset()) {
			// restart looped sounds
		}
	} else if (!audioEngine->Update()) {
		if (audioEngine->IsCriticalError()) {
			//ErrorDialog(L"Audio device lost!", L"Audio Engine failure");
			retryAudio = true;
		}
	}
}

void GameEngine::update(double deltaTime) {

	mouse->saveMouseState();
	if (showDialog->isOpen)
		showDialog->update(deltaTime);
	else
		game->update(deltaTime, mouse.get());
}


#include "CommonStates.h"
void GameEngine::render(double deltaTime) {

	deviceContext->ClearRenderTargetView(renderTargetView.Get(), Colors::PeachPuff);
	/*CommonStates blendState(device.Get());*/
	batch->Begin(SpriteSortMode_Deferred/*, blendState.NonPremultiplied()*/);
	{
		game->draw(batch.get());
		showDialog->draw(batch.get());
		mouse->draw(batch.get());
	}
	batch->End();


	swapChain->Present(0, 0);
}

void GameEngine::suspend() {

	stopFullScreen();
	if (game != NULL)
		game->pause();
	if (audioEngine != NULL)
		audioEngine->Suspend();
}

void GameEngine::resume() {

	setFullScreen(Globals::FULL_SCREEN);
	if (audioEngine != NULL)
		audioEngine->Resume();
}

void GameEngine::exit() {
	if (swapChain.Get() != NULL)
		swapChain->SetFullscreenState(false, NULL);
	DestroyWindow(hwnd);
}


