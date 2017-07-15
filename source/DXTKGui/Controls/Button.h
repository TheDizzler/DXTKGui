#pragma once

#include "TextLabel.h"
#include "../BaseGraphics/PrimitiveShapes.h"


/** A visual and logical representation of a button.
		Now with ActionListeners! */
class Button : public GUIControl, public Texturizable {
public:

	Button(GUIFactory* factory, shared_ptr<MouseController> mouseController,
		const pugi::char_t* font);
	virtual ~Button();

	virtual void reloadGraphicsAsset() override;

	/* position is topleft of button. */
	void setDimensions(const Vector2& position, const Vector2& size,
		const int frameThickness = 2);


	virtual bool update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;

	virtual unique_ptr<GraphicsAsset> texturize() override;
	virtual void textureDraw(SpriteBatch * batch, ComPtr<ID3D11Device> device = NULL) override;


	virtual void setText(wstring text) override;
	virtual const wchar_t* getText() override;
	virtual const Vector2& XM_CALLCONV measureString() const override;

	virtual void setFont(const pugi::char_t* font = "Default Font") override;

	void setTextOffset(const Vector2& unpressedOffset, const Vector2& pressedOffset);
	virtual void moveBy(const Vector2& moveVector) override;
	/* Position is topleft of button. */
	virtual void setPosition(const Vector2& moveVector) override;
	virtual const Vector2& getPosition() const override;

	virtual void setLayerDepth(float newDepth, bool frontToBack = true) override;
	virtual void setScale(const Vector2& scale) override;
	/** NOTE: This DOES NOT return scaled width!
		Use getScaledWidth(). */
	virtual const int getWidth() const override;
	/** NOTE: This DOES NOT return scaled height!!
		Use getScaledHeight(). */
	virtual const int getHeight() const override;

	const int getScaledWidth() const;
	const int getScaledHeight() const;

	/** Colors for imageless button. */
	Color normalColor = Color(1, 1, 1, 1);
	Color hoverColor = Color(1, .75, 0, 1);
	Color selectedColor = Color(1, 0, .4, 1);

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
	};


	void setActionListener(ActionListener* iOnC) {
		if (actionListener != NULL)
			delete actionListener;
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
	bool refreshTexture = true;
	unique_ptr<TexturePanel> texturePanel;

	typedef void (ActionListener::*OnClickFunction) (Button*);
	ActionListener* actionListener = NULL;
	OnClickFunction onClickFunction;
	OnClickFunction onHoverFunction;
	OnClickFunction onPressFunction;

	/* center text and repositions pressed and unpressed text */
	void positionText();
	int width = 0;
	int height = 0;

	virtual void setToUnpressedState();
	virtual void setToHoverState();
	virtual void setToSelectedState();

	unique_ptr<TextLabel> buttonLabel;
	/* Offsets textlabel position.*/
	Vector2 unpressedTextOffset = Vector2(-2, 0);
	Vector2 pressedTextOffset = Vector2(-2, 0);
	Vector2 unpressedTextPosition;
	Vector2 pressedTextPosition;

	unique_ptr<RectangleSprite> rectSprite;
	unique_ptr<RectangleFrame> frame;
	int frameThickness = 2;

	bool hasBeenSetUnpressed = false;
	bool hasBeenSetHover = false;
};


/** A button created with dds files. Requires only an Unpressed image (upButton).
	Pressed button image (downButton) is optional. Defaults to tinting image
	with Button::selectedColor if no downButton is used. */
class ImageButton : public Button {
public:
	ImageButton(GUIFactory* factory, shared_ptr<MouseController> mouseController,
		unique_ptr<Sprite> buttonSprite, const pugi::char_t* font);
	ImageButton(GUIFactory* factory, shared_ptr<MouseController> mouseController,
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
	virtual void setLayerDepth(float newDepth, bool frontToBack = true) override;

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


class AnimatedButton : public GUIControl {
public:
	AnimatedButton(GUIFactory* factory, shared_ptr<MouseController> mouseController,
		shared_ptr<Animation> animation, Vector2 position);
	virtual ~AnimatedButton();

	virtual void reloadGraphicsAsset() override;

	virtual bool update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;

	/* Not used in Animated Button. */
	virtual void setFont(const pugi::char_t * font = "Default Font") override;
	/* Not used in Animated Button. */
	virtual void setText(wstring text) override;

	virtual const Vector2& XM_CALLCONV measureString() const override;
	virtual const Vector2& getPosition() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;

	virtual void setLayerDepth(float newDepth, bool frontToBack = true) override;

	virtual bool clicked() override;
	virtual bool pressed() override;
	virtual bool hovering() override;

	virtual void setToUnpressedState();
	virtual void setToHoverState();
	virtual void setToSelectedState();

	class ActionListener {
	public:
		virtual void onClick(AnimatedButton* button) = 0;
		virtual void onPress(AnimatedButton* button) = 0;
		virtual void onHover(AnimatedButton* button) = 0;
		virtual void afterHover(AnimatedButton* button) {
		};
	};


	void setActionListener(ActionListener* iOnC) {
		if (actionListener != NULL)
			delete actionListener;
		onClickFunction = &ActionListener::onClick;
		onHoverFunction = &ActionListener::onHover;
		onPressFunction = &ActionListener::onPress;
		actionListener = iOnC;
	}

	void onClick() {
		if (actionListener != NULL) {
			(actionListener->*onClickFunction)(this);
		} else {
			currentFrameIndex = animation->animationFrames.size() - 1;
		}

		isClicked = isPressed = false;
	}

	void onPress() {
		if (actionListener != NULL) {
			(actionListener->*onPressFunction)(this);
		} else
			currentFrameIndex = animation->animationFrames.size() - 2;
	}

	void onHover(double deltaTime) {
		timeHovering += deltaTime;

		if (actionListener != NULL) {
			(actionListener->*onHoverFunction)(this);
		} else {
			if (timeHovering > timePerFrame) {
				timeHovering = 0;
				++currentFrameIndex;
				if (currentFrameIndex > animation->animationFrames.size() - 3) {
					currentFrameIndex = animation->animationFrames.size() - 3;
					isOpen = true;
				} else
					adjustPosition(currentFrameIndex - 1);
			}
		}

	}

	double timeHovering = 0;
	double timePerFrame = .167;
	bool isOpen = false;
	int currentFrameIndex = -1;

	shared_ptr<Animation> animation;
	void adjustPosition(int lastFrame);
private:
	typedef void (ActionListener::*OnClickFunction) (AnimatedButton*);
	ActionListener* actionListener = NULL;
	OnClickFunction onClickFunction;
	OnClickFunction onPressFunction;
	OnClickFunction onHoverFunction;



	/** The origin point for adjusting position of animated frames. */
	/** Because frames of animation aren't always the same size... */
	Vector2 center;



};
