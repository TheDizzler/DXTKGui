#pragma once

#include "GUIControl.h"

class TexturePanel;
class TextLabel : public GUIControl, public Texturizable {
public:
	TextLabel(GUIFactory* factory, MouseController* mouseController,
		Vector2 position, wstring text, const pugi::char_t* font, bool useTexture = true);
	/** A Textlabel with no set position. Used in PromptDialog. */
	TextLabel(GUIFactory* factory, MouseController* mouseController,
		wstring text, unique_ptr<FontSet> font, bool useTexture = true);
	TextLabel(GUIFactory* factory, MouseController* mouseController,
		wstring text, const pugi::char_t* font, bool useTexture = true);

	virtual ~TextLabel();

	virtual void forceRefresh() override;
	virtual void reloadGraphicsAsset() override;

	virtual bool update(double deltaTime) override;
	void draw(SpriteBatch* batch);
	/* Draw with an alternate color.
		NOTE: This draws using the SpriteFont, which is highly inefficient. */
	void draw(SpriteBatch* batch, Color tint);

	virtual unique_ptr<GraphicsAsset> texturize() override;
	virtual void textureDraw(SpriteBatch* batch, ComPtr<ID3D11Device> device = NULL) override;

	virtual bool contains(const Vector2& point) const;
	virtual const Vector2& getPosition() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;

	const pugi::char_t* getFont() const;

	virtual void moveBy(const Vector2& moveVector) override;
	virtual void setPosition(const Vector2& position) override;
	virtual void setFont(const pugi::char_t* font = "Default Font") override;
	virtual void setFont(unique_ptr<FontSet> newFont);
	virtual void setTint(const XMFLOAT4 color) override;
	virtual void setTint(const Color& color) override;
	virtual void setTint(const XMVECTORF32 color) override;
	virtual void setAlpha(const float alpha) override;
	virtual void setScale(const Vector2& scl) override;
	/** bool frontToBack has no effect in TextLabel. */
	virtual void setLayerDepth(const float newDepth, bool frontToBack = true) override;

	virtual void setText(string text);
	virtual void setText(wostringstream& text);
	virtual void setText(wstring text) override;
	/** Special use setText. Use on a throw-away TextLabel with useTexture = false. */
	//virtual void setText(wstring text, bool useTexture);
	virtual const wchar_t* getText() override;
	/* Calculated with scaling. */
	virtual const Vector2 XM_CALLCONV measureString() const override;
	/* Convenience method when a FontSet is not available.
		Scaling is not considered. */
	const Vector2 XM_CALLCONV measureString(wstring string) const;


	virtual bool clicked() override;
	virtual bool pressed() override;
	virtual bool hovering() override;

	void setHoverable(bool hoverable);

	Color normalColorText = Color(Vector3(0, 0, 0));
	Color hoverColorText = Color(Vector3(.5, .75, 1));
	Color selectedColorText = Color(Vector3(0, .5, 1));

	class ActionListener {
	public:
		virtual void onClick(TextLabel* button) = 0;
		virtual void onPress(TextLabel* button) = 0;
		virtual void onHover(TextLabel* button) = 0;
	};


	void setActionListener(ActionListener* iOnC) {
		if (actionListener != NULL)
			delete actionListener;
		isHoverable = true;
		onClickFunction = &ActionListener::onClick;
		onHoverFunction = &ActionListener::onHover;
		onPressFunction = &ActionListener::onPress;
		actionListener = iOnC;
	}

	virtual void onClick() override {
		isClicked = true;
		isPressed = false;
		if (actionListener != NULL) {
			isClicked = false;
			(actionListener->*onClickFunction)(this);
		}
		setToUnpressedState();
		hasBeenSetUnpressed = false;
		hasBeenSetHover = false;
	}
	virtual void onPress() override {
		isPressed = true;
		if (actionListener != NULL) {
			(actionListener->*onPressFunction)(this);
		}
		setToSelectedState();
		hasBeenSetUnpressed = false;
		hasBeenSetHover = false;
	}
	virtual void onHover() override {
		if (actionListener != NULL) {
			(actionListener->*onHoverFunction)(this);
		}
		setToHoverState();
		hasBeenSetHover = true;
		hasBeenSetUnpressed = false;
	}
	virtual void resetState() override {
		isPressed = false;
		setToUnpressedState();
		hasBeenSetUnpressed = true;
		hasBeenSetHover = false;
	}

protected:
	typedef void (ActionListener::*OnClickFunction) (TextLabel*);
	ActionListener* actionListener = NULL;
	OnClickFunction onClickFunction;
	OnClickFunction onHoverFunction;
	OnClickFunction onPressFunction;

	wstring label;
	unique_ptr<FontSet> font;

	bool useTexture = false;
	bool refreshTexture = true;
	unique_ptr<TexturePanel> texturePanel;

	/* Sometimes a TextLabel is just a TextLabel. */
	bool isHoverable = false;
	virtual void setToUnpressedState();
	virtual void setToHoverState();
	virtual void setToSelectedState();
	bool hasBeenSetUnpressed = false;
	bool hasBeenSetHover = false;

	
};
