#pragma once

#include <map>
#include <pugixml.hpp>
//#include "../Engine/MouseController.h"
#include "../Engine/KeyboardController.h"
#include "ListBox.h"
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
		wstring text = L"", const char_t* fontName = "Default Font");

	Button* createImageButton(const char_t* upImage, const char_t* downImage,
		const char_t* fontName = "Default Font");


	Button* createButton(const char_t* fontName = "Default Font");
	Button* createButton(const Vector2& position, const Vector2& size,
		int frameThickness = 2, const char_t* fontName = "Default Font");


	Dialog* createDialog(const char_t* fontName = "Default Font");

	//bool createForm();

private:

	ComPtr<ID3D11Device> device;
	/** ID3D11ShaderResourceView is a ComPtr!
		This is used for solid color backgrounds and area fills. */
	ComPtr<ID3D11ShaderResourceView> whitePixel;


	bool getGUIAssetsFromXML(ComPtr<ID3D11Device> device);
	xml_node guiAssetsNode;

	const wchar_t* defaultFontFile;

	map<string, const char_t*> fontMap;
	map<string, unique_ptr<GraphicsAsset> > assetMap;


};


/** A form to add GUI controls to. */
//class GUIForm {
//public:
//	GUIForm();
//	~GUIForm();
//
//
//	void update(double deltaTime, MouseController* mouse, KeyboardController* keys);
//	void draw(SpriteBatch* batch);
//
//
//private:
//	vector<GUIControl> controls;
//
//	GUIFactory* guiFactory;
//};
