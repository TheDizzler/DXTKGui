#pragma once

//#include <vector>

#include "../BaseGraphics/PrimitiveShapes.h"
#include "Button.h"



	/** Creates a Dialog Box using a custom dds background file. */
	/*class CustomBGDialog : public Dialog {
	public:
		CustomBGDialog(const Vector2& position);

		bool initialize(ComPtr<ID3D11Device> device, const wchar_t* fontFile,
			const wchar_t* bgFile);

	private:

		unique_ptr<Sprite> bgSprite;


	};*/

/** A pop-up Dialog Box. Title area and button area
	automatically adjust to size of their components. */
class Dialog : GUIControlBox {
public:

	Dialog();
	~Dialog();

	void initialize(ComPtr<ID3D11ShaderResourceView> pixelTexture,
		const pugi::char_t* font = "Default Font");

	void setDimensions(const Vector2& position, const Vector2& size,
		const int frameThickness = 2);

	void setTitle(wstring text, const Vector2& scale = Vector2(1.5, 1.5),
		const pugi::char_t* font = "Default Font");
	virtual void setText(wstring text) override;
	/** Adds pre-created button to dialog as confirm button.
		Note: user must initalize everything except for position. */
	void setConfirmButton(unique_ptr<Button> okButton);
	void setConfirmButton(wstring text, const pugi::char_t* font = "Default Font");

	void setCancelButton(unique_ptr<Button> cancelButton);
	void setCancelButton(wstring text, const pugi::char_t* font = "Default Font");

	virtual void update(double deltaTime, MouseController* mouse);
	virtual void draw(SpriteBatch* batch);

	virtual void addItem(unique_ptr<GUIControl> control) override;
	virtual void addItems(vector<unique_ptr<GUIControl>> controls) override;

	virtual void setFont(const pugi::char_t* font = "Default Font") override;
	virtual void setTint(const Color& color) override;

	virtual const Vector2& getPosition() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;

	virtual bool clicked() override;
	virtual bool selected() override;
	virtual bool hovering() override;

	/** Not used in DialogBox */
	virtual XMVECTOR XM_CALLCONV measureString() const override;

	void open();
	void close();
	ClickAction getResult();

	bool isOpen = false;

private:

	enum GUIControlLookUp {
		TitleText, DialogText, ButtonOK, ButtonNeutral, ButtonCancel, Other
	};

	vector<unique_ptr<GUIControl> > controls;

	ClickAction result = NONE;

	Vector2 size;
	Vector2 titleFrameSize = Vector2(0, 40);
	Vector2 titleFramePosition;
	Vector2 dialogFrameSize;
	Vector2 dialogFramePosition;
	Vector2 buttonFramePosition;
	Vector2 buttonFrameSize = Vector2(0, 50);

	int frameThickness = 2;
	int buttonMargin = 10; // space between edge of dialog box and button edge
	Vector2 okButtonPosition;
	Vector2 cancelButtonPosition;
	Vector2 neutralButtonPosition;

	void calculateTitlePos();
	void calculateDialogTextPos();
	bool calculateButtonPosition(Vector2& buttonPos);
	int getMaxButtonHeight();

	/** Used for dragging dialog around, if draggable set. */
	void setPosition(const Vector2& position);

	unique_ptr<RectangleSprite> bgSprite;
	unique_ptr<RectangleFrame> frame;
	unique_ptr<RectangleSprite> titleSprite;
	unique_ptr<RectangleSprite> buttonFrameSprite;

	Vector2 standardButtonSize = Vector2(150, 35);
};