#include "ComboBox.h"


ComboBox::ComboBox(const Vector2& pos, const int len,
	size_t itemHeight, const int maxItemsShown) {

	position = pos;
	width = len;

	maxDisplayItems = maxItemsShown;

}


ComboBox::~ComboBox() {
}

#include "../Controls/GUIFactory.h"
bool ComboBox::initialize(shared_ptr<FontSet> fnt,
	GraphicsAsset* pixelAsset, ListBox* lstBx, bool enumerateList) {


	frame.reset(new RectangleFrame(pixelAsset));

	comboListButton.reset((ImageButton*) guiFactory->createImageButton("Combo Button Closed"));

	comboListButton->setPosition(
		Vector2(position.x + width - comboListButton->getWidth(), position.y));
	comboListButton->setOnClickListener(new ShowListBoxListener(this));
	frame->setDimensions(position, Vector2(width, comboListButton->getHeight()));

	listBox.reset(lstBx);
	listBox->setOnClickListener(new ListBoxListener(this));

	selectedLabel.reset(guiFactory->createTextLabel(
		Vector2(position.x + textMarginX, position.y + textMarginY)));

	return true;
}

void ComboBox::setScrollBar(ScrollBarDesc& scrollBarDesc) {
	listBox->setScrollBar(scrollBarDesc);
}

void ComboBox::update(double deltaTime, MouseController* mouse) {

	selectedLabel->update(deltaTime, mouse);
	comboListButton->update(deltaTime, mouse);


	if (isOpen) {
		listBox->update(deltaTime, mouse);
	}

}

void ComboBox::draw(SpriteBatch* batch) {

	if (isOpen) {
		listBox->draw(batch);
	}

	selectedLabel->draw(batch);
	comboListButton->draw(batch);
	frame->draw(batch);
}


void ComboBox::open() {

	isOpen = !isOpen;
	//float rot = XM_PI*isOpen;
	//comboListButton->setRotation(rot);

	//Vector2 buttonPos = comboListButton->getPosition();

	//if (rot == 0)
	//	comboListButton->setPosition(
	//		Vector2(buttonPos.x - comboListButton->getWidth(),
	//			buttonPos.y - comboListButton->getHeight()));
	//else if (rot == XM_PI*isOpen)
	//	comboListButton->setPosition(
	//		Vector2(buttonPos.x + comboListButton->getWidth(),
	//			buttonPos.y + comboListButton->getHeight()));
}

void ComboBox::close() {
	isOpen = false;
	comboListButton->setRotation(0);
}

void ComboBox::resizeBox() {

	comboListButton->setPosition(
		Vector2(position.x + width - comboListButton->getWidth(), position.y));
	comboListButton->setOnClickListener(new ShowListBoxListener(this));
	frame->setDimensions(position, Vector2(width, comboListButton->getHeight()));
}

void ComboBox::setFont(const pugi::char_t * font) {
}

void ComboBox::setSelected(size_t newIndex) {
	listBox->setSelected(newIndex);
	selectedLabel->setText(listBox->getSelected()->toString());
}

ListItem* ComboBox::getSelected() {
	return listBox->getSelected();
}

ListItem* ComboBox::getItem(size_t index) {
	return listBox->getItem(index);
}

void ComboBox::setText(wstring text) {
}

XMVECTOR XM_CALLCONV ComboBox::measureString() const {
	return Vector2();
}

const Vector2 & ComboBox::getPosition() const {
	return position;
}

const int ComboBox::getWidth() const {
	return width;
}

const int ComboBox::getHeight() const {
	return hitArea->size.y;
}

bool ComboBox::clicked() {
	return isClicked;
}

bool ComboBox::selected() {
	return isSelected;
}

bool ComboBox::hovering() {
	return isHover;
}

void ComboBox::addItem(ListItem* item) {
	listBox->addItem(item);
	// may have to do some resizing here
	width = listBox->getWidth();
	resizeBox();
}

void ComboBox::addItems(vector<ListItem*> items) {
	listBox->addItems(items);
	width = listBox->getWidth();
	resizeBox();
}

void ComboBox::clear() {
	listBox->clear();
	resizeBox();
}

void ComboBox::ShowListBoxListener::onClick(Button * button) {
	comboBox->open();
}

void ComboBox::ListBoxListener::onClick(ListBox* listbox, int selectedItemIndex) {
	comboBox->triggerOnClick();
}
