#pragma once

#include "ListBox.h"


class ComboBox : public GUIControl, public Texturizable {
public:
	ComboBox(GUIFactory* factory, MouseController* mouseController,
		const Vector2& position, const int width,
		size_t itemHeight = 32, const int maxItemsShown = 7);
	virtual ~ComboBox();

	bool initialize(const pugi::char_t* fontName, ListBox* listbox,
		const pugi::char_t* buttonAsset = "Combo Button Closed",
		bool enumerateList = false, const int frameThickness = 2);

	virtual void reloadGraphicsAsset() override;
	virtual void forceRefresh() override;

	void setScrollBar(ScrollBarDesc& scrollBarDesc);
	void alwaysShowScrollBar(bool alwaysShow);

	virtual bool update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;

	virtual unique_ptr<GraphicsAsset> texturize() override;
	virtual void textureDraw(SpriteBatch* batch, ComPtr<ID3D11Device> device = NULL) override;
	virtual void setPosition(const Vector2& position) override;
	virtual void moveBy(const Vector2& moveVector) override;
	virtual void setFont(const pugi::char_t * font = "Default Font") override;

	void setSelected(size_t newIndex);
	ListItem* getSelected();
	ListItem* getItem(size_t index);


	/* Not used in ComboBox. */
	virtual void setText(wstring text) override;
	/* Not used in ComboBox. */
	virtual const Vector2 XM_CALLCONV measureString() const override;

	virtual const Vector2& getPosition() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;

	virtual void setLayerDepth(const float depth, bool frontToBack = true) override;

	virtual bool clicked() override;
	virtual bool pressed() override;
	virtual bool hovering() override;

	void addItem(ListItem* item);
	void addItems(vector<ListItem*> items);
	void clear();

	/** Toggles between open and closed. */
	void show();
	/** Closes if open. */
	void hide();
	bool isOpen = false;

	class ActionListener {
	public:
		/** combobox: The ComboBox this ActionListener is attached to.
		selectedItemIndex: index of item in ListBox.*/
		virtual void onClick(ComboBox* combobox, size_t selectedItemIndex) = 0;
		virtual void onHover(ComboBox* listbox, int hoveredItemIndex) = 0;
	};

	typedef void (ActionListener::*OnClickFunction) (ComboBox*, size_t);
	typedef void (ActionListener::*OnHoverFunction) (ComboBox*, int);

	void setActionListener(ActionListener* iOnC) {
		if (actionListener != NULL)
			delete actionListener;
		onClickFunction = &ActionListener::onClick;
		onHoverFunction = &ActionListener::onHover;
		actionListener = iOnC;
	}

	/** Called when ListBox item selected (unless overriden) */
	virtual void onClick() override {
		if (actionListener != NULL)
			(actionListener->*onClickFunction)(this, listBox->getSelectedIndex());
		selectedLabel->setText(listBox->getSelected()->toString());
	}

	/** Not used in Combobox. */
	virtual void onPress() override {
	}

	/** Note: To activate the onHover, it should be called from ListBox onHover. */
	virtual void onHover() override {
		if (actionListener != NULL)
			(actionListener->*onHoverFunction)(this, listBox->getHoveredIndex());
	}

	virtual void resetState() override {
		
	}

private:
	bool refreshTexture = true;
	unique_ptr<TexturePanel> texturePanel;

	unique_ptr<ListBox> listBox;

	unique_ptr<RectangleFrame> frame;
	unique_ptr<ImageButton> comboListButton;

	unique_ptr<TextLabel> selectedLabel;
	unique_ptr<RectangleSprite> selectedBackgroundSprite;

	/* width of combobox */
	int width;
	int height;

	size_t textMarginX = 10;
	size_t textMarginY = 5;

	size_t maxDisplayItems = 7;

	int frameThickness = 2;

	void resizeBox();

	ActionListener* actionListener = NULL;
	OnClickFunction onClickFunction;
	OnHoverFunction onHoverFunction;
	

	class ListBoxListener : public ListBox::ActionListener {
	public:
		ListBoxListener(ComboBox* combo) : comboBox(combo) {
		}

	private:
		ComboBox* comboBox;
		virtual void onClick(ListBox* listbox, size_t selectedItemIndex) override;
		virtual void onHover(ListBox* listbox, int hoveredItemIndex) override;
	};

	class ShowListBoxListener : public Button::ActionListener {
	public:
		ShowListBoxListener(ComboBox* combo) : comboBox(combo) {
		};
		virtual void onClick(Button* button) override;
		virtual void onPress(Button* button) override;
		virtual void onHover(Button* button) override;
		virtual void resetState(Button* button) override;
	private:
		ComboBox* comboBox;
	};

	class SelectedOnClick : public TextLabel::ActionListener {
	public:
		SelectedOnClick(ComboBox* combobox) : comboBox(combobox) {
		}

		virtual void onClick(TextLabel* button) override {
			comboBox->show();
		}
		virtual void onPress(TextLabel* button) override{}
		virtual void onHover(TextLabel* button) override{}
	private:
		ComboBox* comboBox;
	};

};


