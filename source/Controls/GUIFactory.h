#pragma once

#include <map>
#include <pugixml.hpp>
//#include "../Engine/MouseController.h"
//#include "../Engine/KeyboardController.h"
#include "ComboBox.h"
#include "../BaseGraphics/GraphicsAsset.h"
#include "Dialog.h"


using namespace pugi;


class GUIFactory {
public:
	GUIFactory(xml_node guiAssetsNode);
	~GUIFactory();


	bool initialize(ComPtr<ID3D11Device> device);

	unique_ptr<FontSet> getFont(const char_t* fontName);
	unique_ptr<Sprite> getSpriteFromAsset(const char_t* assetName);
	GraphicsAsset* const getAsset(const char_t* assetName);

	TextLabel* createTextLabel(const Vector2& position,
		const char_t* fontName = "Default Font");
	TextLabel* createTextLabel(const Vector2& position,
		wstring text, const char_t* fontName = "Default Font");

	Button* createImageButton(const char_t* upImage, const char_t* downImage,
		const char_t* fontName = "Default Font");

	Button* createButton(const char_t* fontName = "Default Font");
	Button* createButton(const Vector2& position, const Vector2& size,
		wstring text = L"", const char_t* fontName = "Default Font",
		const int frameThickness = 2);

	ListBox* createListBox(const Vector2& position, const int width,
		const int maxItemsShown = 7, const char_t* fontName = "Default Font");

	Dialog* createDialog(const char_t* fontName = "Default Font");


private:

	ComPtr<ID3D11Device> device;
	/** ID3D11ShaderResourceView is a ComPtr!
		This is used for solid color backgrounds and area fills. */
	ComPtr<ID3D11ShaderResourceView> whitePixel;


	bool getGUIAssets(ComPtr<ID3D11Device> device);
	xml_node guiAssetsNode;

	const wchar_t* defaultFontFile;

	map<string, const char_t* > fontMap;
	map<string, unique_ptr<GraphicsAsset> > assetMap;


};
