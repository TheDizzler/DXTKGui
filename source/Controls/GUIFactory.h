#include "../pch.h"
#pragma once

#include <map>
#include <pugixml.hpp>
#include "ComboBox.h"
#include "../BaseGraphics/GraphicsAsset.h"
#include "Dialog.h"
#include "CheckBox.h"


using namespace pugi;



class GUIFactory {
public:
	GUIFactory(xml_node guiAssetsNode);
	~GUIFactory();


	bool initialize(ComPtr<ID3D11Device> device);

	unique_ptr<FontSet> getFont(const char_t* fontName);
	unique_ptr<Sprite> getSpriteFromAsset(const char_t* assetName);
	shared_ptr<Animation> getAnimation(const char_t* animationName);
	GraphicsAsset* const getAsset(const char_t* assetName);


	TextLabel* createTextLabel(const Vector2& position,
		const char_t* fontName = "Default Font");
	TextLabel* createTextLabel(const Vector2& position,
		wstring text, const char_t* fontName = "Default Font");

	/** Creates a button with no set text or position. */
	Button* createButton(const char_t* fontName = "Default Font");
	Button* createButton(const Vector2& position, const Vector2& size,
		wstring text = L"", const char_t* fontName = "Default Font",
		const int frameThickness = 2);


	Button* createImageButton(const char_t* upImage, const char_t* downImage = NULL,
		const char_t* fontName = "Default Font");
	Button* createImageButton(const Vector2& position,
		const char_t* upImage, const char_t* downImage,
		const char_t* fontName = "Default Font");

	AnimatedButton* createAnimatedButton(const char_t* animatedButtonName,
		Vector2 position = Vector2::Zero);

	/** Created a checkbox using default images. */
	CheckBox* createCheckBox(const Vector2& position, wstring text = L"",
		const char_t* fontName = "Default Font");
	CheckBox* createCheckBox(const Vector2& position,
		const char_t* uncheckedImage, const char_t* checkedImage,
		wstring text = L"", const char_t* fontName = "Default Font");

	ListBox* createListBox(const Vector2& position, const int width,
		const int itemHeight = 32, const int maxItemsShown = 7,
		bool enumerateList = false, const char_t* fontName = "Default Font");

	ComboBox* createComboBox(const Vector2& position, const int width,
		const int itemHeight = 32, const int maxItemsShown = 7,
		const char_t* buttonAsset = "Combo Button Closed",
		bool enumerateList = false, const char_t* fontName = "Default Font");

	Dialog* createDialog(bool movable = false, const char_t* fontName = "Default Font");

	ScrollBar* createScrollBar(const Vector2& position, size_t barHeight);
	ScrollBar* createScrollBar(const Vector2& position, size_t barHeight,
		ScrollBarDesc& scrollBarDesc);

private:

	ComPtr<ID3D11Device> device;
	/** ID3D11ShaderResourceView is a ComPtr!
		This is used for solid color backgrounds and area fills. */
	ComPtr<ID3D11ShaderResourceView> whitePixel;

	/* Creates an image button with only one image. */
	Button* createOneImageButton(const char_t* buttonImage,
		const char_t* fontName = "Default Font");

	bool getGUIAssetsFromXML(ComPtr<ID3D11Device> device);
	xml_node guiAssetsNode;

	const char_t* defaultFontFile;

	map<string, string> fontMap;
	map<string, unique_ptr<GraphicsAsset> > assetMap;
	map<string, shared_ptr<Animation> > animationMap;

};
