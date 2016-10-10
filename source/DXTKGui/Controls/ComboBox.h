#pragma once

#include "ListBox.h"


class ComboBox : public GUIControl {
public:
	ComboBox(const Vector2& position, const int width,
		size_t itemHeight = 32, const int maxItemsShown = 7);
	~ComboBox();

	bool initialize(shared_ptr<FontSet> font, GraphicsAsset* pixelAsset,
		ListBox* listbox, const pugi::char_t* buttonAsset = "Combo Button Closed",
		bool enumerateList = false);

	void setScrollBar(ScrollBarDesc& scrollBarDesc);
	void alwaysShowScrollBar(bool alwaysShow);

	virtual void update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;

	virtual void setFont(const pugi::char_t * font = "Default Font") override;

	void setSelected(size_t newIndex);
	ListItem* getSelected();
	ListItem* getItem(size_t index);


	/* Not used in ComboBox. */
	virtual void setText(wstring text) override;
	/* Not used in ComboBox. */
	virtual const Vector2& XM_CALLCONV measureString() const override;

	virtual const Vector2& getPosition() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;

	virtual bool clicked() override;
	virtual bool pressed() override;
	virtual bool hovering() override;

	void addItem(ListItem* item);
	void addItems(vector<ListItem*> items);
	void clear();


	void open();
	void close();
	bool isOpen = false;

	class OnClickListener {
	public:
		/** combobox: The ComboBox this OnClickListener is attached to.
		selectedItemIndex: index of item in ListBox.*/
		virtual void onClick(ComboBox* combobox, int selectedItemIndex) = 0;
	};

	typedef void (OnClickListener::*OnClickFunction) (ComboBox*, int);


	void setOnClickListener(OnClickListener* iOnC) {
		if (onClickListener != NULL)
			delete onClickListener;
		onClickFunction = &OnClickListener::onClick;
		onClickListener = iOnC;
	}

	void onClick() {
		if (onClickListener != NULL)
			(onClickListener->*onClickFunction)(this, listBox->getSelectedIndex());
		selectedLabel->setText(listBox->getSelected()->toString());
	}


private:
	unique_ptr<ListBox> listBox;

	unique_ptr<RectangleFrame> frame;
	unique_ptr<ImageButton> comboListButton;

	unique_ptr<TextLabel> selectedLabel;
	//Vector2 selectedDisplayPosition;

	/* width of combobox */
	int width;

	size_t textMarginX = 10;
	size_t textMarginY = 5;

	size_t maxDisplayItems = 7;

	void resizeBox();

	OnClickFunction onClickFunction;
	OnClickListener* onClickListener = NULL;

	class ListBoxListener : public ListBox::OnClickListener {
	public:
		ListBoxListener(ComboBox* combo) : comboBox(combo) {
		}

	private:
		ComboBox* comboBox;
		virtual void onClick(ListBox * listbox, int selectedItemIndex) override;
	};

	class ShowListBoxListener : public Button::OnClickListener {
	public:
		ShowListBoxListener(ComboBox* combo) : comboBox(combo) {
		};
		virtual void onClick(Button* button) override;
	private:
		ComboBox* comboBox;
	};

	class SelectedOnClick : public TextLabel::OnClickListener {
	public:
		SelectedOnClick(ComboBox* combobox) : comboBox(combobox) {
		}

		virtual void onClick(TextLabel* button) override {
			comboBox->open();
		}
	private:
		ComboBox* comboBox;
	};
};


