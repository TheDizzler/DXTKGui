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
		//MessageBox(0, ws.str().c_str(), L"Fatal Error", MB_OK);
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

	return label;
}


Button* GUIFactory::createButton(const char_t* fontName) {

	Button* button = new Button(whitePixel, getFont(fontName));
	return button;
}


Button* GUIFactory::createButton(const Vector2& position, const Vector2& size,
	wstring text, const char_t* fontName, const int frameThickness) {

	Button* button = new Button(whitePixel, getFont(fontName));
	button->setDimensions(position, size, frameThickness);
	button->setText(text);
	return button;
}


Button* GUIFactory::createImageButton(const char_t* upImageName,
	const char_t* downImageName, const char_t* fontName) {


	if (assetMap.find(upImageName) == assetMap.end()
		|| assetMap.find(downImageName) == assetMap.end()) {

		wostringstream ws;
		ws << "Could not find button image files: ";
		ws << upImageName << " and " << downImageName;
		ws << "\nCreating non-image button instead." << "\n";
		OutputDebugString(ws.str().c_str());

		return createButton(fontName);
	}


	ImageButton* button = new ImageButton(getSpriteFromAsset(upImageName),
		getSpriteFromAsset(downImageName), getFont(fontName));

	return button;
}


CheckBox* GUIFactory::createCheckBox(const Vector2& position,
	wstring text, const char_t* fontName) {

	CheckBox* check = new CheckBox(getSpriteFromAsset("CheckBox Unchecked"),
		getSpriteFromAsset("CheckBox Checked"), getFont(fontName));
	check->setText(text);
	check->setPosition(position);
	return check;
}

ListBox* GUIFactory::createListBox(const Vector2& position,
	const int width, const int maxItemsShown, bool enumerateList,
	const char_t* fontName) {

	ListBox* listbox = new ListBox(position, width, maxItemsShown);
	listbox->initialize(getFont(fontName), whitePixel, enumerateList);
	return listbox;
}

Dialog* GUIFactory::createDialog(const char_t* fontName) {

	Dialog* dialog = new Dialog();
	dialog->initialize(whitePixel, fontName);
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
		} else {
			unique_ptr<GraphicsAsset> guiAsset;
			guiAsset.reset(new GraphicsAsset());
			if (!guiAsset->load(device, Assets::convertCharStarToWCharT(file)))
				return false;

			assetMap[check] = move(guiAsset);
		}

		/*wostringstream ws;
		ws << "file: " << file << "\n";
		ws << "name : " << name << "\n";
		OutputDebugString(ws.str().c_str());*/
	}

	return true;
}
