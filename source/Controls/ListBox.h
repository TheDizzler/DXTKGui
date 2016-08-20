#pragma once

#include <vector>

#include "TextLabel.h"
#include "Button.h"
#include "../BaseGraphics/RectangleSprite.h"

using namespace std;


class ListItem {
public:

	ListItem();
	~ListItem();

	void initialize(const int width, const int height,
		shared_ptr<FontSet> fnt, ComPtr<ID3D11ShaderResourceView> pixelTexture);


	const wchar_t* toString();

	/** Returns true if item selected. */
	bool update(double deltaTime, MouseController* mouse);
	void updatePosition(const Vector2& position);
	virtual void draw(SpriteBatch* batch);


	bool isSelected = false;

protected:

	virtual void setText() = 0;

	//unique_ptr<T> item;
	unique_ptr<TextLabel> textLabel;

	RECT itemRect;
	unique_ptr<HitArea> hitArea;

	/** itemPosition: position of entire Item.
			position: for actual text only. */
	Vector2 itemPosition;

	size_t textMarginX = 10;
	size_t textMarginY = 5;

	bool isHover = false;
	bool buttonDownLast = false;

	ComPtr<ID3D11ShaderResourceView> pixel;

};


class Scrubber : public RectangleSprite {
public:
	Scrubber(ComPtr<ID3D11ShaderResourceView> pixel);
	~Scrubber();

	void setDimensions(const Vector2& startPosition,
		const Vector2& size, const int scrollBarHeight);
	//void setSize(const Vector2& size);
	//void setScrubberHeight(double newHeight);
	virtual void update(double deltaTime, MouseController* mouse);
	//virtual void draw(SpriteBatch* batch);

	void scroll(double increment);

	bool pressed();


	Color normalColor = Color((Vector3(1, 1, 0)));
	Color hoverColor = Color((Vector3(.5, .75, 1)));;
	Color selectedColor = Color((Vector3(0, .5, 1)));;

	double percentAt = 0;

private:

	Vector2 minPosition;
	Vector2 maxPosition;


	int scrollBarHeight;

	bool isHover = false;
	bool isPressed = false;

	int pressedPosition;
	/* Difference between max position and min position. */
	double minMaxDifference;
};


class ScrollBar {
public:
	ScrollBar(Vector2 position);
	~ScrollBar();

	/*bool initialize(ComPtr<ID3D11Device> device,
		ID3D11ShaderResourceView* pixelTexture, size_t maxHeight);*/
	bool initialize(ComPtr<ID3D11ShaderResourceView> pixelTexture, size_t maxHeight);
	void setScrollBar(int totalItems, int itemHeight, int maxDisplayItems);

	void update(double deltaTime, MouseController* mouse);
	void draw(SpriteBatch* batch);

	//void setScrollBar(int totalListHeight);


	int getWidth();

	double percentScroll = 0;
private:

	/* Position of entire scrollbar area. */
	Vector2 position;

	RECT scrollBarRect;
	/* Position of bar part of scroll bar (minus buttons) */
	Vector2 scrollBarPosition;
	unique_ptr<Scrubber> scrubber;

	int maxHeight;

	/* What percent of scroll is equivalent to one item. */
	double percentForOneItem;


	unique_ptr<ImageButton> scrollBarUpButton;
	unique_ptr<ImageButton> scrollBarDownButton;

	double firstClickTimer = 0; // time since button clicked
	double autoScrollStartDelay = .75; // time in seconds before scrollbar starts scrolling
	double autoScrollDelay = autoScrollStartDelay * .75; // time between increments while autoscrolling

	ComPtr<ID3D11ShaderResourceView> pixel;

};

/** A simple control to display various (text) items. */
class ListBox {
public:
	ListBox(const Vector2& position, const int width);
	~ListBox();

	//bool initialize(ComPtr<ID3D11Device> device, const wchar_t* fontFile, ID3D11ShaderResourceView* whitePixel);
	void initialize(shared_ptr<FontSet> font, ComPtr<ID3D11ShaderResourceView> whitePixel);
	void addItems(vector<ListItem*> items);

	/** Returns true if selection changed. */
	bool update(double deltaTime, MouseController* mouse);
	void draw(SpriteBatch* batch);
	//void drawFrame(SpriteBatch* batch);

	void drawSelected(SpriteBatch* batch, const Vector2& selectedPosition);

	void setSelected(size_t newIndex);
	ListItem* getSelected();


	bool multiSelect = false;
	bool alwaysShowScrollBar = false;
	/* Max items to display before showing scroll bar. */
	size_t maxDisplayItems = 7;

private:

	/* width of listbox */
	int width;
	Vector2 position;
	/* Always smaller or equal to maxDisplayItems. */
	size_t itemsToDisplay;


	shared_ptr<FontSet> font;
	vector<ListItem*> listItems;
	size_t selectedIndex = 0;

	size_t itemHeight = 32;
	Vector2 firstItemPos;

	unique_ptr<ScrollBar> scrollBar;
	unique_ptr<RectangleFrame> frame;
	int firstItemToDisplay = 0;

	/** ID3D11ShaderResourceView is a ComPtr! */
	ComPtr<ID3D11ShaderResourceView> pixel;

	int frameThickness = 2;


};
