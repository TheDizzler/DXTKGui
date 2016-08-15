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

	void update(double deltaTime, MouseController* mouse, KeyboardController* keys);


private:

	ID3D11Device* device;
	/** ID3D11ShaderResourceView is a ComPtr! 
		This is used for solid color backgrounds and area fills. */
	ComPtr<ID3D11ShaderResourceView> whitePixel;

	/*shared_ptr<FontSet> mainFont;
	shared_ptr<FontSet> altFont;
	shared_ptr<FontSet> specialFont;*/

	bool getGUIAssets();
	xml_node guiAssetsNode;

	map<const char_t*, shared_ptr<FontSet> > fontMap;
	map<const char_t*, shared_ptr<Sprite> > spriteMap;
};