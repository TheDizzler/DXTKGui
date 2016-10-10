#pragma once

//#include <vector>

#include "ScrollBar.h"


class ListItem {
public:

	ListItem();
	~ListItem();

	void initialize(const int width, const int height,
		shared_ptr<FontSet> fnt, ComPtr<ID3D11ShaderResourceView> pixelTexture,
		size_t listPosition = 0, bool enumerateList = false);

	void setWidth(int newWidth);

	Vector2 measureString() const;
	const wchar_t* toString();

	/** Returns true if item pressed. */
	bool update(double deltaTime, MouseController* mouse);
	void updatePosition(const Vector2& position);
	virtual void draw(SpriteBatch* batch);



	bool isPressed = false;

protected:

	virtual void setText() = 0;
	unique_ptr<TextLabel> textLabel;

	RECT itemRect;
	unique_ptr<HitArea> hitArea;

	/** itemPosition: position of entire Item.
			position: for text only. */
	Vector2 itemPosition;

	size_t textMarginX = 10;
	size_t textMarginY = 5;

	bool isHover = false;
	bool buttonDownLast = false;

	ComPtr<ID3D11ShaderResourceView> pixel;

	bool isEnumerated;
	size_t listPosition = 0;
};

class EmptyListItem : public ListItem {
public:
	virtual void setText() override {

		textLabel->setText("Empty...");
	}
};


/** A simple control to display various (text) items. */
class ListBox : public GUIControl {
	//friend class ComboBox;
public:
	ListBox(const Vector2& position, const int width,
		size_t itemHeight = 32, const int maxItemsShown = 7);
	~ListBox();

	void initialize(shared_ptr<FontSet> font,
		GraphicsAsset* pixelAsset, ScrollBar* scrollBar,
		bool enumerateList = false);

	void setScrollBar(ScrollBarDesc& scrollBarDesc);
	void alwaysShowScrollBar(bool alwaysShow);
	/* Note: this function has not been tested much....not at all really...*/
	void addItem(ListItem* item);
	void addItems(vector<ListItem*> items);
	void clear();

	virtual void update(double deltaTime) override;
	void draw(SpriteBatch* batch);


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



	virtual void setFont(const pugi::char_t* font = "Default Font") override;
	virtual const Vector2 & getPosition() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;

	virtual bool clicked() override;
	virtual bool pressed() override;
	virtual bool hovering() override;

	class OnClickListener {
	public:
		/** listbox: The ListBox this OnClickListener is attached to.
		selectedItemIndex: index of item in ListBox.*/
		virtual void onClick(ListBox* listbox, int selectedItemIndex) = 0;
	};

	typedef void (OnClickListener::*OnClickFunction) (ListBox*, int);


	void setOnClickListener(OnClickListener* iOnC) {
		if (onClickListener != NULL)
			delete onClickListener;
		onClickFunction = &OnClickListener::onClick;
		onClickListener = iOnC;
	}

	void onClick() {
		if (onClickListener != NULL)
			(onClickListener->*onClickFunction)(this, selectedIndex);
	}


private:

	/* width of listbox */
	int width;
	Vector2 position;
	/* Always smaller or equal to maxDisplayItems. */
	size_t itemsToDisplay = 0;

	boolean isEnumerated = false;
	bool alwaysDisplayScrollBar = false;

	shared_ptr<FontSet> font;
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

	int frameThickness = 2;

	EmptyListItem* emptyListItem;

	OnClickFunction onClickFunction;
	OnClickListener* onClickListener = NULL;

	void setWidth(int newWidth);
	void resizeBox();

};
