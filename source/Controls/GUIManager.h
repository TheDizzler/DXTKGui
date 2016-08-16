#pragma once

#include <map>
#include <pugixml.hpp>
//#include "../Engine/MouseController.h"
#include "../Engine/KeyboardController.h"
#include "ListBox.h"

using namespace pugi;


class GUIManager {
public:
	GUIManager(xml_node guiAssetsNode);
	~GUIManager();


	bool initialize(ID3D11Device* device);

	//void update(double deltaTime, MouseController* mouse, KeyboardController* keys);
	shared_ptr<FontSet> getFont(const char_t* fontName);
	shared_ptr<Sprite> getSprite(const char_t* spriteName);

	Button* createImageButton(const char_t* fontName, const char_t* upImage, const char_t* downImage);
	Button* createButton(const char_t* fontName);


	//bool createForm();

private:

	//ID3D11Device* device;
	/** ID3D11ShaderResourceView is a ComPtr!
		This is used for solid color backgrounds and area fills. */
	ComPtr<ID3D11ShaderResourceView> whitePixel;

	/*shared_ptr<FontSet> mainFont;
	shared_ptr<FontSet> altFont;
	shared_ptr<FontSet> specialFont;*/

	bool getGUIAssets(ID3D11Device* device);
	xml_node guiAssetsNode;

	shared_ptr<FontSet> defaultFont;
	map<string, shared_ptr<FontSet> > fontMap;
	map<string, shared_ptr<Sprite> > spriteMap;


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
