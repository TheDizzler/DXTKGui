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
	virtual ~TextLabel();
	virtual void reloadGraphicsAsset() override;

	virtual bool update(double deltaTime) override;
	void draw(SpriteBatch* batch);
	/* Draw with an alternate color.
		NOTE: This draws using the SpriteFont, which is highly inefficient. */
	void draw(SpriteBatch* batch, Color tint);

	virtual unique_ptr<GraphicsAsset> texturize() override;
	virtual void textureDraw(SpriteBatch* batch, ComPtr<ID3D11Device> device = NULL) override;

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
	virtual void setAlpha(const float alpha) override;
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

	void onClick() {
		if (actionListener != NULL) {
			isClicked = isPressed = false;
			(actionListener->*onClickFunction)(this);
		}
	}

	void onHover() {
		if (actionListener != NULL) {
			(actionListener->*onHoverFunction)(this);
		}
	}

protected:
	typedef void (ActionListener::*OnClickFunction) (TextLabel*);
	ActionListener* actionListener = NULL;
	OnClickFunction onClickFunction;
	OnClickFunction onHoverFunction;
	OnClickFunction onPressFunction;

	wstring label;
	shared_ptr<FontSet> font;

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
