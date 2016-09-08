#pragma once

#include "Button.h"
#include "../BaseGraphics/PrimitiveShapes.h"

using namespace std;

class Scrubber : public RectangleSprite {
public:
	/* If using an image file instead of a pixel, set staticImage to true. */
	Scrubber(GraphicsAsset* const graphicsAsset, bool assetIsPixel = true);
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
	bool assetIsPixel;
	Vector2 minPosition;
	Vector2 maxPosition;


	int scrollBarHeight;

	bool isHover = false;
	bool isPressed = false;

	int pressedPosition;
	/* Difference between max position and min position. */
	double minMaxDifference;
};



/** Do not use this classes HitArea - it is NULL. */
class ScrollBar : public GUIControl {
public:
	ScrollBar(Vector2 position);
	~ScrollBar();

	/* scrollBarButtons[0] = Up Button
		scrollBarButtons[1] = Down Button */
	bool initialize(GraphicsAsset* const pixelAsset, size_t maxHeight,
		ImageButton* scrollBarButtons[2] = NULL, unique_ptr<Sprite> scrollBarTrack = NULL, 
		GraphicsAsset* scrubber = NULL);
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

	unique_ptr<Sprite> scrollBarTrack;
	unique_ptr<Scrubber> scrubber;

	int maxHeight;

	/* What percent of scroll is equivalent to one item. */
	double percentForOneItem;


	unique_ptr<ImageButton> scrollBarUpButton;
	unique_ptr<ImageButton> scrollBarDownButton;

	double firstClickTimer = 0; // time since button clicked
	double autoScrollStartDelay = .25; // time in seconds before scrollbar starts scrolling
	double autoScrollDelay = autoScrollStartDelay * .75; // time between increments while autoscrolling

	//ComPtr<ID3D11ShaderResourceView> pixel;

};