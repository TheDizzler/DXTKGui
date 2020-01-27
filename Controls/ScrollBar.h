#pragma once

#include "Button.h"


class Scrubber : public RectangleSprite {
public:
	/* If using an image file instead of a pixel, set staticImage to true. */
	Scrubber(GraphicsAsset* const graphicsAsset, bool assetIsPixel = true);
	virtual ~Scrubber();


	void setDimensions(const Sprite* scrollBarTrack,
		double percentShowing, double maxPercent);

	virtual bool update(double deltaTime, MouseController* mouse);

	virtual void setPosition(const Vector2& position) override;
	virtual void moveBy(const Vector2& moveVector) override;

	void setScrollPositionByCoord(float newCoordinatePosition);
	void setScrollPositionByPercent(double newPositionPercentage);

	void scroll(double itemIncrement, double scrubberIncrement);

	bool hovering();
	bool pressed();


	Color normalColor = Color(1, 1, 1);
	Color hoverColor = Color(.5, .75, 1);
	Color selectedColor = Color(0, .5, 1);

	double percentAt = 0;

private:
	int minPosition = 0;
	int maxPosition = 0;

	int scrollBarHeight;
	double scrubberPercentAt = 0;
	double maxPercent = 0;
	double percentDifference = 0;
	bool isHover = false;
	bool isPressed = false;

	int pressedPosition;
	/* Difference between max position and min position. */
	double minMaxDifference;
};

struct ScrollBarDesc {
	/* Default: "ScrollBar Up" */
	string upButtonImage = "ScrollBar Up";
	/* Optional */
	string upPressedButtonImage = "";
	/* Optional */
	string downButtonImage = "";
	/* Optional */
	string downPressedButtonImage = "";
	/* Optional */
	string trackImage = "";
	/* Optional */
	string scrubberImage = "";
};



/** A vertical scrollbar for scrolling contents of a window.
	NOTE: Do not use this classes HitArea - it is NULL. */
class ScrollBar : public GUIControl {
public:
	ScrollBar(GUIFactory* factory, MouseController* mouseController,
		const Vector2& position);
	virtual ~ScrollBar();

	/* scrollBarButtons[0] = Up Button
		scrollBarButtons[1] = Down Button */
	bool initialize(GraphicsAsset* const pixelAsset, size_t barHeight,
		ImageButton* scrollBarButtons[2] = NULL, unique_ptr<Sprite> scrollBarTrack = NULL,
		GraphicsAsset* scrubber = NULL);

	virtual void reloadGraphicsAsset() override;

	void setScrollBar(size_t totalItems, size_t itemHeight, size_t maxDisplayItems);

	virtual bool update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;

	double getPercentScroll();

	void setScrollPositionByPercent(double newPositionPercentage);
	virtual void moveBy(const Vector2& moveVector) override;
	virtual void setPosition(const Vector2& newPosition) override;
	//void resetPositionTo(const Vector2& newPosition);

	void setBarHeight(size_t barHeight);
	/** Call this from parent control on mouse scroll. */
	void scrollByIncrement(int scrollIncrement);

	virtual const Vector2& getPosition() const override;
	const Vector2 getSize() const;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;

	

	virtual void setLayerDepth(const float depth, bool frontToBack = true) override;
	virtual void setScale(const Vector2& newScale) override;
	/* Unused in ScrollBar. */
	virtual void setFont(const pugi::char_t * font = "Default Font") override;
	/* Unused in ScrollBar. */
	virtual void setText(wstring text) override;
	/* Unused in ScrollBar. */
	virtual const Vector2 XM_CALLCONV measureString() const override;

	virtual bool clicked() override;
	virtual bool pressed() override;
	virtual bool hovering() override;

	/** Not used in Scrollbar. */
	virtual void onClick() override {
	};
	/** Not used in Scrollbar. */
	virtual void onPress() override {
	};
	/** Not used in Scrollbar. */
	virtual void onHover() override {
	};
	/** Not used in Scrollbar. */
	virtual void resetState() override {
	};

private:

	unique_ptr<Sprite> scrollBarTrack;
	unique_ptr<Scrubber> scrubber;
	unique_ptr<ImageButton> scrollBarUpButton;
	unique_ptr<ImageButton> scrollBarDownButton;

	size_t barHeight;

	/* What percent of scroll is equivalent to one item. */
	double percentForOneItem;
	/* Percentage of scubber movement is equivalent to item. */
	double scrubberPercentForOneItem;

	double percentScroll = 0;

	double firstClickTimer = 0; // time since button clicked
	double autoScrollStartDelay = .25; // time in seconds before scrollbar starts scrolling
	double autoScrollDelay = autoScrollStartDelay * .75; // time between increments while autoscrolling

};