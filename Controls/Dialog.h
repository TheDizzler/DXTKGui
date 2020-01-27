#pragma once

#include "TexturePanel.h"
#include "../Effects/TransitionEffects.h"
#include "../Controllers/Selector.h"


class Dialog : public GUIControlBox, public Texturizable {
public:

	Dialog(GUIFactory* factory, MouseController* mouseController);
	virtual ~Dialog();
	virtual void draw(SpriteBatch* batch) = 0;


	virtual void show();
	virtual void hide();

	virtual bool isOpen();

	virtual void setText(wstring text) override;
	/** Returns non-formatted text. */
	virtual const wchar_t* getText() override;

	virtual void setFont(const pugi::char_t* font = "Default Font") override;
	virtual void setTextTint(const XMFLOAT4 color);

	void setOpenTransition(TransitionEffects::TransitionEffect* effect);
	void setCloseTransition(TransitionEffects::TransitionEffect* effect);


	virtual bool clicked() override;
	virtual bool pressed() override;
	virtual bool hovering() override;

	virtual const Vector2& getPosition() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;

	/** Could make this adaptable? */
	Vector2 dialogTextMargin = Vector2(10, 10);

	/** Not used in Dialog */
	virtual const Vector2 XM_CALLCONV measureString() const override;

	/* Trash? */
	/* Add other GUIControls to dialog. Control position should be relative to Dialog.
	Returns the position of control in control list. */
	virtual size_t addControl(unique_ptr<GUIControl> control) override;
	/* Trash? */
	/* Add other GUIControls to dialog. Not Implemented Yet. */
	virtual void addControls(vector<unique_ptr<GUIControl>> controls) override;
	virtual GUIControl* getControl(size_t controlPosition) const override;

	/** Required for some TransitionEffects. Bundle up all IElement2D to send for morphing time. */
	virtual const vector<IElement2D*> getElements() const;


	/** Not used in Dialog. */
	virtual void onClick() override {
	}
	/** Not used in Dialog. */
	virtual void onPress() override {
	}
	/** Not used in Dialog. */
	virtual void onHover() override {
	}
	/** Not used in Dialog. */
	virtual void resetState() override {
	}
protected:
	bool refreshTexture = true;
	unique_ptr<TexturePanel> texturePanel;

	bool isShowing = false;
	bool isOpening = false;
	bool isClosing = false;

	Vector2 size;

	unique_ptr<TextLabel> dialogText;

	TransitionEffects::TransitionEffect* openTransition = NULL;
	TransitionEffects::TransitionEffect* closeTransition = NULL;
	TransitionEffects::Run runTransition;
	TransitionEffects::Reset resetTransition;
	TransitionEffects::Draw drawTransition;


	/** Generic OnClick for Cancel Button. Closes the dialog. That's all. */
	class OnClickListenerCancelButton : public Button::ActionListener {
	public:
		OnClickListenerCancelButton(Dialog* dlg) : dialog(dlg) {
		}
		virtual void onClick(Button* button) override;
		virtual void onPress(Button* button) override;
		virtual void onHover(Button* button) override;
		virtual void resetState(Button* button) override;
	private:
		Dialog* dialog;
	};
};



/** A pop-up Dialog Box with button options (optional - up to 3: Ok, Neutral, Cancel).
	Title area and button area automatically adjust to size of their components.
	NOTE: This control is fairly expensive. Use conservatively. EDIT: It's a bit better now. */
class PromptDialog : public Dialog {
public:

	PromptDialog(GUIFactory* factory,
		MouseController* mouseController, HWND hwnd, bool movable, bool centerText);
	virtual ~PromptDialog();

	void initialize(const pugi::char_t* font = "Default Font");

	void setSelectorManager(Joystick* joy, KeyboardController* keys);

	virtual void forceRefresh() override;
	virtual void reloadGraphicsAsset() override;

	virtual void setScrollBar(ScrollBarDesc& scrollBarDesc);
	virtual void alwaysShowScrollBar(bool alwaysShow);

	virtual void setDimensions(const Vector2& position, const Vector2& size,
		const int frameThickness = 2);


	void setTitle(wstring text, const Vector2& scale = Vector2(1.5, 1.5),
		const pugi::char_t* font = "Default Font", Color color = Color(1, .5, 0, 1));
	virtual void setText(wstring text) override;

	/* Works in the y dimension. x dimension gets uppity. */
	void setTitleAreaDimensions(const Vector2& newSize);

	/** Adds pre-created button to dialog as confirm button.
		Note: user must initalize everything except for position.
		Set autoSize to false if using an ImageButton. */
	void setConfirmButton(unique_ptr<Button> okButton,
		bool autoPosition = true, bool autoSize = true);
	/** Creates a Button that does nothing until ActionListener is set. */
	void setConfirmButton(wstring text, const pugi::char_t* font = "Default Font");
	/** Sets the listener to the confirm button. Will create the confirm button if one
		has not already beeen created for it. */
	void setConfirmOnClickListener(Button::ActionListener* iOnClickListener);

	void setCancelButton(unique_ptr<Button> cancelButton,
		bool autoPosition = true, bool autoSize = true);
	/* Creates a cancel button to dialog that just closes the dialog. */
	void setCancelButton(wstring text, const pugi::char_t* font = "Default Font");
	/** Sets the listener to the cancel button. Will create the cancel button if one
	has not already beeen created for it. */
	void setCancelOnClickListener(Button::ActionListener* iOnClickListener);

	/* Dialog checks to see if it's open before performing any logic. */
	virtual bool update(double deltaTime);
	/* Dialog checks to see if it's open before performing any logic. */
	virtual void draw(SpriteBatch* batch);

	virtual unique_ptr<GraphicsAsset> texturize() override;
	virtual void textureDraw(SpriteBatch* batch, ComPtr<ID3D11Device> device = NULL) override;

	/* Add other GUIControls to dialog. Control position should be relative to Dialog.
		Returns the position of control in control list. */
	virtual size_t addControl(unique_ptr<GUIControl> control) override;
	/* Add other GUIControls to dialog. Not Implemented Yet. */
	virtual void addControls(vector<unique_ptr<GUIControl>> controls) override;
	virtual GUIControl* getControl(size_t controlPosition) const override;

	virtual void setFont(const pugi::char_t* font = "Default Font") override;
	void setTextTint(const XMFLOAT4 color) override;
	virtual void setTint(const XMFLOAT4 color) override;
	virtual void setScale(const Vector2& newScale) override;
	virtual void setPosition(const Vector2& newPosition) override;
	virtual void setLayerDepth(const float depth, bool frontToBack = true) override;

	const Color& getPanelTint() const;

	virtual const vector<IElement2D*> getElements() const override;

	int titleTextMargin = 10;
protected:

	enum GUIControlLookUp {
		TitleText, ButtonOK, ButtonNeutral, ButtonCancel, Other
	};

	HWND hwnd;
	vector<unique_ptr<GUIControl> > controls;
	unique_ptr<TexturePanel> panel;
	unique_ptr<TexturePanel> texturePanel;

	bool refreshTexture = true;

	Vector2 titleFrameSize = Vector2(0, 0);
	Vector2 titleFramePosition;
	Vector2 textFrameSize;
	Vector2 textFramePosition;
	Vector2 buttonFramePosition;
	Vector2 buttonFrameSize = Vector2(0, 0);

	int minTextFrameHeight;

	int frameThickness = 2;
	int buttonMargin = 10; // space between edge of dialog box and button edge
	Vector2 okButtonPosition;
	Vector2 cancelButtonPosition;
	Vector2 neutralButtonPosition;

	wstring reformatText(size_t* scrollBarBuffer);
	void testMinimumSize();

	void calculateTitlePos();
	void calculateDialogTextPos();
	bool calculateButtonPosition(Vector2& buttonPos);
	int getMaxButtonHeight();

	/** Used for dragging dialog around, if draggable set.
		Also used for transition effects. */
	void setDraggedPosition(Vector2& newPosition);
	/** NOT USED. */
	//void movePosition(const Vector2& moveVector);

	unique_ptr<RectangleSprite> bgSprite;
	unique_ptr<RectangleFrame> frame;
	unique_ptr<RectangleSprite> titleSprite;
	unique_ptr<RectangleSprite> buttonFrameSprite;

	Vector2 standardButtonSize = Vector2(150, 35);

	Vector2 pressedPosition;
	bool movable = false;
	bool centerText = false;

	unique_ptr<SelectorManager> selector = NULL;
};