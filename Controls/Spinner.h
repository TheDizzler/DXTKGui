#pragma once

#include "Button.h"


class Spinner : public GUIControl, public Texturizable {
	friend class SpinnerUpButtonListener;
	friend class SpinnerDownButtonListener;
public:
	/** Width does not include buttons. */
	Spinner(GUIFactory* factory, MouseController* mouseController,
		const Vector2& pos, const size_t width, const size_t itemHeight, bool autoSize);
	virtual ~Spinner();

	/* Note, if itemHeight is smaller than the font height and/or the button heights (combined)
		this controller will grow to fit everything. */
	void initialize(const pugi::char_t* fontName,
		const pugi::char_t* upButtonName, const pugi::char_t* downButtonName);

	virtual void forceRefresh() override;
	virtual void reloadGraphicsAsset() override;

	virtual unique_ptr<GraphicsAsset> texturize() override;
	virtual void textureDraw(SpriteBatch* batch, ComPtr<ID3D11Device> device = NULL) override;

	virtual bool update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;

	void addItem(wstring item);
	void addItems(const vector<wstring> items);
	/** Returns true if item found and removed from list. */
	bool removeItem(wstring item);
	const wstring getSelected() const;

	void increase();
	void decrease();

	virtual void setLayerDepth(const float newDepth, bool frontToBack = true) override;
	/** Not yet implemented. */
	virtual void setScale(const Vector2& scale) override;
	virtual void setFont(const pugi::char_t* font = "Default Font") override;
	/** Not used in Spinner. */
	virtual void setText(wstring text) override;
	/** Returns the longest string in the list. */
	virtual const Vector2 XM_CALLCONV measureString() const override;

	virtual void moveBy(const Vector2& moveVector) override;
	virtual void setPosition(const Vector2& newPosition) override;

	virtual const Vector2& getPosition() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;

	virtual bool clicked() override;
	virtual bool pressed() override;
	virtual bool hovering() override;

	/** Does nothing in Spinner. */
	virtual void onClick() override {};
	/** Does nothing in Spinner. */
	virtual void onPress() override {};
	/** Does nothing in Spinner. */
	virtual void onHover() override {};
	virtual void resetState() override {};

private:
	unique_ptr<TexturePanel> texturePanel;
	bool refreshTexture = true;

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


class SpinnerUpButtonListener : public Button::ActionListener {
public:
	SpinnerUpButtonListener(Spinner* spnr) : spinner(spnr) {
	}

	virtual void onClick(Button* button) override;
	virtual void onPress(Button* button) override;
	virtual void onHover(Button* button) override;
	virtual void resetState(Button* button) override;
private:
	Spinner* spinner;
};

class SpinnerDownButtonListener : public Button::ActionListener {
public:
	SpinnerDownButtonListener(Spinner* spnr) : spinner(spnr) {
	}
	virtual void onClick(Button* button) override;
	virtual void onPress(Button* button) override;
	virtual void onHover(Button* button) override;
	virtual void resetState(Button* button) override;
private:
	Spinner* spinner;
};