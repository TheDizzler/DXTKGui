#include "GUIManager.h"

GUIManager::GUIManager(pugi::xml_node guiAssets) {

	guiAssetsNode = guiAssets;
}

GUIManager::~GUIManager() {
}


#include "../globals.h"
#include "DDSTextureLoader.h"
bool GUIManager::initialize(ID3D11Device* device) {

	if (!getGUIAssets(device)) {
		MessageBox(0, L"Sprite retrieval from Asset Manifest failed.",
			L"Epic failure", MB_OK);
		return false;
	}


	return true;
}

shared_ptr<FontSet> GUIManager::getFont(const char_t* fontName) {

	if (fontMap.find(fontName) == fontMap.end()) {

		wostringstream ws;
		ws << "Cannot find font file. Using default font instead.";
		OutputDebugString(ws.str().c_str());
		return defaultFont;
	} else
		return fontMap[fontName];


}

shared_ptr<Sprite> GUIManager::getSprite(const char_t* spriteName) {

	shared_ptr<Sprite> sprite;

	
	if (spriteMap.find(spriteName) == spriteMap.end()) {
		wostringstream ws;
		ws << "Cannot find sprite file: " << spriteName << "\n";
		ws << "Count : " << spriteMap.count(spriteName) << "\n";
		OutputDebugString(ws.str().c_str());
		return NULL;
	}

	return spriteMap[spriteName];
}


Button* GUIManager::createImageButton(const char_t* fontName,
	const char_t* upImageName, const char_t* downImageName) {


	if (spriteMap.find(upImageName) == spriteMap.end()
		|| spriteMap.find(downImageName) == spriteMap.end()) {

		wostringstream ws;
		ws << "Could not find button image files.";
		ws << "Creating non-image button instead." << "\n";
		OutputDebugString(ws.str().c_str());

		return createButton(fontName);
	};

	shared_ptr<FontSet> font = getFont(fontName);


	ImageButton* button = new ImageButton();
	button->load(font, spriteMap[upImageName], spriteMap[downImageName]);

	return button;
}


Button* GUIManager::createButton(const char_t * fontName) {

	shared_ptr<FontSet> font = getFont(fontName);

	Button* button = new Button();
	button->load(font, whitePixel);
	return button;
}


bool GUIManager::getGUIAssets(ID3D11Device* device) {

	xml_node fonts = guiAssetsNode.child("spritefonts");
	for (xml_node fontNode = fonts.child("font"); fontNode;
		fontNode = fontNode.next_sibling("font")) {

		const char_t* file = fontNode.attribute("file").as_string();
		const char_t* name = fontNode.attribute("name").as_string();

		shared_ptr<FontSet> font;
		font.reset(new FontSet());
		if (!font->load(device, Assets::convertCharStarToWCharT(file)))
			return false;
		font->setTint(DirectX::Colors::White.v);

		fontMap[name] = font;

	}

	defaultFont.reset(new FontSet());
	if (!defaultFont->load(device, Assets::defaultFontFile)) {
		MessageBox(0, L"Could not find default font file.", L"Critical Failure", MB_OK);
		return false;
	}
	

	for (xml_node spriteNode = guiAssetsNode.child("sprite"); spriteNode;
		spriteNode = spriteNode.next_sibling("sprite")) {

		const char_t* file = spriteNode.attribute("file").as_string();
		const char_t* name = spriteNode.attribute("name").as_string();

		string check = name; // I think this is required - pretty sure it is
		if (name == string("White Pixel")) {
			if (Globals::reportError(DirectX::CreateDDSTextureFromFile(
				device, Assets::convertCharStarToWCharT(file), NULL,
				whitePixel.GetAddressOf()))) {

				MessageBox(NULL, L"Failed to create texture from WhitePixel.dds",
					L"ERROR", MB_OK);
				return false;
			}
		} else {
			shared_ptr<Sprite> guiSprite;
			guiSprite.reset(new Sprite());
			if (!guiSprite->load(device, Assets::convertCharStarToWCharT(file)))
				return false;

			spriteMap[check] = guiSprite;
		}

		/*wostringstream ws;
		ws << "file: " << file << "\n";
		ws << "name : " << name << "\n";
		OutputDebugString(ws.str().c_str());*/
	}

	return true;
}
