#pragma once

#include "Button.h"


class Spinner : public GUIControl {
	friend class SpinnerUpButtonListener;
	friend class SpinnerDownButtonListener;
public:
	/** Width does not include buttons. */
	Spinner(const Vector2& pos, const size_t width, const size_t itemHeight, bool autoSize);
	~Spinner();

	/* Note, if itemHeight is smaller than the font height and/or the button heights (combined)
		this controller will grow to fit everything. */
	void initialize(const pugi::char_t* fontName, GraphicsAsset* pixelAsset,
		const pugi::char_t* upButtonName, const pugi::char_t* downButtonName);

	virtual void update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;


	void addItems(const vector<wstring> items);
	const wstring getSelected() const;

	void increase();
	void decrease();

	/** Not yet implemented. */
	virtual void setScale(const Vector2& scale) override;
	virtual void setFont(const pugi::char_t* font = "Default Font") override;
	/** Not used in Spinner. */
	virtual void setText(wstring text) override;
	/** Returns the longest string in the list. */
	virtual const Vector2 &XM_CALLCONV measureString() const override;

	virtual void moveBy(const Vector2& moveVector) override;
	virtual void setPosition(const Vector2& newPosition) override;

	virtual const Vector2& getPosition() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;

	virtual bool clicked() override;
	virtual bool pressed() override;
	virtual bool hovering() override;


private:

	vector<wstring> list;
	unique_ptr<TextLabel> label;
	unique_ptr<RectangleFrame> frame;
	unique_ptr<RectangleSprite> rectangle;

	unique_ptr<ImageButton> upButton;
	unique_ptr<ImageButton> downButton;

	size_t width;
	short textBuffer = 10;
	size_t itemHeight;
	size_t longestStringLength;

	size_t selected = 0;

	bool autoSize;

};


class SpinnerUpButtonListener : public Button::OnClickListener {
public:
	SpinnerUpButtonListener(Spinner* spnr) : spinner(spnr) {
	}
	virtual void onClick(Button* button) override {
		spinner->increase();
	}
private:
	Spinner* spinner;
};

class SpinnerDownButtonListener : public Button::OnClickListener {
public:
	SpinnerDownButtonListener(Spinner* spnr) : spinner(spnr) {
	}
	virtual void onClick(Button* button) override {
		spinner->decrease();
	}
private:
	Spinner* spinner;
};