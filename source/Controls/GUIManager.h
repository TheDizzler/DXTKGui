#pragma once

#include <map>
#include <pugixml.hpp>
//#include "../Engine/MouseController.h"
#include "../Engine/KeyboardController.h"
#include "ListBox.h"
#include "../BaseGraphics/GraphicsAsset.h"


using namespace pugi;


class GUIManager {
public:
	GUIManager(xml_node guiAssetsNode);
	~GUIManager();


	bool initialize(ID3D11Device* device);

	//void update(double deltaTime, MouseController* mouse, KeyboardController* keys);
	unique_ptr<FontSet> getFont(const char_t* fontName);
	unique_ptr<Sprite> getSpriteFromAsset(const char_t* assetName);
	GraphicsAsset* getAsset(const char_t* assetName);

	Button* createImageButton(const char_t* fontName, const char_t* upImage, const char_t* downImage);
	Button* createButton(const char_t* fontName);


	//bool createForm();

private:

	ID3D11Device* device;
	/** ID3D11ShaderResourceView is a ComPtr!
		This is used for solid color backgrounds and area fills. */
	ComPtr<ID3D11ShaderResourceView> whitePixel;


	bool getGUIAssets(ID3D11Device* device);
	xml_node guiAssetsNode;

	//shared_ptr<FontSet> defaultFont;
	const wchar_t* defaultFontFile;
	//map<string, unique_ptr<FontSet> > fontMap;
	map<string, const char_t* > fontMap;
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
//	GUIManager* guiManager;
//};
