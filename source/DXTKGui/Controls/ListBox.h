#pragma once

#include "ScrollBar.h"


class ListItem {
public:
	virtual ~ListItem();

	void initialize(const int width, const int height,
		TextLabel* label, ComPtr<ID3D11ShaderResourceView> pixelTexture,
		size_t listPosition = 0, bool enumerateList = false);

	void setWidth(int newWidth);

	Vector2 measureString() const;
	const wchar_t* toString();

	/** Returns true if item pressed. */
	bool update(double deltaTime, MouseController* mouse);
	void updatePosition(const Vector2& position);
	virtual void draw(SpriteBatch* batch);


	void setSelected(bool isSelected);
	bool isSelected = false;

protected:

	Color selectedFontColor = Color(0, 0, 0, 1);
	Color normalFontColor = Color(1, 1, 1, 1);

	virtual void setText() = 0;
	unique_ptr<TextLabel> textLabel;

	RECT itemRect;
	unique_ptr<HitArea> hitArea;

	/** itemPosition: position of entire Item.
			position: for text only. */
	Vector2 itemPosition;

	float layerDepth = 1;

	size_t textMarginX = 10;
	size_t textMarginY = 5;

	bool isHover = false;
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
	ListBox(GUIFactory* factory, shared_ptr<MouseController> mouseController,
		const Vector2& position, const int width,
		size_t itemHeight = 32, const int maxItemsShown = 7);
	virtual ~ListBox();

	void initialize(const pugi::char_t* fontName,
		GraphicsAsset* pixelAsset, ScrollBar* scrollBar,
		bool enumerateList = false);


	void setScrollBar(ScrollBarDesc& scrollBarDesc);
	void alwaysShowScrollBar(bool alwaysShow);
	/* Note: this function has not been tested much....not at all really...*/
	void addItem(ListItem* item);
	void addItems(vector<ListItem*> items);
	void clear();

	virtual bool update(double deltaTime) override;
	void draw(SpriteBatch* batch);

	virtual unique_ptr<GraphicsAsset> texturize() override;
	virtual void textureDraw(SpriteBatch* batch, ComPtr<ID3D11Device> device = NULL) override;


	void setSelected(size_t newIndex);
	const int getSelectedIndex() const;
	ListItem* getSelected();
	ListItem* getItem(size_t index);


	/* Not used in ListBox. */
	virtual void setText(wstring text);
	/* Not used in ListBox. */
	virtual const Vector2& XM_CALLCONV measureString() const;

	bool multiSelect = false;

	/* Max items to display before showing scroll bar. */
	size_t maxDisplayItems = 7;


	virtual void moveBy(const Vector2& moveVector) override;
	virtual void setPosition(const Vector2& position) override;

	virtual void setFont(const pugi::char_t* font = "Default Font") override;
	virtual const Vector2 & getPosition() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;

	virtual bool clicked() override;
	virtual bool pressed() override;
	virtual bool hovering() override;

	class ActionListener {
	public:
		/** listbox: The ListBox this ActionListener is attached to.
		selectedItemIndex: index of item in ListBox.*/
		virtual void onClick(ListBox* listbox, int selectedItemIndex) = 0;
	};

	


	void setActionListener(ActionListener* iOnC) {
		if (actionListener != NULL)
			delete actionListener;
		onClickFunction = &ActionListener::onClick;
		actionListener = iOnC;
	}

	void onClick() {
		if (actionListener != NULL)
			(actionListener->*onClickFunction)(this, selectedIndex);
	}


private:
	typedef void (ActionListener::*OnClickFunction) (ListBox*, int);
	OnClickFunction onClickFunction;
	ActionListener* actionListener = NULL;
	/* width of listbox */
	int width;
	/* Always smaller or equal to maxDisplayItems. */
	size_t itemsToDisplay = 0;

	boolean isEnumerated = false;
	bool alwaysDisplayScrollBar = false;

	const pugi::char_t* fontName;
	vector<ListItem*> listItems;
	size_t selectedIndex = 0;

	size_t itemHeight = 32;
	Vector2 firstItemPos;
	int longestLabelLength = 0;

	unique_ptr<ScrollBar> scrollBar;
	unique_ptr<RectangleFrame> frame;
	int firstItemToDisplay = 0;

	/** ID3D11ShaderResourceView is a ComPtr! */
	ComPtr<ID3D11ShaderResourceView> pixel;
	unique_ptr<TexturePanel> texturePanel;
	bool refreshPanel = false;

	int frameThickness = 2;

	EmptyListItem* emptyListItem;

	void setWidth(int newWidth);
	void resizeBox();


};
