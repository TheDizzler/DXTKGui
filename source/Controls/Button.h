#pragma once

#include "../Engine/MouseController.h"
#include "GUIControl.h"
#include "TextLabel.h"

using namespace std;


/** A visual and logical representation of a button.
Provides no actions on click; that must be handled else where.*/
class Button : public GUIControl {
public:

	Button();
	~Button();

	void load(shared_ptr<FontSet> font,
		ComPtr<ID3D11ShaderResourceView> pixelTexture);


	virtual void update(double deltaTime, MouseController* mouse) override;
	virtual void draw(SpriteBatch* batch) override;

	void setText(string text);

	
	virtual void setFont(shared_ptr<FontSet> newFont);
	
	void setDimensions(const Vector2& position, const Vector2& size);
	virtual void setPosition(Vector2& position) override;
	virtual const Vector2& getPosition() override;

	virtual void setScale(const Vector2& scale) override;
	virtual int getWidth() override;
	virtual int getHeight() override;

	/** Colors for imageless button. */
	Color normalColor = Color((Vector3(.501, .501, .501)));
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

	ComPtr<ID3D11ShaderResourceView> pixel;

	shared_ptr<FontSet> buttonFont;
	unique_ptr<TextLabel> buttonLabel;

	RECT buttonRect;

	
};


/** A button created with dds files. Requires a Pressed button image (downButton)
	and an Unpressed image (upButton). */
class ImageButton : public Button {
public:
	ImageButton();
	~ImageButton();

	void load(shared_ptr<FontSet> font, shared_ptr<Sprite> upButtonSprite,
		shared_ptr<Sprite> downButtonSprite);


	virtual void draw(SpriteBatch* batch) override;

	virtual void setScale(const Vector2 & scale) override;
private:
	shared_ptr<Sprite> normalSprite;
	shared_ptr<Sprite> pressedSprite;
};


//class TextButton : public Button {
//public:
//	TextButton();
//	~TextButton();


//	/*bool load(ID3D11Device* device, const wchar_t* fontFile,
//		const wchar_t* upButtonFile, const wchar_t* downButtonFile);*/


//	virtual void setPosition(Vector2& position);
//	void setText(string text);
//	virtual void draw(SpriteBatch* batch);

//private:

//	shared_ptr<FontSet> buttonFont;
//	unique_ptr<TextLabel> buttonLabel;
//};