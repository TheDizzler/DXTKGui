#include "GUIFactory.h"
#include "StringHelper.h"
#include "CommonStates.h"
#include "DDSTextureLoader.h"
#include "GuiAssets.h"

bool GUIFactory::initialized = false;


//GUIFactory::GUIFactory(HWND h) {
//
//	hwnd = h;
//}
//
//GUIFactory::GUIFactory(HWND h, pugi::xml_node guiAssets) {
//	hwnd = h;
//	guiAssetsNode = guiAssets;
//}


GUIFactory::~GUIFactory() {


}

HWND GUIFactory::getHWND() {
	return hwnd;
}

MouseController* GUIFactory::getMouseController() {
	return mouseController;
}


bool GUIFactory::initialize(HWND h, ComPtr<ID3D11Device> dev,
	ComPtr<ID3D11DeviceContext> devCon, ComPtr<IDXGISwapChain> sChain,
	SpriteBatch* sBatch, MouseController* mouse, const char_t* assetManifestFile) {

	hwnd = h;


	// get graphical assets from default file
	unique_ptr<xml_document> docAssMan = make_unique<xml_document>();
	if (!docAssMan->load_file(GUIAssets::assetManifestFile)) {
		StringHelper::reportError(
			L"Could not read AssetManifest file!",
			L"Fatal Read Error!", false);
		return false;
	}
	guiAssetsNode.push_back(docAssMan->child("root").child("gui"));
	docs.push_back(move(docAssMan));

	if (assetManifestFile != NULL) {
		// get graphical assets from custom xml file
		unique_ptr<xml_document> docAssManCustom = make_unique<xml_document>();
		if (!docAssManCustom->load_file(assetManifestFile)) {
			StringHelper::reportError(
				L"Could not read custom AssetManifest file!",
				L"Fatal Read Error!", false);
			return false;
		}

		guiAssetsNode.push_back(docAssManCustom->child("root").child("gui"));
		docs.push_back(move(docAssManCustom));
	}


	device = dev;
	deviceContext = devCon;
	batch = sBatch;

	for (xml_node node : guiAssetsNode) {
		if (!getGUIAssetsFromXML(node)) {
			StringHelper::reportError(L"Asset retrieval from Asset Manifest failed.",
				L"Epic failure", false);
			return false;
		}
	}
	mouseController = mouse;

	initialized = true;
	return true;
}

void GUIFactory::reInitDevice(ComPtr<ID3D11Device> dev,
	ComPtr<ID3D11DeviceContext> devCon, SpriteBatch* sBatch) {

	device.Reset();
	deviceContext->Flush();
	deviceContext.Reset();

	device = dev;
	deviceContext = devCon;
	batch = sBatch;
	assetMap.clear();

	for (xml_node node : guiAssetsNode) {
		if (!getGUIAssetsFromXML(node)) {
			StringHelper::reportError(
				L"Asset retrieval from Asset Manifest failed.",
				L"Epic failure", false);
		}
	}
}



unique_ptr<FontSet> GUIFactory::getFont(const char_t* fontName) {

	if (fontMap.find(fontName) == fontMap.end()) {

		wostringstream ws;
		ws << "Cannot find font file. Using default font instead.\n";
		OutputDebugString(ws.str().c_str());

		unique_ptr<FontSet> defaultFont;
		defaultFont.reset(new FontSet());
		defaultFont->load(device,
			StringHelper::convertCharStarToWCharT(defaultFontFile),
			"Default Font");

		return move(defaultFont);
	}

	const char_t* fontFile = fontMap[fontName].c_str();

	unique_ptr<FontSet> font;
	font.reset(new FontSet());
	font->load(device,
		StringHelper::convertCharStarToWCharT(fontFile),
		fontName);
	return move(font);
}


GraphicsAsset* const GUIFactory::getAsset(const char_t* assetName) {

	if (assetMap.find(assetName) == assetMap.end()) {
		wostringstream ws;
		ws << "Cannot find asset file: " << assetName << "\n";
		OutputDebugString(ws.str().c_str());
		return NULL;
	}

	return assetMap[assetName].get();
}


AssetSet* const GUIFactory::getAssetSet(const char_t* setName) {

	if (setMap.find(setName) == setMap.end()) {
		wostringstream ws;
		ws << "Cannot find asset set: " << setName << "\n";
		OutputDebugString(ws.str().c_str());
		return NULL;
	}

	return setMap[setName].get();
}


unique_ptr<Sprite> GUIFactory::getSpriteFromAsset(const char_t* assetName) {

	GraphicsAsset* const asset = getAsset(assetName);
	if (asset == NULL)
		return NULL;
	unique_ptr<Sprite> sprite;
	sprite.reset(new Sprite());
	sprite->load(asset);
	return move(sprite);
}

Animation* GUIFactory::getAnimation(const char_t* animationName) {

	if (animationMap.find(animationName) == animationMap.end()) {
		wostringstream ws;
		ws << "Cannot find asset file: " << animationName << "\n";
		ws << "Count : " << animationMap.count(animationName) << "\n";
		OutputDebugString(ws.str().c_str());
		return NULL;
	}

	return animationMap[animationName].get();
}


Line* GUIFactory::createLine(const Vector2& position, const Vector2& size, Color lineColor) {

	Line* line = new Line(getAsset("White Pixel"), position, size);
	line->setTint(lineColor);

	return line;
}

Line* GUIFactory::createLineBetween(const Vector2 pointA, const Vector2 pointB, Color lineColor) {

	Vector2 v = pointB - pointA;
	float angle = atan2(v.y, v.x);
	float length = Vector2::Distance(pointA, pointB);
	Line* line = new Line(getAsset("White Pixel"), pointA, Vector2(length, 2));
	line->setTint(lineColor);
	line->setRotation(angle);
	return line;
}

RectangleSprite* GUIFactory::createRectangle(
	const Vector2& position, const Vector2& size, Color color) {

	RectangleSprite* rect = new RectangleSprite(getAsset("White Pixel"), color);
	rect->setDimensions(position, size);

	return rect;
}

RectangleFrame* GUIFactory::createRectangleFrame(const Vector2& position,
	const Vector2& size, USHORT frameThickness, Color frameColor) {

	RectangleFrame* frame = new RectangleFrame(getAsset("White Pixel"), this);
	frame->setDimensions(position, size, frameThickness);
	frame->setTint(frameColor, false);

	return frame;
}

TriangleFrame* GUIFactory::createTriangleFrame(const Vector2& pt1, const Vector2& pt2,
	const Vector2& pt3, USHORT thickness) {

	TriangleFrame* triangle = new TriangleFrame(getAsset("White Pixel"));
	triangle->setDimensions(pt1, pt2, pt3, thickness);

	return triangle;
}


TextLabel* GUIFactory::createTextLabel(const Vector2& position,
	wstring text, const char_t* fontName, bool useTexture) {

	TextLabel* label = new TextLabel(this, mouseController,
		position, text, fontName, useTexture);
	return label;
}

LetterJammer* GUIFactory::createLetterJammer(
	const Vector2& position, wstring text, Color textColor,
	bool autoRun, const char_t* fontName) {

	LetterJammer* jammer = new LetterJammer(
		this, mouseController, position, text, autoRun, fontName);
	jammer->setTint(textColor);
	return jammer;
}


Button* GUIFactory::createButton(const char_t* fontName) {

	Button* button = new Button(this, mouseController, fontName);

	return button;
}


Button* GUIFactory::createButton(const Vector2& position, const Vector2& size,
	wstring text, const char_t* fontName, const int frameThickness) {

	Button* button = new Button(this, mouseController, fontName);

	button->setDimensions(position, size, frameThickness);
	button->setText(text);
	return button;
}

/* Creates an image button with only one image. */
Button* GUIFactory::createOneImageButton(const char_t* buttonImage, const char_t* fontName) {

	if (assetMap.find(buttonImage) == assetMap.end()) {

		wostringstream ws;
		ws << "Could not find button image file ";
		ws << buttonImage;
		ws << "\nCreating non-image button instead." << "\n";
		OutputDebugString(ws.str().c_str());

		Button* button = createButton(fontName);
		button->setDimensions(Vector2::Zero, Vector2(80, 68));
		button->setText(L"No Image!");
		return button;

	}

	ImageButton* button = new ImageButton(this, mouseController,
		getSpriteFromAsset(buttonImage), fontName);
	return button;
}

Button* GUIFactory::createImageButton(const char_t* upImageName,
	const char_t* downImageName, const char_t* fontName) {

	if (downImageName == NULL || assetMap.find(downImageName) == assetMap.end()) {

		wostringstream ws;
		ws << "Could not find downImageName file ";
		ws << "in createImageButton()";
		ws << "\nCreating one image button instead." << "\n";
		OutputDebugString(ws.str().c_str());
		return createOneImageButton(upImageName, fontName);
	}

	if (assetMap.find(upImageName) == assetMap.end()) {

		wostringstream ws;
		ws << "Could not find upImageName image file: ";
		ws << upImageName;
		ws << "\nCreating non-image button instead." << "\n";
		OutputDebugString(ws.str().c_str());

		Button* button = createButton(fontName);
		button->setDimensions(Vector2::Zero, Vector2(80, 68)); // just some random
																// defaults to be ugly
		button->setText(L"No Image!");
		return button;
	}

	ImageButton* button = new ImageButton(this, mouseController,
		getSpriteFromAsset(upImageName), getSpriteFromAsset(downImageName), fontName);

	return button;
}

Button* GUIFactory::createImageButton(const Vector2& position,
	const char_t* upImage, const char_t* downImage, const char_t* fontName) {

	ImageButton* button = (ImageButton*) createImageButton(upImage, downImage, fontName);
	button->setPosition(position);
	return button;
}

Button* GUIFactory::createImageButton(unique_ptr<Sprite> upSprite, const char_t* fontName) {

	ImageButton* button = new ImageButton(this,
		mouseController, move(upSprite), fontName);
	return button;
}


CheckBox* GUIFactory::createCheckBox(const Vector2& position,
	wstring text, const char_t* fontName) {

	CheckBox* check = new CheckBox(this, mouseController,
		getSpriteFromAsset("CheckBox Unchecked"),
		getSpriteFromAsset("CheckBox Checked"), fontName);

	check->setText(text);
	check->setPosition(position);
	return check;
}

CheckBox* GUIFactory::createCheckBox(const Vector2& position,
	const char_t* uncheckedImage, const char_t* checkedImage,
	wstring text, const char_t* fontName) {

	if (assetMap.find(uncheckedImage) == assetMap.end()
		|| assetMap.find(checkedImage) == assetMap.end()) {

		wostringstream ws;
		ws << "Could not find button image files: ";
		ws << uncheckedImage << " and/or " << checkedImage;
		ws << "\nCreating default checkbox instead." << "\n";
		OutputDebugString(ws.str().c_str());

		return createCheckBox(position, text, fontName);
	}

	CheckBox* check = new CheckBox(this, mouseController,
		getSpriteFromAsset(uncheckedImage), getSpriteFromAsset(checkedImage),
		fontName);
	check->setText(text);
	check->setPosition(position);
	return check;

}


Spinner* GUIFactory::createSpinner(const Vector2& position, const size_t width,
	const size_t itemHeight, bool autoSize, const char_t* upButtonAsset,
	const char_t* downButtonAsset, const char_t* fontName) {

	Spinner* spinner = new Spinner(this, mouseController, position, width, itemHeight, autoSize);
	spinner->initialize(fontName, upButtonAsset, downButtonAsset);
	return spinner;
}


ScrollBar* GUIFactory::createScrollBar(const Vector2& position, size_t barHeight) {

	ScrollBar* scrollBar = new ScrollBar(this, mouseController, position);

	if (!scrollBar->initialize(getAsset("White Pixel"), barHeight)) {

		MessageBox(NULL, L"Failed to create ScrollBar",
			L"GUI initialization ERROR", MB_OK);
	}

	return scrollBar;
}


ScrollBar* GUIFactory::createScrollBar(const Vector2& position, size_t barHeight,
	ScrollBarDesc& scrollBarDesc) {

	ScrollBar* scrollBar = new ScrollBar(this, mouseController, position);

	ImageButton* buttons[2] = {NULL, NULL};
	if (scrollBarDesc.upPressedButtonImage == "")
		buttons[0] = (ImageButton*) createOneImageButton(scrollBarDesc.upButtonImage.c_str());
	else
		buttons[0] = (ImageButton*) createImageButton(scrollBarDesc.upButtonImage.c_str(),
			scrollBarDesc.upPressedButtonImage.c_str());

	if (scrollBarDesc.downButtonImage == "") { // copy the up imagebutton and reverse
		if (scrollBarDesc.upPressedButtonImage == "")
			buttons[1] = (ImageButton*) createOneImageButton(scrollBarDesc.upButtonImage.c_str());
		else
			buttons[1] = (ImageButton*) createImageButton(scrollBarDesc.upButtonImage.c_str(),
				scrollBarDesc.upPressedButtonImage.c_str());
		buttons[1]->setRotation(XM_PI);
	} else {
		if (scrollBarDesc.downPressedButtonImage == "")
			buttons[1] = (ImageButton*) createOneImageButton(scrollBarDesc.downButtonImage.c_str());
		else
			buttons[1] = (ImageButton*) createImageButton(scrollBarDesc.downButtonImage.c_str(),
				scrollBarDesc.downPressedButtonImage.c_str());
	}

	unique_ptr<Sprite> scrollBarTrack;
	if (scrollBarDesc.trackImage != "") {
		scrollBarTrack = getSpriteFromAsset(scrollBarDesc.trackImage.c_str());
	}

	GraphicsAsset* scrollBarScrubber = NULL;
	if (scrollBarDesc.scrubberImage != "")
		scrollBarScrubber = getAsset(scrollBarDesc.scrubberImage.c_str());

	if (!scrollBar->initialize(getAsset("White Pixel"), barHeight, buttons,
		move(scrollBarTrack), scrollBarScrubber)) {

		MessageBox(NULL, L"Failed to create ScrollBar",
			L"GUI initialization ERROR", MB_OK);
	}

	return scrollBar;
}

TexturePanel* GUIFactory::createPanel(bool neverShowScrollBar) {

	ScrollBar* scrllbar = NULL;
	if (!neverShowScrollBar)
		scrllbar = createScrollBar(Vector2::Zero, 10);
	TexturePanel* panel = new TexturePanel(this, mouseController, scrllbar);
	return panel;
}


ListBox* GUIFactory::createListBox(const Vector2& position,
	const int width, const int itemHeight, const int maxItemsShown,
	bool enumerateList, const char_t* fontName, const int frameThickness) {


	ListBox* listbox = new ListBox(this, mouseController,
		position, width, itemHeight, maxItemsShown);

	Vector2 scrollBarPos(position.x + width, position.y);
	listbox->initialize(fontName, getAsset("White Pixel"),
		createScrollBar(scrollBarPos, itemHeight * maxItemsShown), enumerateList,
		frameThickness);

	return listbox;
}


ComboBox* GUIFactory::createComboBox(const Vector2& position,
	const int width, const int itemHeight, const int maxItemsShown,
	bool enumerateList, const char_t* buttonAsset, const char_t* fontName,
	const int frameThickness) {

	ComboBox* combobox = new ComboBox(this, mouseController,
		position, width, itemHeight, maxItemsShown);

	combobox->initialize(fontName,
		createListBox(
			Vector2(position.x, position.y + itemHeight),
			width, itemHeight, maxItemsShown, enumerateList, fontName, frameThickness),
		buttonAsset, enumerateList, frameThickness);

	return combobox;
}

PromptDialog* GUIFactory::createDialog(const Vector2& position, const Vector2& size,
	bool movable, bool centerText, int frameThickness, const char_t* fontName) {

	PromptDialog* dialog = new PromptDialog(this, mouseController,
		hwnd, movable, centerText);

	dialog->initialize(fontName);
	dialog->setDimensions(position, size, frameThickness);
	return dialog;
}

DynamicDialog* GUIFactory::createDynamicDialog(const char_t* imageSet,
	const Vector2& position, const Vector2& size, const char_t* fontName) {

	DynamicDialog* dialog = new DynamicDialog(this, mouseController);
	dialog->initialize(getAssetSet(imageSet), fontName);
	dialog->setDimensions(position, size);
	return dialog;
}

DynamicDialog* GUIFactory::createDynamicDialog(AssetSet* dialogImageSet,
	const Vector2& position, const Vector2& size, const char_t* fontName) {

	DynamicDialog* dialog = new DynamicDialog(this, mouseController);
	dialog->initialize(dialogImageSet, fontName);
	dialog->setDimensions(position, size);
	return dialog;
}


int elementCounter = 0;
unique_ptr<GraphicsAsset> GUIFactory::createTextureFromTexturizable(
	Texturizable* control, bool autoBatchDraw, Color bgColor) {

	int buffer = 45; // padding to give a bit of lee-way to prevent tearing

	RECT rect;
	GetClientRect(hwnd, &rect);

	int screenWidth = rect.right - rect.left + buffer;
	int screenHeight = rect.bottom - rect.top + buffer;

	/*wostringstream wss;
	wss << "W: " << rect.right - rect.left << " H: " << rect.bottom - rect.top << endl;
	OutputDebugString(wss.str().c_str());*/

	int width = control->getWidth();
	int height = control->getHeight();
	int heightPadding = 0;
	int widthPadding = 0;
	float ratio = (float) screenWidth / screenHeight;
	if (width > height) {
		heightPadding = INT(width / ratio) - height;
		height = INT(width / ratio);
	} else {
		widthPadding = INT(height*ratio) - width;
		width = INT(height * ratio);
	}
	width += buffer;
	height += buffer;

	ComPtr<ID3D11Texture2D> renderTargetTexture;
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;



	if (StringHelper::reportError(device->CreateTexture2D(&textureDesc, NULL,
		renderTargetTexture.GetAddressOf()),
		L"Failed to create render target texture.", L"Aw shucks"))
		return NULL;

	ComPtr<ID3D11RenderTargetView> textureRenderTargetView;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;


	if (StringHelper::reportError(
		device->CreateRenderTargetView(renderTargetTexture.Get(),
			NULL, textureRenderTargetView.GetAddressOf()),
		L"Failed to create render target view for new texture.", L"Fatal Error"))
		return NULL;



	ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;



	if (StringHelper::reportError(
		device->CreateShaderResourceView(renderTargetTexture.Get(),
			&shaderResourceViewDesc, shaderResourceView.GetAddressOf()),
		L"Failed to create Shader Resource View for new texture.", L"Fatal error"))
		return NULL;


	// get normal rendertargetview and switch to temp one
	ComPtr<ID3D11RenderTargetView> oldRenderTargetView;
	deviceContext->OMGetRenderTargets(1, oldRenderTargetView.GetAddressOf(), nullptr);
	ID3D11RenderTargetView* nullViews[] = {nullptr};
	deviceContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
	deviceContext->Flush();
	deviceContext->OMSetRenderTargets(1, textureRenderTargetView.GetAddressOf(), nullptr);



	Vector2 oldPos = control->getPosition();
	//Vector2 oldSize = Vector2(control->getWidth(), control->getHeight());
	control->setPosition(Vector2(0, 0));

	deviceContext->ClearRenderTargetView(textureRenderTargetView.Get(), bgColor);


	/** Create a new viewport incase the current one is not normal.*/
	/*const UINT MAX_VIEWPORTS = 16;
	UINT numViewports = MAX_VIEWPORTS;
	D3D11_VIEWPORT oldViewports[MAX_VIEWPORTS];
	deviceContext->RSGetViewports(&numViewports, oldViewports);
	D3D11_VIEWPORT textureViewport;
	ZeroMemory(&textureViewport, sizeof(D3D11_VIEWPORT));
	textureViewport.TopLeftX = 0;
	textureViewport.TopLeftY = 0;
	textureViewport.Width = screenWidth;
	textureViewport.Height = screenHeight;
	textureViewport.MinDepth = 0.0f;
	textureViewport.MaxDepth = 1.0f;
	deviceContext->RSSetViewports(1, &textureViewport);
	batch->SetViewport(textureViewport);*/

	if (autoBatchDraw) {
		/* This is supposed to be the bare minimum. If you need more you should
			set autoBatchDraw to false and override the objects textureDraw().*/
		batch->Begin(SpriteSortMode_Deferred);
		{
			control->textureDraw(batch);
		}
		batch->End();
	} else
		control->textureDraw(batch, device);

	deviceContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
	textureRenderTargetView.Reset();
	deviceContext->Flush();
	deviceContext->OMSetRenderTargets(1, oldRenderTargetView.GetAddressOf(), nullptr);
	//deviceContext->RSSetViewports(numViewports, oldViewports);
	//batch->SetViewport(oldViewports[0]);

	textureRenderTargetView.Reset();
	oldRenderTargetView.Reset();

	control->setPosition(oldPos);
	unique_ptr<GraphicsAsset> gfxAsset = make_unique<GraphicsAsset>();

	string name = "Texturized Control #" + to_string(elementCounter++);

	gfxAsset->loadAsPartOfSheet(shaderResourceView, name.c_str(), Vector2::Zero,
		Vector2(FLOAT(width - widthPadding - buffer), FLOAT(height - heightPadding - buffer)), Vector2::Zero);

	shaderResourceView.Reset();
	return move(gfxAsset);
}

int screenCounter = 0;
unique_ptr<GraphicsAsset> GUIFactory::createTextureFromScreen(
	Screen* screen, bool autoBatchDraw, Color bgColor) {

	int buffer = 5; // padding to give a bit of lee-way to prevent tearing


	RECT rect;
	GetClientRect(hwnd, &rect);

	int screenWidth = rect.right - rect.left + buffer;
	int screenHeight = rect.bottom - rect.top + buffer;

	ComPtr<ID3D11Texture2D> renderTargetTexture;
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Width = screenWidth;
	textureDesc.Height = screenHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;



	if (StringHelper::reportError(device->CreateTexture2D(&textureDesc, NULL,
		renderTargetTexture.GetAddressOf()),
		L"Failed to create render target texture.", L"Aw shucks"))
		return NULL;

	ComPtr<ID3D11RenderTargetView> textureRenderTargetView;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;


	if (StringHelper::reportError(
		device->CreateRenderTargetView(renderTargetTexture.Get(),
			NULL, textureRenderTargetView.GetAddressOf()),
		L"Failed to create render target view for new texture.", L"Fatal Error"))
		return NULL;



	ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;



	if (StringHelper::reportError(
		device->CreateShaderResourceView(renderTargetTexture.Get(),
			&shaderResourceViewDesc, shaderResourceView.GetAddressOf()),
		L"Failed to create Shader Resource View for new texture.", L"Fatal error"))
		return NULL;




	// get normal rendertargetview and switch to temp one
	ComPtr<ID3D11RenderTargetView> oldRenderTargetView;
	deviceContext->OMGetRenderTargets(1, oldRenderTargetView.GetAddressOf(), nullptr);
	ID3D11RenderTargetView* nullViews[] = {nullptr};
	deviceContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
	deviceContext->Flush();
	deviceContext->OMSetRenderTargets(1, textureRenderTargetView.GetAddressOf(), nullptr);


	deviceContext->ClearRenderTargetView(textureRenderTargetView.Get(), bgColor);


	/** Create a new viewport incase the current one is not normal.*/
	const UINT MAX_VIEWPORTS = 16;
	UINT numViewports = MAX_VIEWPORTS;
	D3D11_VIEWPORT oldViewports[MAX_VIEWPORTS];
	deviceContext->RSGetViewports(&numViewports, oldViewports);
	D3D11_VIEWPORT textureViewport;
	ZeroMemory(&textureViewport, sizeof(D3D11_VIEWPORT));
	textureViewport.TopLeftX = 0;
	textureViewport.TopLeftY = 0;
	textureViewport.Width = (float) screenWidth;
	textureViewport.Height = (float) screenHeight;
	textureViewport.MinDepth = 0.0f;
	textureViewport.MaxDepth = 1.0f;
	deviceContext->RSSetViewports(1, &textureViewport);
	batch->SetViewport(textureViewport);


	if (autoBatchDraw) {
		batch->Begin(SpriteSortMode_Deferred);
		{
			screen->textureDraw(batch);
		}
		batch->End();
	} else
		screen->textureDraw(batch);

	deviceContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
	textureRenderTargetView.Reset();
	deviceContext->Flush();
	deviceContext->OMSetRenderTargets(1, oldRenderTargetView.GetAddressOf(), nullptr);
	deviceContext->RSSetViewports(numViewports, oldViewports);
	batch->SetViewport(oldViewports[0]);

	string name = "Texturized Screen #" + to_string(screenCounter++);
	unique_ptr<GraphicsAsset> gfxAsset = make_unique<GraphicsAsset>();
	gfxAsset->loadAsPartOfSheet(shaderResourceView, name.c_str(), Vector2::Zero,
		Vector2((float) screenWidth - buffer, (float) screenHeight - buffer), Vector2::Zero);

	shaderResourceView.Reset();
	return move(gfxAsset);
}



bool GUIFactory::getGUIAssetsFromXML(xml_node assetNode) {

	string assetsDir =
		assetNode.parent().attribute("dir").as_string();

	string guiDir =
		assetsDir + assetNode.attribute("dir").as_string();


	xml_node fonts = assetNode.child("spritefonts");
	string fontDir = assetsDir + fonts.attribute("dir").as_string();
	for (xml_node fontNode = fonts.child("font"); fontNode;
		fontNode = fontNode.next_sibling("font")) {

		string file_s = fontDir + fontNode.attribute("file").as_string();
		const char_t* file = file_s.c_str();
		const char_t* name = fontNode.attribute("name").as_string();

		fontMap[name] = file;

	}

	defaultFontFile = GUIAssets::defaultFontFile;
	fontMap["Default Font"] = GUIAssets::defaultFontFile;

	for (xml_node spriteNode : assetNode.children("sprite")) {
		string file_s = guiDir + spriteNode.attribute("file").as_string();
		const char_t* file = file_s.c_str();
		const char_t* name = spriteNode.attribute("name").as_string();

		Vector2 origin = Vector2(-1000, -1000);
		xml_node originNode = spriteNode.child("origin");
		if (originNode) {
			origin.x = (float) originNode.attribute("x").as_int();
			origin.y = (float) originNode.attribute("y").as_int();
		}
		unique_ptr<GraphicsAsset> guiAsset;
		guiAsset.reset(new GraphicsAsset());
		if (!guiAsset->load(device, name, StringHelper::convertCharStarToWCharT(file), origin)) {
			wstringstream wss;
			wss << "Unable to load file: " << file;
			MessageBox(hwnd, wss.str().c_str(), L"Critical error", MB_OK);
			return false;
		}

		assetMap[name] = move(guiAsset);
	}

	for (xml_node spritesheetNode : assetNode.children("spritesheet")) {

		string file_s = guiDir + spritesheetNode.attribute("file").as_string();
		const char_t* file = file_s.c_str();
		const char_t* masterAssetName = spritesheetNode.attribute("name").as_string();

		unique_ptr<GraphicsAsset> masterAsset = make_unique<GraphicsAsset>();
		if (!masterAsset->load(device, masterAssetName,
			StringHelper::convertCharStarToWCharT(file), Vector2::Zero))
			return false;


		for (xml_node spritesetNode : spritesheetNode.children("spriteset")) {

			const char_t* setName = spritesetNode.attribute("name").as_string();

			if (spritesetNode.attribute("repeat")) {
				// if repeat
				int rows, cols;
				string repeatStr = spritesetNode.attribute("repeat").as_string();

				string substr = repeatStr.substr(0, 1);
				istringstream(substr) >> cols;
				substr = repeatStr.substr(2, 1);
				istringstream(substr) >> rows;

				int offset = spritesetNode.attribute("offset").as_int();
				int subNum = 0;
				for (int i = 0; i < rows; ++i) {
					for (int j = 0; j < cols; ++j) {
						ostringstream oss;
						oss << setName << " " << subNum;
						string subsetName(oss.str());
						if (setMap.find(subsetName) == setMap.end()) {
							// new set
							setMap[subsetName] = make_unique<AssetSet>(subsetName.c_str());
						}

						for (xml_node spriteNode : spritesetNode.children("sprite")) {
							setMap[subsetName]->addAsset(spriteNode.attribute("name").as_string(),
								parseSprite(spriteNode, masterAsset->getTexture(), offset * j, offset * i));
						}
						++subNum;
					}
				}
			} else {
			// if not repeat
				for (xml_node spriteNode : spritesetNode.children("sprite")) {

					const char_t* spriteName = spriteNode.attribute("name").as_string();
					if (setMap.find(setName) == setMap.end()) {
						// new set
						setMap[setName] = make_unique<AssetSet>(setName);
					}
					setMap[setName]->addAsset(spriteNode.attribute("name").as_string(),
						parseSprite(spriteNode, masterAsset->getTexture()));
				}
			}
		}

		// parse all animations from spritesheet
		for (xml_node animationNode : spritesheetNode.children("animation")) {

			const char_t* name = animationNode.attribute("name").as_string();
			float timePerFrame = animationNode.attribute("timePerFrame").as_float();

			vector<unique_ptr<Frame>> frames;
			for (xml_node spriteNode : animationNode.children("sprite")) {

				RECT rect;
				rect.left = spriteNode.attribute("x").as_int();
				rect.top = spriteNode.attribute("y").as_int();
				rect.right = rect.left + spriteNode.attribute("width").as_int();
				rect.bottom = rect.top + spriteNode.attribute("height").as_int();
				Vector2 origin = Vector2(0, 0);
				xml_node originNode = spriteNode.child("origin");
				if (originNode) {
					origin.x = (float) originNode.attribute("x").as_int();
					origin.y = (float) originNode.attribute("y").as_int();
				}
				unique_ptr<Frame> frame;
				if (spriteNode.attribute("frameTime"))
					frame.reset(new Frame(rect, origin,
						spriteNode.attribute("frameTime").as_float()));
				else
					frame.reset(new Frame(rect, origin, timePerFrame));
				frames.push_back(move(frame));

			}

			unique_ptr<Animation> animationAsset;
			animationAsset.reset(new Animation(masterAsset->getTexture(), move(frames), name));
			animationMap[name] = move(animationAsset);
		}

		// parse all single sprites from spritesheet
		for (xml_node spriteNode : spritesheetNode.children("sprite")) {
			const char_t* name = spriteNode.attribute("name").as_string();
			if (spriteNode.attribute("set")) {
				string setName = spriteNode.attribute("set").as_string();
				if (setMap.find(setName) == setMap.end()) {
					// new set
					setMap[setName] = make_unique<AssetSet>(setName.c_str());
				}
				setMap[setName]->addAsset(name,
					parseSprite(spriteNode, masterAsset->getTexture()));
			} else if (spritesheetNode.attribute("set")) {
				string setName = spritesheetNode.attribute("set").as_string();
				if (setMap.find(setName) == setMap.end()) {
					// new set
					setMap[setName] = make_unique<AssetSet>(setName.c_str());
				}
				setMap[setName]->addAsset(name,
					parseSprite(spriteNode, masterAsset->getTexture()));
			} else
				assetMap[name] = parseSprite(spriteNode, masterAsset->getTexture());
		}


		assetMap[masterAssetName] = move(masterAsset);
	}
	return true;
}

unique_ptr<GraphicsAsset> GUIFactory::parseSprite(xml_node spriteNode,
	ComPtr<ID3D11ShaderResourceView> sheetTexture, int xOffset, int yOffset) {

	const char_t* spritename = spriteNode.attribute("name").as_string();

	// pos in spritesheet
	Vector2 position = Vector2(FLOAT(spriteNode.attribute("x").as_int() + xOffset),
		FLOAT(spriteNode.attribute("y").as_int() + yOffset));
	// dimensions in spritesheet
	Vector2 size = Vector2((float) spriteNode.attribute("width").as_int(),
		(float) spriteNode.attribute("height").as_int());

	Vector2 origin = Vector2(-1000.0f, -1000.0f); // this indicates to GfxAsset that origin should be center
	xml_node originNode = spriteNode.child("origin");
	if (originNode) {
		origin.x = (float) originNode.attribute("x").as_int();
		origin.y = (float) originNode.attribute("y").as_int();
	}

	unique_ptr<GraphicsAsset> spriteAsset = make_unique<GraphicsAsset>();
	spriteAsset->loadAsPartOfSheet(sheetTexture,
		spritename, position, size, origin);
	return move(spriteAsset);
}