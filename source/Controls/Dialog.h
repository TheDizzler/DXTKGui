#pragma once

#include "Button.h"
#include "../BaseGraphics/TransitionEffects.h"



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
class Dialog : public GUIControlBox {
public:

	Dialog(bool movable);
	~Dialog();

	void initialize(GraphicsAsset* pixelAsset,
		const pugi::char_t* font = "Default Font");

	void setDimensions(const Vector2& position, const Vector2& size,
		const int frameThickness = 2);

	void setTransition(TransitionEffects::TransitionEffect* effect);

	/** End position is set position in dimensions. */
	void setSlideInTransition(Vector2 startPos, float transitionSpeed);
	//void setGrowTransition(Vector2 startScale, float transitionSpeed);

	void setTitle(wstring text, const Vector2& scale = Vector2(1.5, 1.5),
		const pugi::char_t* font = "Default Font");
	virtual void setText(wstring text) override;

	/* Works in the y dimension. x dimension gets uppity. */
	void setTitleAreaDimensions(const Vector2& newSize);

	/** Adds pre-created button to dialog as confirm button.
		Note: user must initalize everything except for position. */
	void setConfirmButton(unique_ptr<Button> okButton,
		bool autoPosition = true, bool autoSize = true);
	/** Creates a Button that does nothing until OnClickListener is set. */
	void setConfirmButton(wstring text, const pugi::char_t* font = "Default Font");
	/** Sets the listener to the confirm button. Will create the confirm button if one
		has not already beeen created for it. */
	void setConfirmOnClickListener(Button::OnClickListener* iOnClickListener);

	void setCancelButton(unique_ptr<Button> cancelButton);
	/* Creates a cancel button to dialog that just closes the dialog. */
	void setCancelButton(wstring text, const pugi::char_t* font = "Default Font");
	/** Sets the listener to the cancel button. Will create the cancel button if one
	has not already beeen created for it. */
	void setCancelOnClickListener(Button::OnClickListener* iOnClickListener);

	virtual void update(double deltaTime, MouseController* mouse);
	virtual void draw(SpriteBatch* batch);

	/* Add other GUIControls to dialog. Not Implemented Yet. */
	virtual void addItem(unique_ptr<GUIControl> control) override;
	/* Add other GUIControls to dialog. Not Implemented Yet. */
	virtual void addItems(vector<unique_ptr<GUIControl>> controls) override;

	virtual void setFont(const pugi::char_t* font = "Default Font") override;
	virtual void setTint(const Color& color) override;
	virtual void setScale(const Vector2& newScale) override;
	virtual void setPosition(const Vector2& newPosition) override;
	
	virtual const Vector2& getPosition() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;

	virtual bool clicked() override;
	virtual bool pressed() override;
	virtual bool hovering() override;

	/** Not used in DialogBox */
	virtual XMVECTOR XM_CALLCONV measureString() const override;

	void open();
	void close();
	/* Deprecating. Use ActionListeners instead. */
	//ClickAction getResult();

	bool isOpen = false;
	bool isTransitioning = false;
private:

	enum GUIControlLookUp {
		TitleText, DialogText, ButtonOK, ButtonNeutral, ButtonCancel, Other
	};

	vector<unique_ptr<GUIControl> > controls;

	/* Deprecating. Use ActionListeners instead. */
	//ClickAction result = NONE;

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

	/** Used for dragging dialog around, if draggable set.
		Also used for transition effects. */
	void setDraggedPosition(Vector2& newPosition);
	/** NOT USED. */
	void movePosition(const Vector2& moveVector);

	unique_ptr<RectangleSprite> bgSprite;
	unique_ptr<RectangleFrame> frame;
	unique_ptr<RectangleSprite> titleSprite;
	unique_ptr<RectangleSprite> buttonFrameSprite;

	Vector2 standardButtonSize = Vector2(150, 35);

	Vector2 pressedPosition;
	bool movable = false;

	TransitionEffects::TransitionEffect* transition = NULL;
	TransitionEffects::Run runTransition;
	TransitionEffects::Reset resetTransition;

	/** Generic OnClick for Cancel Button. */
	class OnClickListenerCancelButton : public Button::OnClickListener {
	public:
		OnClickListenerCancelButton(Dialog* dlg) : dialog(dlg) {
		}
		virtual void onClick(Button * button) override;
	private:
		Dialog* dialog;
	};
};



