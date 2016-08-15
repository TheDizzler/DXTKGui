#include "GUIManager.h"

GUIManager::GUIManager(pugi::xml_node guiAssets) {

	guiAssetsNode = guiAssets;
}

GUIManager::~GUIManager() {
}


#include "../globals.h"
#include "DDSTextureLoader.h"
bool GUIManager::initialize(ID3D11Device * device) {

	if (!getGUIAssets()) {
		MessageBox(0, L"Sprite retrieval from Asset Manifest failed."
			L"Epic failure", MB_OK);
		return false;
	}





	return true;
}

void GUIManager::update(double deltaTime, MouseController * mouse, KeyboardController * keys) {


}

bool GUIManager::getGUIAssets() {

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


	for (xml_node spriteNode = guiAssetsNode.child("sprite"); spriteNode;
		spriteNode = spriteNode.next_sibling("sprite")) {

		const char_t* file = spriteNode.attribute("file").as_string();
		const char_t* name = spriteNode.attribute("name").as_string();

		string check = name; // I think this is required?
		if (check == string("White Pixel")) {
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

			spriteMap[name] = guiSprite;
		}
	}

	return true;
}
