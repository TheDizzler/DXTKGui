#pragma once

#include "../Engine/MouseController.h"
#include "GUIControl.h"
#include "TextLabel.h"
#include "../BaseGraphics/RectangleSprite.h"

using namespace std;


/** A visual and logical representation of a button.
Provides no actions on click; that must be handled else where.*/
class Button : public GUIControl {
public:

	Button();
	~Button();

	void load(unique_ptr<FontSet> font,
		ComPtr<ID3D11ShaderResourceView> pixelTexture);

	void setDimensions(const Vector2& position, const Vector2& size,
		const int frameThickness);

	virtual void update(double deltaTime, MouseController* mouse) override;
	virtual void draw(SpriteBatch* batch) override;

	void setText(string text);


	virtual void setFont(unique_ptr<FontSet> newFont) override;


	virtual void setPosition(const Vector2& position) override;
	virtual const Vector2& getPosition() const override;

	virtual void setScale(const Vector2& scale) override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;

	/** Colors for imageless button. */
	Color normalColor = Color((Vector3(1, 1, 1)));
	Color hoverColor = Color((Vector3(1, .75, 0)));
	Color selectedColor = Color((Vector3(1, 0, .4)));

	/** Colors for text on button. */
	Color normalColorText = Color((Vector3(1, 1, 1)));
	Color hoverColorText = Color((Vector3(.5, .75, 1)));
	Color selectedColorText = Color((Vector3(0, .5, 1)));

	virtual bool clicked() override;
	virtual bool selected() override;
	virtual bool hovering() override;

protected:

	virtual void setToUnpressedState();
	virtual void setToHoverState();
	virtual void setToSelectedState();

	unique_ptr<TextLabel> buttonLabel;

	unique_ptr<RectangleSprite> rectSprite;
	unique_ptr<RectangleFrame> frame;
};


/** A button created with dds files. Requires a Pressed button image (downButton)
	and an Unpressed image (upButton). */
class ImageButton : public Button {
public:
	ImageButton();
	~ImageButton();

	void load(unique_ptr<FontSet> font, unique_ptr<Sprite> upButtonSprite,
		unique_ptr<Sprite> downButtonSprite);


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
	Sprite* drawSprite;
};
