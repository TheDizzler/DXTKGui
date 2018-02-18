#pragma once

#include "TextLabel.h"
#include "../BaseGraphics/PrimitiveShapes.h"


/** A visual and logical representation of a button. */
class Button : public Selectable, public Texturizable {

public:

	Button(GUIFactory* factory, MouseController* mouseController,
		const pugi::char_t* font);
	virtual ~Button();

	virtual void forceRefresh() override;
	virtual void reloadGraphicsAsset() override;

	/* position is topleft of button. */
	void setDimensions(const Vector2& position, const Vector2& size,
		const int frameThickness = 2);

	/* For use in SelectionManager only. Note that mouse behaviour is different. */
	virtual bool updateSelect(double deltaTime) override;
	virtual bool update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;

	virtual unique_ptr<GraphicsAsset> texturize() override;
	virtual void textureDraw(SpriteBatch* batch,
		ComPtr<ID3D11Device> device = NULL) override;

	virtual void setTextLabel(TextLabel* newLabel, bool isLetterJammer);

	virtual void setText(wstring text) override;
	virtual const wchar_t* getText() override;
	virtual const Vector2 XM_CALLCONV measureString() const override;

	virtual void setFont(const pugi::char_t* font = "Default Font") override;

	void setTextOffset(const Vector2& unpressedOffset, const Vector2& pressedOffset);
	virtual void moveBy(const Vector2& moveVector) override;
	/* Position is topleft of button. */
	virtual void setPosition(const Vector2& newPosition) override;
	virtual void setOrigin(const Vector2& origin) override;
	virtual void setRotation(const float rotation) override;
	virtual const Vector2& getPosition() const override;

	virtual void setLayerDepth(const float newDepth, bool frontToBack = true) override;
	virtual void setScale(const Vector2& scale) override;
	/** NOTE: This DOES NOT return scaled width!
	Use getScaledWidth(). */
	virtual const int getWidth() const override;
	/** NOTE: This DOES NOT return scaled height!!
	Use getScaledHeight(). */
	virtual const int getHeight() const override;

	const int getScaledWidth() const;
	const int getScaledHeight() const;

	virtual void setUnpressedColor(const Color& newColor);
	virtual void setPressedColor(const Color& newColor);
	virtual void setHoverColor(const Color& newColor);

	/** Colors for text on button. */
	Color normalColorText = Color(0, 0, 0, 1);
	Color hoverColorText = Color(.5, .75, 1, 1);
	Color selectedColorText = Color(0, .5, 1, 1);

	virtual bool clicked() override;
	virtual bool pressed() override;
	virtual bool hovering() override;

	class ActionListener {
	public:
		virtual void onClick(Button* button) = 0;
		virtual void onPress(Button* button) = 0;
		virtual void onHover(Button* button) = 0;
		virtual void resetState(Button* button) = 0;
	};


	void setActionListener(ActionListener* iOnC);

	virtual void onClick() override;
	virtual void onPress() override;
	virtual void onHover() override;
	virtual void resetState() override;

	unique_ptr<TextLabel> buttonLabel;
protected:
	bool refreshTexture = true;
	unique_ptr<TexturePanel> texturePanel;

	typedef void (ActionListener::*OnClickFunction) (Button*);
	ActionListener* actionListener = NULL;
	OnClickFunction onClickFunction;
	OnClickFunction onHoverFunction;
	OnClickFunction onPressFunction;
	OnClickFunction onResetFunction;

	/* center text and repositions pressed and unpressed text */
	void positionText();
	int width = 0;
	int height = 0;

	virtual void setToUnpressedState();
	virtual void setToHoverState();
	virtual void setToSelectedState();

	/** Colors for imageless button. */
	Color normalColor = Color(1, 1, 1, 1);
	Color hoverColor = Color(1, .75f, 0, 1);
	Color selectedColor = Color(1, 0, .4f, 1);

	/* Offsets textlabel position.*/
	Vector2 unpressedTextOffset = Vector2(-2, 0);
	Vector2 pressedTextOffset = Vector2(-2, 0);
	Vector2 unpressedTextPosition;
	Vector2 pressedTextPosition;

	unique_ptr<RectangleSprite> rectSprite;
	unique_ptr<RectangleFrame> frame;
	int frameThickness = 2;

	bool isLetterJammer = false;
	bool mouseHover = false;
	//bool lastWasHover = false;
	/** Flag to prevent continuous texture refresh. */
	bool hasBeenSetUnpressed = false;
	/** Flag to prevent continuous texture refresh. */
	bool hasBeenSetHover = false;
	bool hasBeenReset = true;
};


/** A button created with dds files. Requires only an Unpressed image (upButton).
Pressed button image (downButton) is optional. Defaults to tinting image
with Button::selectedColor if no downButton is used. */
class ImageButton : public Button {
public:
	ImageButton(GUIFactory* factory, MouseController* mouseController,
		unique_ptr<Sprite> buttonSprite, const pugi::char_t* font);
	ImageButton(GUIFactory* factory, MouseController* mouseController,
		unique_ptr<Sprite> upButtonSprite,
		unique_ptr<Sprite> downButtonSprite, const pugi::char_t* font);
	virtual ~ImageButton();

	virtual void reloadGraphicsAsset() override;

	virtual void draw(SpriteBatch* batch) override;

	virtual void textureDraw(SpriteBatch * batch, ComPtr<ID3D11Device> device = NULL) override;

	/* Changing size will change the scale.*/
	void setDimensions(const Vector2& position, const Vector2& size);

	virtual void setText(wstring text) override;

	virtual void moveBy(const Vector2& moveVector) override;
	virtual void setPosition(const Vector2& position) override;
	virtual void setScale(const Vector2& scale) override;
	/** Remember: Rotation is around the origin! */
	virtual void setRotation(const float rotation) override;
	virtual void setLayerDepth(const float newDepth, bool frontToBack = true) override;

protected:
	virtual void setToUnpressedState() override;
	virtual void setToHoverState() override;
	virtual void setToSelectedState() override;
private:
	unique_ptr<Sprite> normalSprite;
	unique_ptr<Sprite> pressedSprite;

	ID3D11ShaderResourceView* texture;
	RECT sourceRect;
};



