#pragma once

#include "ScrollBar.h"


class ListItem {
public:
	virtual ~ListItem();

	void initialize(const int width, const int height,
		TextLabel* label, ComPtr<ID3D11ShaderResourceView> pixelTexture,
		size_t listPosition = 0, float layerDepth = .925, bool enumerateList = false);

	virtual void reloadGraphicsAsset(GUIFactory* guiFactory);

	void setWidth(int newWidth);

	Vector2 measureString() const;
	const wchar_t* toString();

	bool update(double deltaTime, MouseController* mouse);
	void updatePosition(const Vector2& position);
	virtual void draw(SpriteBatch* batch);

	void setLayerDepth(const float depth);

	void setSelected(bool isSelected);
	bool isSelected = false;
	bool isHovered = false;
protected:

	Color selectedFontColor = Color(0, 0, 0, 1);
	Color normalFontColor = Color(1, 1, 1, 1);

	Color selectedBGColor = Colors::White;
	Color hoverBGColor = Colors::Aqua;
	Color normalBGColor = Colors::BurlyWood;
	Color selectedAndHoveredColor = Colors::AntiqueWhite;
	Color currentBGColor = normalBGColor;

	virtual void setText() = 0;
	unique_ptr<TextLabel> textLabel;

	RECT itemRect;
	unique_ptr<HitArea> hitArea;

	/** itemPosition: position of entire Item.
			position: for text only. */
	Vector2 itemPosition;

	float layerDepth;

	size_t textMarginX = 10;
	size_t textMarginY = 5;

	
	bool buttonDownLast = false;

	ComPtr<ID3D11ShaderResourceView> pixel;

	bool isEnumerated;
	size_t listPosition = 0;

	GUIFactory* guiFactory;
};

class EmptyListItem : public ListItem {
public:
	virtual void setText() override {

		textLabel->setText("Empty...");
	}
};


/** A simple control to display various (text) items. */
class ListBox : public GUIControl, public Texturizable {
public:
	ListBox(GUIFactory* factory, MouseController* mouseController,
		const Vector2& position, const int width,
		size_t itemHeight = 32, const int maxItemsShown = 7);
	virtual ~ListBox();

	void initialize(const pugi::char_t* fontName,
		GraphicsAsset* pixelAsset, ScrollBar* scrollBar,
		bool enumerateList = false, const int frameThickness = 2);

	virtual void forceRefresh() override;
	virtual void reloadGraphicsAsset() override;

	virtual bool update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;

	virtual unique_ptr<GraphicsAsset> texturize() override;
	virtual void textureDraw(SpriteBatch* batch, ComPtr<ID3D11Device> device = NULL) override;


	void setScrollBar(ScrollBarDesc& scrollBarDesc);
	void alwaysShowScrollBar(bool alwaysShow);
	/* Note: this function has not been tested much....not at all really...*/
	void addItem(ListItem* item);
	void addItems(vector<ListItem*> items);
	void clear();


	void setSelected(size_t newIndex);
	const size_t getSelectedIndex() const;
	const int getHoveredIndex() const;
	ListItem* getSelected();
	ListItem* getItem(size_t index);


	/* Not used in ListBox. */
	virtual void setText(wstring text);
	/* Not used in ListBox. */
	virtual const Vector2 XM_CALLCONV measureString() const;

	bool multiSelect = false;

	/* Max items to display before showing scroll bar. */
	size_t maxDisplayItems = 7;


	virtual void moveBy(const Vector2& moveVector) override;
	virtual void setPosition(const Vector2& position) override;

	virtual void setFont(const pugi::char_t* font = "Default Font") override;
	virtual const Vector2 & getPosition() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;

	virtual void setLayerDepth(const float depth, bool frontToBack = true) override;

	virtual bool clicked() override;
	virtual bool pressed() override;
	virtual bool hovering() override;

	class ActionListener {
	public:
		/** listbox: The ListBox this ActionListener is attached to.
		selectedItemIndex: index of item in ListBox.*/
		virtual void onClick(ListBox* listbox, size_t selectedItemIndex) = 0;
		virtual void onHover(ListBox* listbox, int hoveredItemIndex) = 0;
	};


	void setActionListener(ActionListener* iOnC) {
		if (actionListener != NULL)
			delete actionListener;
		onClickFunction = &ActionListener::onClick;
		onHoverFunction = &ActionListener::onHover;
		actionListener = iOnC;
	}

	void onClick() {
		if (actionListener != NULL)
			(actionListener->*onClickFunction)(this, selectedIndex);
	}
	/** Not used in ListBox. */
	virtual void onPress() override {
	};
	/** Action to perform when an item is hovered. */
	virtual void onHover() override {
		if (actionListener != NULL)
			(actionListener->*onHoverFunction)(this, hoveredIndex);
	};
	
	virtual void resetState() override {
	};

private:
	typedef void (ActionListener::*OnClickFunction) (ListBox*, size_t);
	typedef void (ActionListener::*OnHoverFunction) (ListBox*, int);
	OnClickFunction onClickFunction;
	OnHoverFunction onHoverFunction;

	ActionListener* actionListener = NULL;
	/* width of listbox */
	int width;
	/* Always smaller or equal to maxDisplayItems. */
	size_t itemsToDisplay = 0;

	bool isEnumerated = false;
	bool alwaysDisplayScrollBar = false;

	const pugi::char_t* fontName;
	vector<ListItem*> listItems;
	size_t selectedIndex = 0;
	int hoveredIndex = -1;

	size_t itemHeight = 32;
	Vector2 firstItemPos;
	int longestLabelLength = 0;

	unique_ptr<ScrollBar> scrollBar;
	unique_ptr<RectangleFrame> frame;
	size_t firstItemToDisplay = 0;

	/** ID3D11ShaderResourceView is a ComPtr! */
	ComPtr<ID3D11ShaderResourceView> pixel;
	unique_ptr<TexturePanel> texturePanel;
	bool refreshTexture = false;

	int frameThickness = 2;

	EmptyListItem* emptyListItem;

	void setWidth(int newWidth);
	void resizeBox();


};
