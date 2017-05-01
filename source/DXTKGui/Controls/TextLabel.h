#pragma once

#include "GUIControl.h"

class TexturePanel;
class TextLabel : public GUIControl, public Texturizable {
public:
	TextLabel(GUIFactory* factory, shared_ptr<MouseController> mouseController,
		Vector2 position, wstring text, const pugi::char_t* font, bool useTexture = true);
	/** A Textlabel with no set position. Used in PromptDialog. */
	TextLabel(GUIFactory* factory, shared_ptr<MouseController> mouseController,
		wstring text, shared_ptr<FontSet> font, bool useTexture = true);

	~TextLabel();

	virtual void update(double deltaTime) override;
	void draw(SpriteBatch* batch);
	/* Draw with an alternate color. */
	void draw(SpriteBatch* batch, Color tint);

	virtual unique_ptr<GraphicsAsset> texturize() override;
	virtual void textureDraw(SpriteBatch* batch) override;

	virtual const Vector2& getPosition() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;

	const shared_ptr<FontSet> getFont() const;

	virtual void moveBy(const Vector2& moveVector) override;
	virtual void setPosition(const Vector2& position) override;
	virtual void setFont(const pugi::char_t* font = "Default Font") override;
	virtual void setFont(shared_ptr<FontSet> newFont);
	virtual void setTint(const XMFLOAT4 color) override;
	virtual void setTint(const Color& color) override;
	virtual void setTint(const XMVECTORF32 color) override;
	virtual void setScale(const Vector2& scl) override;
	/** bool frontToBack has no effect in TextLabel. */
	virtual void setLayerDepth(const float newDepth, bool frontToBack = true) override;

	void setText(string text);
	void setText(wostringstream& text);
	virtual void setText(wstring text) override;
	/** Special use setText. Use on a throw-away TextLabel with useTexture = false. */
	//virtual void setText(wstring text, bool useTexture);
	virtual const wchar_t* getText() override;
	/* Calculated with scaling. */
	virtual const Vector2& XM_CALLCONV measureString() const override;
	/* Convenience method when a FontSet is not available.
		Scaling is not considered. */
	const Vector2& XM_CALLCONV measureString(wstring string) const;


	virtual bool clicked() override;
	virtual bool pressed() override;
	virtual bool hovering() override;

	void setHoverable(bool hoverable);

	Color normalColorText = Color(Vector3(0, 0, 0));
	Color hoverColorText = Color(Vector3(.5, .75, 1));
	Color selectedColorText = Color(Vector3(0, .5, 1));

	class OnClickListener {
	public:
		virtual void onClick(TextLabel* button) = 0;
	};


	void setOnClickListener(OnClickListener* iOnC) {
		if (onClickListener != NULL)
			delete onClickListener;
		isHoverable = true;
		onClickFunction = &OnClickListener::onClick;
		onClickListener = iOnC;
	}

	void onClick() {
		if (onClickListener != NULL) {
			isClicked = isPressed = false;
			(onClickListener->*onClickFunction)(this);
		}
	}

	void setOnHoverListener(OnClickListener* iOnC) {
		if (!onHoverListener != NULL)
			delete onHoverListener;
		onHoverFunction = &OnClickListener::onClick;
		onHoverListener = iOnC;

	}

	void onHover() {
		if (onHoverListener != NULL) {
			(onHoverListener->*onHoverFunction)(this);
		}
	}

private:

	wstring label;
	shared_ptr<FontSet> font;

	bool useTexture = false;
	unique_ptr<TexturePanel> texturePanel;

	/* Sometimes a TextLabel is just a TextLabel. */
	bool isHoverable = false;

	typedef void (OnClickListener::*OnClickFunction) (TextLabel*);
	OnClickFunction onClickFunction;
	OnClickListener* onClickListener = NULL;
	OnClickFunction onHoverFunction;
	OnClickListener* onHoverListener = NULL;
	// to do: tooltips

};
