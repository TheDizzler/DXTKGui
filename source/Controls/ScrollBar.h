#pragma once

#include "Button.h"
#include "../BaseGraphics/PrimitiveShapes.h"

using namespace std;

class Scrubber : public RectangleSprite {
public:
	Scrubber(ComPtr<ID3D11ShaderResourceView> pixel);
	~Scrubber();

	void setDimensions(const Vector2& startPosition,
		const Vector2& size, const int scrollBarHeight);

	virtual void update(double deltaTime, MouseController* mouse);

	void setScrollPositionByCoord(int newCoordinatePosition);
	void setScrollPositionByPercent(float newPositionPercentage);

	void scroll(double increment);

	bool hovering();
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


class ScrollBar : public GUIControl {
public:
	ScrollBar(Vector2 position);
	~ScrollBar();

	bool initialize(ComPtr<ID3D11ShaderResourceView> pixelTexture, size_t maxHeight);
	void setScrollBar(int totalItems, int itemHeight, int maxDisplayItems);

	void update(double deltaTime, MouseController* mouse);
	void draw(SpriteBatch* batch);

	void setScrollPositionByPercent(float newPositionPercentage);
	void setPosition(const Vector2 newPosition);
	/** Call this from parent control on mouse scroll. */
	void scrollByIncrement(int scrollIncrement);

	virtual const Vector2& getPosition() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;

	double percentScroll = 0;

	/* Unused in ScrollBar. */
	virtual void setFont(const pugi::char_t * font = "Default Font") override;
	/* Unused in ScrollBar. */
	virtual void setText(wstring text) override;
	/* Unused in ScrollBar. */
	virtual XMVECTOR XM_CALLCONV measureString() const override;


	virtual bool clicked() override;
	virtual bool selected() override;
	virtual bool hovering() override;

private:

	/* Position of entire scrollbar area. */
	//Vector2 position;

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
	double autoScrollStartDelay = .25; // time in seconds before scrollbar starts scrolling
	double autoScrollDelay = autoScrollStartDelay * .75; // time between increments while autoscrolling

	ComPtr<ID3D11ShaderResourceView> pixel;

};