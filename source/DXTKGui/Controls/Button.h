#pragma once

#include "TextLabel.h"
#include "../BaseGraphics/PrimitiveShapes.h"


/** A visual and logical representation of a button.
		Now with ActionListeners! */
class Button : public GUIControl {
public:

	Button(GraphicsAsset* pixelAsset, unique_ptr<FontSet> font);
	~Button();

	/* position is topleft of button. */
	void setDimensions(const Vector2& position, const Vector2& size,
		const int frameThickness = 2);


	virtual void update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;

	virtual void setText(wstring text) override;
	virtual const wchar_t* getText() override;
	virtual const Vector2& XM_CALLCONV measureString() const override;

	virtual void setFont(const pugi::char_t* font = "Default Font") override;

	void setTextOffset(const Vector2& unpressedOffset, const Vector2& pressedOffset);
	/* position is topleft of button. */
	virtual void setPosition(const Vector2& position) override;
	virtual const Vector2& getPosition() const override;

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

	class OnClickListener {
	public:
		virtual void onClick(Button* button) = 0;
	};


	void setOnClickListener(OnClickListener* iOnC) {
		if (onClickListener != NULL)
			delete onClickListener;
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

protected:
	typedef void (OnClickListener::*OnClickFunction) (Button*);
	OnClickFunction onClickFunction;
	OnClickListener* onClickListener = NULL;
	OnClickFunction onHoverFunction;
	OnClickListener* onHoverListener = NULL;

	void positionText();
	int width = 0;
	int height = 0;

	virtual void setToUnpressedState();
	virtual void setToHoverState();
	virtual void setToSelectedState();

	unique_ptr<TextLabel> buttonLabel;
	/* Offsets textlabel position.*/
	Vector2 unpressedTextOffset = Vector2::Zero;
	Vector2 pressedTextOffset = Vector2::Zero;
	Vector2 unpressedTextPosition;
	Vector2 pressedTextPosition;

	unique_ptr<RectangleSprite> rectSprite;
	unique_ptr<RectangleFrame> frame;
	int frameThickness = 2;
};


/** A button created with dds files. Requires only an Unpressed image (upButton).
	Pressed button image (downButton) is optional. Defaults to tinting image
	with Button::selectedColor if no downButton is used. */
class ImageButton : public Button {
public:
	ImageButton(unique_ptr<Sprite> buttonSprite,
		unique_ptr<FontSet> font);
	ImageButton(unique_ptr<Sprite> upButtonSprite,
		unique_ptr<Sprite> downButtonSprite, unique_ptr<FontSet> font);
	~ImageButton();

	virtual void draw(SpriteBatch* batch) override;

	/* Changing size will change the scale.*/
	void setDimensions(const Vector2& position, const Vector2& size);

	virtual void setText(wstring text) override;

	virtual void setPosition(const Vector2& position) override;
	virtual void setScale(const Vector2& scale) override;
	/** Remember: Rotation is around the origin! */
	virtual void setRotation(const float rotation) override;

protected:
	virtual void setToUnpressedState() override;
	virtual void setToHoverState() override;
	virtual void setToSelectedState() override;
private:
	unique_ptr<Sprite> normalSprite;
	unique_ptr<Sprite> pressedSprite;

	ID3D11ShaderResourceView* texture;
};


class AnimatedButton : public GUIControl {
public:
	AnimatedButton(shared_ptr<Animation> animation, Vector2 position);
	~AnimatedButton();

	virtual void update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;

	/* Not used in Animated Button. */
	virtual void setFont(const pugi::char_t * font = "Default Font") override;
	/* Not used in Animated Button. */
	virtual void setText(wstring text) override;
	
	virtual const Vector2& XM_CALLCONV measureString() const override;
	virtual const Vector2& getPosition() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;

	virtual bool clicked() override;
	virtual bool pressed() override;
	virtual bool hovering() override;

	virtual void setToUnpressedState();
	virtual void setToHoverState();
	virtual void setToSelectedState();

	class ButtonActionListener {
	public:
		virtual void onClick(AnimatedButton* button) = 0;
		virtual void onPress(AnimatedButton* button) = 0;
		virtual void onHover(AnimatedButton* button) = 0;
		virtual void afterHover(AnimatedButton* button) {
		};
	};


	void setActionListener(ButtonActionListener* iOnC) {
		if (onClickListener != NULL)
			delete onClickListener;
		onClickFunction = &ButtonActionListener::onClick;
		onHoverFunction = &ButtonActionListener::onHover;
		onPressFunction = &ButtonActionListener::onPress;
		onClickListener = iOnC;
	}

	void onClick() {
		if (onClickListener != NULL) {
			(onClickListener->*onClickFunction)(this);
		} else {
				currentFrameIndex = animation->animationFrames.size() - 1;
		}

		isClicked = isPressed = false;
	}

	void onPress() {
		if (onClickListener != NULL) {
			(onClickListener->*onPressFunction)(this);
		} else 
			currentFrameIndex = animation->animationFrames.size() - 2;
	}

	void onHover(double deltaTime) {
		timeHovering += deltaTime;

		if (onClickListener != NULL) {
			(onClickListener->*onHoverFunction)(this);
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
	typedef void (ButtonActionListener::*OnClickFunction) (AnimatedButton*);
	ButtonActionListener* onClickListener = NULL;
	OnClickFunction onClickFunction;
	OnClickFunction onPressFunction;
	OnClickFunction onHoverFunction;




	/** Because frames of animation aren't always the same size... */

	/** The origin point for adjusting position of animated frames. */
	Vector2 center;



};
