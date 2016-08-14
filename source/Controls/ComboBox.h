#pragma once

#include "ListBox.h"


class ComboBox {
public:
	ComboBox(const Vector2& position, const int width);
	~ComboBox();

	virtual bool initialize(ID3D11Device* device, const wchar_t* fontFile);
	virtual void update(double deltaTime, MouseController* mouse);
	virtual void draw(SpriteBatch* batch);


	//void addItems(vector<wstring> items);

	void open();
	void close();
	bool isOpen = false;

private:
	unique_ptr<ListBox> listBox;

	unique_ptr<ImageButton> buttonClosed;
	unique_ptr<ImageButton> buttonOpen;
	ImageButton* button;

	Vector2 position;

	Vector2 selectedDisplayPosition;

	// width of combobox
	int width;

	size_t textMarginX = 10;
	size_t textMarginY = 10;

};