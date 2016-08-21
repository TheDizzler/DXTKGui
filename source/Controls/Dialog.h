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

class Dialog : GUIControlBox {
public:

	Dialog();
	~Dialog();

	void initialize(unique_ptr<FontSet> font,
		ComPtr<ID3D11ShaderResourceView> pixelTexture);

	void setDimensions(const Vector2& position, const Vector2& size,
		const int frameThickness = 2);

	void setTitle(wstring text);
	virtual void setText(wstring text) override;
	/** Not used in DialogBox */
	virtual XMVECTOR XM_CALLCONV measureString() const override;

	virtual void update(double deltaTime, MouseController* mouse);
	virtual void draw(SpriteBatch* batch);

	void setConfirmButton(unique_ptr<Button> okButton);

	virtual void addItem(unique_ptr<GUIControl> control) override;
	virtual void addItems(vector<unique_ptr<GUIControl>> controls) override;

	virtual void setFont(unique_ptr<FontSet> newFont) override;
	virtual void setTint(const Color& color) override;

	virtual const Vector2& getPosition() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;

	virtual bool clicked() override;
	virtual bool selected() override;
	virtual bool hovering() override;


	void open();
	void close();
	ClickAction getResult();

	bool isOpen = false;

private:

	enum GUIControlLookUp {
		TitleText, DialogText, ButtonOK, ButtonNeutral, ButtonCancel, Other
	};
		//unique_ptr<FontSet> font;
	vector<unique_ptr<GUIControl> > controls;

	ClickAction result = NONE;

	//unique_ptr<TextLabel> titleText;
	//unique_ptr<TextLabel> dialogText;

	Vector2 size;
	Vector2 titleFrameSize = Vector2(0, 40);
	Vector2 titleFramePosition;
	Vector2 dialogFrameSize;
	Vector2 dialogFramePosition;
	Vector2 buttonFramePosition;
	Vector2 buttonFrameSize;

	void calculateTitlePos();
	void calculateDialogTextPos();
	/*Vector2 titlePosition;
	Vector2 textPosition;
	Vector2 btn1Position;
	Vector2 btn2Position;
	Vector2 btn3Position;*/

	unique_ptr<RectangleSprite> bgSprite;
	unique_ptr<RectangleFrame> frame;
	unique_ptr<RectangleSprite> titleSprite;

	Vector2 standardButtonSize = Vector2(150, 35);
};