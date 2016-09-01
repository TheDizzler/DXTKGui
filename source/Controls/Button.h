#pragma once

#include "../Engine/MouseController.h"
#include "GUIControl.h"
#include "TextLabel.h"
#include "../BaseGraphics/PrimitiveShapes.h"

using namespace std;


/** A visual and logical representation of a button.
Provides no actions on click; that must be handled else where.*/
class Button : public GUIControl {
public:

	Button(ComPtr<ID3D11ShaderResourceView> pixelTexture, unique_ptr<FontSet> font);
	~Button();

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
		/** checkbox: The CheckBox this OnClickListener is attached to.
		isChecked: whether box is checked or no*/
		virtual void onClick(Button* button) = 0;
	};

	typedef void (OnClickListener::*OnClickFunction) (Button*);

	void setOnClickListener(OnClickListener* iOnC) {
		if (onClickListener != NULL)
			delete onClickListener;
		onClickFunction = &OnClickListener::onClick;
		onClickListener = iOnC;
	}

	void triggerOnClick() {
		if (onClickListener != NULL)
			(onClickListener->*onClickFunction)(this);
	}

protected:

	OnClickFunction onClickFunction;
	OnClickListener* onClickListener = NULL;


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


/** A button created with dds files. Requires a Pressed button image (downButton)
	and an Unpressed image (upButton). */
class ImageButton : public Button {
public:
	ImageButton(unique_ptr<Sprite> upButtonSprite,
		unique_ptr<Sprite> downButtonSprite, unique_ptr<FontSet> font);
	~ImageButton();


	virtual void draw(SpriteBatch* batch) override;

	virtual void setPosition(const Vector2& position) override;
	virtual void setScale(const Vector2 & scale) override;

protected:
	virtual void setToUnpressedState() override;
	virtual void setToHoverState() override;
	virtual void setToSelectedState() override;
private:
	unique_ptr<Sprite> normalSprite;
	unique_ptr<Sprite> pressedSprite;
	/* The sprite used in draw(). */
	Sprite* drawSprite;
};
