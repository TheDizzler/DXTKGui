#pragma once



#include "BaseGraphics/screen.h"
#include "BaseGraphics/GraphicsAsset.h"
#include "Controls/ComboBox.h"
#include "Controls/DynamicDialog.h"
#include "Controls/CheckBox.h"
#include "Controls/Spinner.h"
#include "Effects/LetterJammer.h"


using namespace pugi;


class GUIFactory {
public:
	virtual ~GUIFactory();

	/** Required if a user wants to create their own controls. */
	HWND getHWND();
	/** Required if a user wants to create their own controls. */
	MouseController* getMouseController();

	/** DeviceContext and SpriteBatch references are required
		to create textures from a GUIControl.
		If an optional gui asset file is used it MUST have a node called 'gui'. */
	bool initialize(HWND hwnd, ComPtr<ID3D11Device> device,
		ComPtr<ID3D11DeviceContext> devCon, ComPtr<IDXGISwapChain> swapChain,
		SpriteBatch* batch, MouseController* mouse,
		const char_t* assetManifestFile = NULL);

	void reInitDevice(ComPtr<ID3D11Device> device,
		ComPtr<ID3D11DeviceContext> devCon, SpriteBatch* batch);

	unique_ptr<FontSet> getFont(const char_t* fontName = "Default Font");
	unique_ptr<Sprite> getSpriteFromAsset(const char_t* assetName);
	Animation* getAnimation(const char_t* animationName);
	GraphicsAsset* const getAsset(const char_t* assetName);
	AssetSet* const getAssetSet(const char_t* setName);

	Line* createLine(const Vector2& position, const Vector2& size, Color lineColor = Color(0, 0, 0, 1));
	Line* createLineBetween(const Vector2 pointA, const Vector2 pointB, Color lineColor = Color(0, 0, 0, 1));

	RectangleSprite* createRectangle(const Vector2& position = Vector2::Zero,
		const Vector2& size = Vector2::Zero, Color color = Color(1, 1, 1, 1));

	RectangleFrame* createRectangleFrame(const Vector2& position = Vector2::Zero,
		const Vector2& size = Vector2(10, 10), USHORT frameThickness = 2,
		Color frameColor = Color(0, 0, 0, 1));

	TriangleFrame* createTriangleFrame(const Vector2& point1,
		const Vector2& point2, const Vector2& point3, USHORT thickness = 2);

	/** @useTexture - drawing fonts is expensive so the complete text is
			turned into a texture to cut down on draw time. This is far more efficient
			even if the text is updating every second (have not tested it at more than 1 update
			a second but it should remain more efficient as long as the changes made
			to the text is less frequent than the amount of draw commands.)
			Only set to false if using a throw away text that won't be drawn
			OR you need a textlabel that updates in real-time (not reccomended). */
	TextLabel* createTextLabel(const Vector2& position,
		wstring text = L"", const char_t* fontName = "Default Font",
		bool useTexture = true);

	LetterJammer* createLetterJammer(const Vector2& position,
		wstring text = L"", Color textColor = Color(1, 1, 1, 1),
		bool autoRun = true, const char_t* fontName = "Default Font");

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
	Button* createImageButton(unique_ptr<Sprite> upSprite, const char_t* fontName = "Default Font");


	/** Created a checkbox using default images. */
	CheckBox* createCheckBox(const Vector2& position, wstring text = L"",
		const char_t* fontName = "Default Font");
	CheckBox* createCheckBox(const Vector2& position,
		const char_t* uncheckedImage, const char_t* checkedImage,
		wstring text = L"", const char_t* fontName = "Default Font");

	Spinner* createSpinner(const Vector2& position, const size_t width,
		const size_t itemHeight = 32, bool autoSize = true, const char_t* upButtonAsset = "Spinner Up Button",
		const char_t* downButtonAsset = "Spinner Down Button", const char_t* fontName = "Default Font");


	ListBox* createListBox(const Vector2& position, const int width,
		const int itemHeight = 32, const int maxItemsShown = 7,
		bool enumerateList = false, const char_t* fontName = "Default Font",
		const int frameThickness = 2);

	ComboBox* createComboBox(const Vector2& position, const int width,
		const int itemHeight = 32, const int maxItemsShown = 7,
		bool enumerateList = false, const char_t* buttonAsset = "Combo Button Closed",
		const char_t* fontName = "Default Font", const int frameThickness = 2);

	PromptDialog* createDialog(
		const Vector2& position = Vector2::Zero, const Vector2& size = Vector2::Zero,
		bool movable = false, bool centerText = false, int frameThickness = 2,
		const char_t* fontName = "Default Font");

	DynamicDialog* createDynamicDialog(const char_t* imageSet,
		const Vector2& position = Vector2::Zero, const Vector2& size = Vector2::Zero,
		const char_t* fontName = "Default Font");
	DynamicDialog* createDynamicDialog(AssetSet* dialogImageSet,
		const Vector2& position = Vector2::Zero, const Vector2& size = Vector2::Zero,
		const char_t* fontName = "Default Font");

	ScrollBar* createScrollBar(const Vector2& position, size_t barHeight);
	ScrollBar* createScrollBar(const Vector2& position, size_t barHeight,
		ScrollBarDesc& scrollBarDesc);

	TexturePanel* createPanel(bool neverShowScrollBar = true);

	/* Creates a texture from a screen grab of an IElement2D object.
		offset is the vector to bring object to top left corner of screen
			in prep for its close up.*/
	unique_ptr<GraphicsAsset> createTextureFromTexturizable(Texturizable* control,
		bool autoBatchDraw = true, Color bgColor = {0, 0, 0, 0});

	unique_ptr<GraphicsAsset> createTextureFromScreen(Screen* screen,
		bool autoBatchDraw = true, Color bgColor = {0, 0, 0, 0});

	static bool initialized;
private:
	HWND hwnd;
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> deviceContext;
	MouseController* mouseController;
	vector<unique_ptr<xml_document>> docs;

	SpriteBatch* batch;

	/* Creates an image button with only one image. */
	Button* createOneImageButton(const char_t* buttonImage,
		const char_t* fontName = "Default Font");

	bool getGUIAssetsFromXML(xml_node assetNode);
	unique_ptr<GraphicsAsset> parseSprite(xml_node spriteNode,
		ComPtr<ID3D11ShaderResourceView> sheetTexture,
		int xOffset = 0, int yOffset = 0);

	/* Allows for multiple asset files and sources. */
	vector<xml_node> guiAssetsNode;

	const char_t* defaultFontFile;

	map<string, string> fontMap;
	map<string, unique_ptr<GraphicsAsset> > assetMap;
	map<string, unique_ptr<Animation> > animationMap;
	map<string, unique_ptr<AssetSet> > setMap;

};
