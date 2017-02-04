#include "GUIFactory.h"

bool GUIFactory::initialized = false;


GUIFactory::GUIFactory(HWND h) {

	hwnd = h;
}

GUIFactory::GUIFactory(HWND h, pugi::xml_node guiAssets) {
	hwnd = h;
	guiAssetsNode = guiAssets;
}

GUIFactory::~GUIFactory() {
	assetMap.clear();
	fontMap.clear();
}

#include "../GuiAssets.h"
bool GUIFactory::initialize(ComPtr<ID3D11Device> dev,
	ComPtr<ID3D11DeviceContext> devCon, ComPtr<IDXGISwapChain> sChain,
	SpriteBatch* sBatch, shared_ptr<MouseController> mouse, const char_t* assetManifestFile) {


	if (assetManifestFile != NULL) {
	// get graphical assets from xml file
		docAssMan.reset(new pugi::xml_document());
		if (!docAssMan->load_file(assetManifestFile)) {
			MessageBox(0, L"Could not read AssetManifest file!",
				L"Fatal Read Error!", MB_OK);
			return false;
		}

		guiAssetsNode = docAssMan->child("root").child("gui");
	}


	device = dev;
	deviceContext = devCon;
	batch = sBatch;

	if (!getGUIAssetsFromXML()) {
		MessageBox(0, L"Sprite retrieval from Asset Manifest failed.",
			L"Epic failure", MB_OK);
		return false;
	}

	mouseController = mouse;

	initialized = true;
	return true;
}


#include "../StringHelper.h"
#include <sstream>
unique_ptr<FontSet> GUIFactory::getFont(const char_t* fontName) {

	if (fontMap.find(fontName) == fontMap.end()) {

		wostringstream ws;
		ws << "Cannot find font file. Using default font instead.\n";
		OutputDebugString(ws.str().c_str());

		unique_ptr<FontSet> defaultFont;
		defaultFont.reset(new FontSet());
		defaultFont->load(device, StringHelper::convertCharStarToWCharT(defaultFontFile));
		//defaultFont->setTint(Color(1, 1, 1, 1));

		return move(defaultFont);
	}

	const char_t* fontFile = fontMap[fontName].c_str();

	unique_ptr<FontSet> font;
	font.reset(new FontSet());
	font->load(device, StringHelper::convertCharStarToWCharT(fontFile));
	font->setTint(Color(1, 1, 1, 1));
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


unique_ptr<Sprite> GUIFactory::getSpriteFromAsset(const char_t* assetName) {

	GraphicsAsset* const asset = getAsset(assetName);
	if (asset == NULL)
		return NULL;
	unique_ptr<Sprite> sprite;
	sprite.reset(new Sprite());
	sprite->load(asset);
	return move(sprite);
}

shared_ptr<Animation> GUIFactory::getAnimation(const char_t* animationName) {

	if (animationMap.find(animationName) == animationMap.end()) {
		wostringstream ws;
		ws << "Cannot find asset file: " << animationName << "\n";
		ws << "Count : " << animationMap.count(animationName) << "\n";
		OutputDebugString(ws.str().c_str());
		return NULL;
	}

	return animationMap[animationName];
}


Line* GUIFactory::createLine(const Vector2& position, const Vector2& size, Color lineColor) {

	Line* line = new Line(getAsset("White Pixel"), position, size);
	line->setTint(lineColor);

	return line;
}

RectangleSprite* GUIFactory::createRectangle(
	const Vector2& position, const Vector2& size) {

	RectangleSprite* rect = new RectangleSprite(getAsset("White Pixel"));
	rect->setDimensions(position, size);

	return rect;
}

RectangleFrame* GUIFactory::createRectangleFrame(const Vector2& position,
	const Vector2& size, USHORT frameThickness, Color frameColor) {

	RectangleFrame* frame = new RectangleFrame(getAsset("White Pixel"));
	frame->setDimensions(position, size, frameThickness);
	frame->setTint(frameColor);

	return frame;
}

TriangleFrame* GUIFactory::createTriangleFrame(const Vector2& pt1, const Vector2& pt2,
	const Vector2& pt3, USHORT thickness) {

	TriangleFrame* triangle = new TriangleFrame(getAsset("White Pixel"));
	triangle->setDimensions(pt1, pt2, pt3, thickness);

	return triangle;
}

TextLabel* GUIFactory::createTextLabel(const Vector2& position,
	const char_t* fontName) {

	TextLabel* label = new TextLabel(position, getFont(fontName));
	label->initializeControl(this, mouseController);
	return label;
}

TextLabel* GUIFactory::createTextLabel(const Vector2& position,
	wstring text, const char_t* fontName) {

	TextLabel* label = new TextLabel(position, text, getFont(fontName));
	label->initializeControl(this, mouseController);
	return label;
}


Button* GUIFactory::createButton(const char_t* fontName) {

	Button* button = new Button(getAsset("White Pixel"), getFont(fontName));
	button->initializeControl(this, mouseController);
	return button;
}


Button* GUIFactory::createButton(const Vector2& position, const Vector2& size,
	wstring text, const char_t* fontName, const int frameThickness) {

	Button* button = new Button(getAsset("White Pixel"), getFont(fontName));
	button->initializeControl(this, mouseController);
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

	ImageButton* button = new ImageButton(getSpriteFromAsset(buttonImage),
		getFont(fontName));
	button->initializeControl(this, mouseController);
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

	ImageButton* button = new ImageButton(getSpriteFromAsset(upImageName),
		getSpriteFromAsset(downImageName), getFont(fontName));
	button->initializeControl(this, mouseController);

	return button;
}

Button* GUIFactory::createImageButton(const Vector2& position,
	const char_t* upImage, const char_t* downImage, const char_t* fontName) {

	ImageButton* button = (ImageButton*) createImageButton(upImage, downImage, fontName);
	button->setPosition(position);
	return button;
}

Button* GUIFactory::createImageButton(unique_ptr<Sprite> upSprite, const char_t* fontName) {

	ImageButton* button = new ImageButton(move(upSprite), getFont(fontName));
	button->initializeControl(this, mouseController);
	return button;
}

AnimatedButton* GUIFactory::createAnimatedButton(const char_t* animatedButtonName,
	Vector2 position) {

	AnimatedButton* button = new AnimatedButton(getAnimation(animatedButtonName), position);
	button->initializeControl(this, mouseController);
	return button;
}


CheckBox* GUIFactory::createCheckBox(const Vector2& position,
	wstring text, const char_t* fontName) {

	CheckBox* check = new CheckBox(getSpriteFromAsset("CheckBox Unchecked"),
		getSpriteFromAsset("CheckBox Checked"), getFont(fontName));
	check->initializeControl(this, mouseController);
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

	CheckBox* check = new CheckBox(getSpriteFromAsset(uncheckedImage),
		getSpriteFromAsset(checkedImage), getFont(fontName));
	check->initializeControl(this, mouseController);
	check->setText(text);
	check->setPosition(position);
	return check;

}


Spinner* GUIFactory::createSpinner(const Vector2& position, const size_t width,
	const size_t itemHeight, bool autoSize, const char_t* upButtonAsset,
	const char_t* downButtonAsset, const char_t* fontName) {

	Spinner* spinner = new Spinner(position, width, itemHeight, autoSize);
	spinner->initializeControl(this, mouseController);
	spinner->initialize(fontName, getAsset("White Pixel"), upButtonAsset, downButtonAsset);
	return spinner;
}


ScrollBar* GUIFactory::createScrollBar(const Vector2& position, size_t barHeight) {

	ScrollBar* scrollBar = new ScrollBar(position);
	scrollBar->initializeControl(this, mouseController);

	if (!scrollBar->initialize(getAsset("White Pixel"), barHeight)) {

		MessageBox(NULL, L"Failed to create ScrollBar",
			L"GUI initialization ERROR", MB_OK);
	}

	return scrollBar;
}


ScrollBar* GUIFactory::createScrollBar(const Vector2& position, size_t barHeight,
	ScrollBarDesc& scrollBarDesc) {

	ScrollBar* scrollBar = new ScrollBar(position);
	scrollBar->initializeControl(this, mouseController);

	ImageButton* buttons[2] = {NULL, NULL};
	buttons[0] = (ImageButton*) createImageButton(scrollBarDesc.upButtonImage.c_str(),
		scrollBarDesc.upPressedButtonImage.c_str());

	if (scrollBarDesc.downButtonImage == "") { // copy the up imagebutton and reverse
		buttons[1] = (ImageButton*) createImageButton(scrollBarDesc.upButtonImage.c_str(),
			scrollBarDesc.upPressedButtonImage.c_str());
		buttons[1]->setRotation(XM_PI);
		//buttons[1]->setPosition(
	} else {
		buttons[1] = (ImageButton*) createImageButton(scrollBarDesc.downButtonImage.c_str(),
			scrollBarDesc.downPressedButtonImage.c_str());
	}

	if (!scrollBar->initialize(getAsset("White Pixel"), barHeight, buttons,
		getSpriteFromAsset(scrollBarDesc.trackImage.c_str()),
		getAsset(scrollBarDesc.scrubberImage.c_str()))) {

		MessageBox(NULL, L"Failed to create ScrollBar",
			L"GUI initialization ERROR", MB_OK);
	}

	return scrollBar;
}

TexturePanel* GUIFactory::createPanel(bool scrollBarAlwaysVisible) {

	TexturePanel* panel = new TexturePanel(getAsset("White Pixel"), createScrollBar(Vector2::Zero, 10));
	panel->initializeControl(this, mouseController);
	return panel;
}


ListBox* GUIFactory::createListBox(const Vector2& position,
	const int width, const int itemHeight, const int maxItemsShown,
	bool enumerateList, const char_t* fontName) {


	ListBox* listbox = new ListBox(position, width, itemHeight, maxItemsShown);
	listbox->initializeControl(this, mouseController);
	Vector2 scrollBarPos(position.x + width, position.y);
	listbox->initialize(getFont(fontName), getAsset("White Pixel"),
		createScrollBar(scrollBarPos, itemHeight * maxItemsShown), enumerateList);

	return listbox;
}


ComboBox* GUIFactory::createComboBox(const Vector2& position,
	const int width, const int itemHeight, const int maxItemsShown,
	bool enumerateList, const char_t* buttonAsset, const char_t* fontName) {

	ComboBox* combobox = new ComboBox(position, width, itemHeight, maxItemsShown);
	combobox->initializeControl(this, mouseController);

	combobox->initialize(getFont(fontName), getAsset("White Pixel"),
		createListBox(
			Vector2(position.x, position.y + itemHeight),
			width, itemHeight, maxItemsShown, enumerateList, fontName),
		buttonAsset, enumerateList);

	return combobox;
}

Dialog* GUIFactory::createDialog(bool movable, bool centerText, const char_t* fontName) {

	Dialog* dialog = new Dialog(hwnd, movable, centerText);
	dialog->initializeControl(this, mouseController);
	dialog->initialize(getAsset("White Pixel"), fontName);
	return dialog;
}



GraphicsAsset* GUIFactory::createTextureFromIElement2D(
	IElement2D* control, Color bgColor) {

	int buffer = 20; // padding to give a bit of lee-way to prevent tearing

	RECT rect;
	GetClientRect(hwnd, &rect);

	int screenWidth = rect.right - rect.left + buffer;
	int screenHeight = rect.bottom - rect.top + buffer;

	int width = control->getWidth();
	int height = control->getHeight();
	int heightPadding = 0;
	int widthPadding = 0;
	float ratio = (float) screenWidth / screenHeight;
	if (width > height) {
		heightPadding = width / ratio - height;
		height = width / ratio;
	} else {
		widthPadding = height*ratio - width;
		width = height * ratio;
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
	deviceContext->OMSetRenderTargets(1, textureRenderTargetView.GetAddressOf(), nullptr);


	Vector2 oldPos = control->getPosition();
	//Vector2 oldSize = Vector2(control->getWidth(), control->getHeight());
	control->setPosition(Vector2(0, 0));

	deviceContext->ClearRenderTargetView(textureRenderTargetView.Get(), bgColor);

	batch->Begin(SpriteSortMode_Immediate);
	{
		control->draw(batch);
	}
	batch->End();


	deviceContext->OMSetRenderTargets(1, oldRenderTargetView.GetAddressOf(), nullptr);

	control->setPosition(oldPos);
	GraphicsAsset* gfxAsset = new GraphicsAsset();
	gfxAsset->loadAsPartOfSheet(shaderResourceView, Vector2(0, 0),
		Vector2(width - widthPadding - buffer, height - heightPadding - buffer));
	return gfxAsset;
}

GraphicsAsset* GUIFactory::createTextureFromScreen(Screen* screen, Color bgColor) {

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
	deviceContext->OMSetRenderTargets(1, textureRenderTargetView.GetAddressOf(), nullptr);


	deviceContext->ClearRenderTargetView(textureRenderTargetView.Get(), bgColor);

	batch->Begin(SpriteSortMode_Immediate);
	{
		screen->draw(batch);
	}
	batch->End();


	deviceContext->OMSetRenderTargets(1, oldRenderTargetView.GetAddressOf(), nullptr);

	GraphicsAsset* gfxAsset = new GraphicsAsset();
	gfxAsset->loadAsPartOfSheet(shaderResourceView, Vector2(0, 0),
		Vector2(screenWidth - buffer, screenHeight - buffer));
	return gfxAsset;
}


#include "DDSTextureLoader.h"
bool GUIFactory::getGUIAssetsFromXML() {

	string assetsDir =
		guiAssetsNode.parent().attribute("dir").as_string();

	string guiDir =
		assetsDir + guiAssetsNode.attribute("dir").as_string();


	xml_node fonts = guiAssetsNode.child("spritefonts");
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

	for (xml_node spriteNode = guiAssetsNode.child("sprite"); spriteNode;
		spriteNode = spriteNode.next_sibling("sprite")) {

		string file_s = guiDir + spriteNode.attribute("file").as_string();
		const char_t* file = file_s.c_str();
		const char_t* name = spriteNode.attribute("name").as_string();

		string check = name; // I think this is required - ya, pretty sure it is
		if (name == string("White Pixel")) {
			if (StringHelper::reportError(
				DirectX::CreateDDSTextureFromFile(
					device.Get(), StringHelper::convertCharStarToWCharT(file), NULL,
					whitePixel.GetAddressOf()),
				L"Failed to create texture from WhitePixel.dds", L"ERROR"))
				return false;

		}

		Vector2 origin = Vector2(-1000, -1000);
		xml_node originNode = spriteNode.child("origin");
		if (originNode) {
			origin.x = originNode.attribute("x").as_int();
			origin.y = originNode.attribute("y").as_int();
		}
		unique_ptr<GraphicsAsset> guiAsset;
		guiAsset.reset(new GraphicsAsset());
		if (!guiAsset->load(device, StringHelper::convertCharStarToWCharT(file), origin)) {
			wstringstream wss;
			wss << "Unable to load file: " << file;
			MessageBox(hwnd, wss.str().c_str(), L"Critical error", MB_OK);
			return false;
		}

		assetMap[check] = move(guiAsset);
	}

	for (xml_node spritesheetNode = guiAssetsNode.child("spritesheet");
		spritesheetNode; spritesheetNode = spritesheetNode.next_sibling("spritesheet")) {

		string file_s = guiDir + spritesheetNode.attribute("file").as_string();
		const char_t* file = file_s.c_str();

		// the spritesheet itself is never saved into the map
		unique_ptr<GraphicsAsset> masterAsset;
		masterAsset.reset(new GraphicsAsset());
		if (!masterAsset->load(device, StringHelper::convertCharStarToWCharT(file), Vector2::Zero))
			return false;


		// parse all animations from spritesheet
		for (xml_node animationNode = spritesheetNode.child("animation");
			animationNode; animationNode = animationNode.next_sibling("animation")) {

			const char_t* name = animationNode.attribute("name").as_string();

			vector<shared_ptr<Frame>> frames;
			for (xml_node spriteNode = animationNode.child("sprite"); spriteNode;
				spriteNode = spriteNode.next_sibling("sprite")) {


				RECT rect;
				rect.left = spriteNode.attribute("x").as_int();
				rect.top = spriteNode.attribute("y").as_int();
				rect.right = rect.left + spriteNode.attribute("width").as_int();
				rect.bottom = rect.top + spriteNode.attribute("height").as_int();
				shared_ptr<Frame> frame;
				frame.reset(new Frame(rect));
				frames.push_back(move(frame));

			}
			float frameTime = animationNode.attribute("timePerFrame").as_float();
			shared_ptr<Animation> animationAsset;
			animationAsset.reset(new Animation(masterAsset->getTexture(), frames, frameTime));
			animationMap[name] = animationAsset;
		}

		// parse all single sprites from spritesheet
		for (xml_node spriteNode = spritesheetNode.child("sprite"); spriteNode;
			spriteNode = spriteNode.next_sibling("sprite")) {

			const char_t* name = spriteNode.attribute("name").as_string();
			// pos in spritesheet
			Vector2 position = Vector2(spriteNode.attribute("x").as_int(),
				spriteNode.attribute("y").as_int());
			// dimensions in spritesheet
			Vector2 size = Vector2(spriteNode.attribute("width").as_int(),
				spriteNode.attribute("height").as_int());

			Vector2 origin = Vector2(-1000, -1000);
			xml_node originNode = spriteNode.child("origin");
			if (originNode) {
				origin.x = originNode.attribute("x").as_int();
				origin.y = originNode.attribute("y").as_int();
			}

			unique_ptr<GraphicsAsset> spriteAsset;
			spriteAsset.reset(new GraphicsAsset());
			spriteAsset->loadAsPartOfSheet(masterAsset->getTexture(), position, size, origin);

			assetMap[name] = move(spriteAsset);
		}

	}
	return true;
}
