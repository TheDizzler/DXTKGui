#include "GUIFactory.h"

GUIFactory::GUIFactory(pugi::xml_node guiAssets) {

	guiAssetsNode = guiAssets;
}

GUIFactory::~GUIFactory() {

	assetMap.clear();
	fontMap.clear();
}


bool GUIFactory::initialize(ComPtr<ID3D11Device> dev) {

	device = dev;
	if (!getGUIAssetsFromXML(device)) {
		MessageBox(0, L"Sprite retrieval from Asset Manifest failed.",
			L"Epic failure", MB_OK);
		return false;
	}


	return true;
}

#include "../assets.h"
unique_ptr<FontSet> GUIFactory::getFont(const char_t* fontName) {

	if (fontMap.find(fontName) == fontMap.end()) {

		wostringstream ws;
		ws << "Cannot find font file. Using default font instead.\n";
		OutputDebugString(ws.str().c_str());

		unique_ptr<FontSet> defaultFont;
		defaultFont.reset(new FontSet());
		defaultFont->load(device, Assets::convertCharStarToWCharT(defaultFontFile));
		defaultFont->setTint(DirectX::Colors::White.v);

		return defaultFont;
	}

	const char_t* fontFile = fontMap[fontName];

	unique_ptr<FontSet> font;
	font.reset(new FontSet());
	font->load(device, Assets::convertCharStarToWCharT(fontFile));
	font->setTint(DirectX::Colors::White.v);
	return font;
}


GraphicsAsset* const GUIFactory::getAsset(const char_t* assetName) {

	if (assetMap.find(assetName) == assetMap.end()) {
		wostringstream ws;
		ws << "Cannot find asset file: " << assetName << "\n";
		ws << "Count : " << assetMap.count(assetName) << "\n";
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
	return sprite;
}


TextLabel* GUIFactory::createTextLabel(const Vector2& position,
	const char_t* fontName) {
	return new TextLabel(position, getFont(fontName));
}

TextLabel* GUIFactory::createTextLabel(const Vector2& position,
	wstring text, const char_t* fontName) {

	TextLabel* label = new TextLabel(position, text, getFont(fontName));
	label->setFactory(this);
	return label;
}


Button* GUIFactory::createButton(const char_t* fontName) {

	Button* button = new Button(getAsset("White Pixel"), getFont(fontName));
	button->setFactory(this);
	return button;
}


Button* GUIFactory::createButton(const Vector2& position, const Vector2& size,
	wstring text, const char_t* fontName, const int frameThickness) {

	Button* button = new Button(getAsset("White Pixel"), getFont(fontName));
	button->setFactory(this);
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
	button->setFactory(this);
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
	button->setFactory(this);

	return button;
}

Button* GUIFactory::createImageButton(const Vector2& position,
	const char_t* upImage, const char_t* downImage, const char_t* fontName) {

	ImageButton* button = (ImageButton*) createImageButton(upImage, downImage, fontName);
	button->setPosition(position);
	return button;
}


CheckBox* GUIFactory::createCheckBox(const Vector2& position,
	wstring text, const char_t* fontName) {

	CheckBox* check = new CheckBox(getSpriteFromAsset("CheckBox Unchecked"),
		getSpriteFromAsset("CheckBox Checked"), getFont(fontName));
	check->setFactory(this);
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
	check->setFactory(this);
	check->setText(text);
	check->setPosition(position);
	return check;

}

ScrollBar* GUIFactory::createScrollBar(const Vector2& position, size_t barHeight) {

	ScrollBar* scrollBar = new ScrollBar(position);
	scrollBar->setFactory(this);

	if (!scrollBar->initialize(getAsset("White Pixel"), barHeight)) {

		MessageBox(NULL, L"Failed to create ScrollBar",
			L"GUI initialization ERROR", MB_OK);
	}

	return scrollBar;
}


ScrollBar* GUIFactory::createScrollBar(const Vector2& position, size_t barHeight,
	ScrollBarDesc& scrollBarDesc) {

	ScrollBar* scrollBar = new ScrollBar(position);
	scrollBar->setFactory(this);

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

//ScrollBar* GUIFactory::createScrollBar(const Vector2 & position, size_t maxHeight,
//	const char_t* scrollButtonImage, const char_t* scrollBarImage,
//	const char_t* scrubberImage) {
//
//	ImageButton* upButton;
//	if (assetMap.find(scrollButtonImage) == assetMap.end()) {
//		wostringstream ws;
//		ws << "Could not find scroll button image file: ";
//		ws << scrollButtonImage;
//		ws << "\nUsing default." << "\n";
//		OutputDebugString(ws.str().c_str());
//		scrollButtonImage = "ScrollBar Up";
//	}
//	if (assetMap.find(scrollBarImage) == assetMap.end()) {
//
//		wostringstream ws;
//		ws << "Could not find scroll bar image file: ";
//		ws << scrollBarImage;
//		ws << "\nUsing default." << "\n";
//		OutputDebugString(ws.str().c_str());
//		scrollBarImage = 
//	}
//
//	return scrollBar;
//}

ListBox* GUIFactory::createListBox(const Vector2& position,
	const int width, const int itemHeight, const int maxItemsShown,
	bool enumerateList, const char_t* fontName) {


	ListBox* listbox = new ListBox(position, width, itemHeight, maxItemsShown);
	listbox->setFactory(this);
	Vector2 scrollBarPos(position.x + width, position.y);
	listbox->initialize(getFont(fontName), getAsset("White Pixel"),
		createScrollBar(scrollBarPos, itemHeight * maxItemsShown), enumerateList);

	return listbox;
}



ComboBox* GUIFactory::createComboBox(const Vector2& position,
	const int width, const int itemHeight, const int maxItemsShown,
	bool enumerateList, const char_t* fontName) {

	ComboBox* combobox = new ComboBox(position, width, itemHeight, maxItemsShown);
	combobox->setFactory(this);

	combobox->initialize(getFont(fontName), getAsset("White Pixel"),
		createListBox(
			Vector2(position.x, position.y + itemHeight),
			width, itemHeight, maxItemsShown, enumerateList, fontName),
		enumerateList);

	return combobox;
}

Dialog* GUIFactory::createDialog(bool movable, const char_t* fontName) {

	Dialog* dialog = new Dialog(movable);
	dialog->setFactory(this);
	dialog->initialize(getAsset("White Pixel"), fontName);
	return dialog;
}



#include "DDSTextureLoader.h"
bool GUIFactory::getGUIAssetsFromXML(ComPtr<ID3D11Device> device) {

	xml_node fonts = guiAssetsNode.child("spritefonts");
	for (xml_node fontNode = fonts.child("font"); fontNode;
		fontNode = fontNode.next_sibling("font")) {

		const char_t* file = fontNode.attribute("file").as_string();
		const char_t* name = fontNode.attribute("name").as_string();

		fontMap[name] = file;

	}

	defaultFontFile = Assets::defaultFontFile;
	fontMap["Default Font"] = Assets::defaultFontFile;

	for (xml_node spriteNode = guiAssetsNode.child("sprite"); spriteNode;
		spriteNode = spriteNode.next_sibling("sprite")) {

		const char_t* file = spriteNode.attribute("file").as_string();
		const char_t* name = spriteNode.attribute("name").as_string();

		string check = name; // I think this is required - pretty sure it is
		if (name == string("White Pixel")) {
			if (Globals::reportError(DirectX::CreateDDSTextureFromFile(
				device.Get(), Assets::convertCharStarToWCharT(file), NULL,
				whitePixel.GetAddressOf()))) {

				MessageBox(NULL, L"Failed to create texture from WhitePixel.dds",
					L"ERROR", MB_OK);
				return false;
			}
		} /*else {*/
		unique_ptr<GraphicsAsset> guiAsset;
		guiAsset.reset(new GraphicsAsset());
		if (!guiAsset->load(device, Assets::convertCharStarToWCharT(file)))
			return false;

		assetMap[check] = move(guiAsset);
	//}

	/*wostringstream ws;
	ws << "file: " << file << "\n";
	ws << "name : " << name << "\n";
	OutputDebugString(ws.str().c_str());*/
	}

	return true;
}
