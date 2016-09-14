#pragma once

#include "../Engine/MouseController.h"
#include "GUIControl.h"
#include "TextLabel.h"
#include "../BaseGraphics/PrimitiveShapes.h"

using namespace std;


/** A visual and logical representation of a button.
		Now with ActionListeners!.*/
class Button : public GUIControl {
public:

	Button(GraphicsAsset* pixelAsset, unique_ptr<FontSet> font);
	virtual ~Button();

	/* position is topleft of button. */
	void setDimensions(const Vector2& position, const Vector2& size,
		const int frameThickness = 2);

	virtual void update(double deltaTime, MouseController* mouse) override;
	virtual void draw(SpriteBatch* batch) override;

	virtual void setText(wstring text) override;
	virtual XMVECTOR XM_CALLCONV measureString() const override;

	virtual void setFont(const pugi::char_t* font = "Default Font") override;

	void setTextOffset(const Vector2& unpressedOffset, const Vector2& pressedOffset);
	/* position is topleft of button. */
	virtual void setPosition(const Vector2& position) override;
	virtual const Vector2& getPosition() const override;

	virtual void setScale(const Vector2& scale) override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;


	/** Colors for imageless button. */
	Color normalColor = Color(Vector3(1, 1, 1));
	Color hoverColor = Color(Vector3(1, .75, 0));
	Color selectedColor = Color(Vector3(1, 0, .4));

	/** Colors for text on button. */
	Color normalColorText = Color(Vector3(0, 0, 0));
	Color hoverColorText = Color(Vector3(.5, .75, 1));
	Color selectedColorText = Color(Vector3(0, .5, 1));

	virtual bool clicked() override;
	virtual bool selected() override;
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
			isClicked = isSelected = false;
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

	virtual void setPosition(const Vector2& position) override;
	virtual void setScale(const Vector2 & scale) override;
	/** Remember: Rotation is around the origin!
		This is really hacky and should probably not be used! */
	virtual void setRotation(const float rotation) override;

protected:
	virtual void setToUnpressedState() override;
	virtual void setToHoverState() override;
	virtual void setToSelectedState() override;
private:
	unique_ptr<Sprite> normalSprite;
	unique_ptr<Sprite> pressedSprite;
	/* The sprite used in draw(). */
	//Sprite* drawSprite;
	ID3D11ShaderResourceView* texture;
};
