#include "ComboBox.h"
#include "../GUIFactory.h"

ComboBox::ComboBox(GUIFactory* factory, MouseController* mouseController,
	const Vector2& pos, const int len, size_t itemHeight, const int maxItemsShown)
	: GUIControl(factory, mouseController) {

	position = pos;
	width = len;

	maxDisplayItems = maxItemsShown;

}

ComboBox::~ComboBox() {

	if (actionListener)
		delete actionListener;

}

bool ComboBox::initialize(const pugi::char_t* fontName,
	ListBox* lstBx, const pugi::char_t* buttonName,
	bool enumerateList, const int frameThickness) {


	comboListButton.reset(
		(ImageButton*) guiFactory->createImageButton(buttonName));

	comboListButton->setPosition(
		Vector2(position.x + width - comboListButton->getWidth(), position.y));
	comboListButton->setActionListener(new ShowListBoxListener(this));

	frame.reset(guiFactory->createRectangleFrame(
		position, Vector2((float) width, (float) comboListButton->getScaledHeight()),
		frameThickness));

	listBox.reset(lstBx);
	listBox->setActionListener(new ListBoxListener(this));

	selectedLabel.reset(guiFactory->createTextLabel(
		Vector2(position.x + textMarginX, position.y + textMarginY), L"", fontName));
	//selectedLabel->setActionListener(new SelectedOnClick(this));

	selectedBackgroundSprite.reset(guiFactory->createRectangle(position,
		Vector2((float) width, (float) comboListButton->getScaledHeight()), Color(.5f, .5f, .5f, 1.0f)));

	height = frame->getHeight();

	texturePanel.reset(guiFactory->createPanel());

	return true;
}

void ComboBox::reloadGraphicsAsset() {

	comboListButton->reloadGraphicsAsset();
	frame.reset(guiFactory->createRectangleFrame(
		position, Vector2((float) width, (float) comboListButton->getScaledHeight()),
		frameThickness, frame->getTint()));
	listBox->reloadGraphicsAsset();
	selectedLabel->reloadGraphicsAsset();
	selectedBackgroundSprite->reloadGraphicsAsset(guiFactory);
	texturePanel.reset(guiFactory->createPanel());

	refreshTexture = true;
}

void ComboBox::forceRefresh() {
	refreshTexture = true;
	selectedLabel->forceRefresh();
	comboListButton->forceRefresh();
	frame->forceRefresh();
}

void ComboBox::setScrollBar(ScrollBarDesc& scrollBarDesc) {
	listBox->setScrollBar(scrollBarDesc);
}

bool ComboBox::update(double deltaTime) {

	if (frame->contains(mouse->getPosition()) && mouse->clicked()) {
		comboListButton->onClick();
		refreshTexture = true;
	}

	bool refresh = false;
	if (isOpen) {
		if (listBox->update(deltaTime))
			refresh = true;
	}

	if (selectedLabel->update(deltaTime))
		refreshTexture = true;
	if (comboListButton->update(deltaTime))
		refreshTexture = true;
	if (frame->update())
		refreshTexture = true;

	if (refreshTexture) {
		texturePanel->setTexture(texturize());
		refreshTexture = false;
		return true;
	}
	return refresh;
}

void ComboBox::draw(SpriteBatch* batch) {

	if (isOpen) {
		listBox->draw(batch);
	}

	texturePanel->draw(batch);
}


unique_ptr<GraphicsAsset> ComboBox::texturize() {
	return move(guiFactory->createTextureFromTexturizable(this));
}

void ComboBox::textureDraw(SpriteBatch * batch, ComPtr<ID3D11Device> device) {

	selectedBackgroundSprite->draw(batch);
	selectedLabel->draw(batch);
	comboListButton->draw(batch);
	frame->draw(batch);
}

void ComboBox::setPosition(const Vector2& pos) {
	Vector2 moveDiff = pos - position;
	position = pos;
	selectedBackgroundSprite->moveBy(moveDiff);
	selectedLabel->moveBy(moveDiff);
	comboListButton->moveBy(moveDiff);
	frame->moveBy(moveDiff);
	listBox->moveBy(moveDiff);
	texturePanel->setPosition(pos);
}

void ComboBox::moveBy(const Vector2& moveVector) {
	position += moveVector;
	selectedBackgroundSprite->moveBy(moveVector);
	selectedLabel->moveBy(moveVector);
	comboListButton->moveBy(moveVector);
	frame->moveBy(moveVector);
	listBox->moveBy(moveVector);
	texturePanel->setPosition(position);
}


void ComboBox::show() {
	isOpen = !isOpen;
}

void ComboBox::hide() {
	isOpen = false;
}

void ComboBox::resizeBox() {

	comboListButton->setPosition(
		Vector2(FLOAT(position.x + width - comboListButton->getWidth()), position.y));
	frame->setDimensions(position, Vector2((float) width, (float) comboListButton->getHeight()));
	selectedBackgroundSprite->setDimensions(position,
		Vector2((float) width, (float) comboListButton->getHeight()));
}

void ComboBox::alwaysShowScrollBar(bool alwaysShow) {
	listBox->alwaysShowScrollBar(alwaysShow);
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

const Vector2 XM_CALLCONV ComboBox::measureString() const {
	return Vector2::Zero;
}

const Vector2& ComboBox::getPosition() const {
	return position;
}

const int ComboBox::getWidth() const {
	return width;
}

const int ComboBox::getHeight() const {
	return height;
}

void ComboBox::setLayerDepth(const float depth, bool frontToBack) {

	float nudge = .00000001f;
	if (!frontToBack)
		nudge *= -1;

	listBox->setLayerDepth(depth, frontToBack);
	selectedBackgroundSprite->setLayerDepth(depth, frontToBack);
	selectedLabel->setLayerDepth(depth + nudge, frontToBack);
	comboListButton->setLayerDepth(depth + nudge * 2, frontToBack);
	frame->setLayerDepth(depth + nudge * 3, frontToBack);

}

bool ComboBox::clicked() {
	return isClicked;
}

bool ComboBox::pressed() {
	return isPressed;
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

void ComboBox::ShowListBoxListener::onClick(Button* button) {
	comboBox->show();
}

void ComboBox::ShowListBoxListener::onPress(Button* button) {
}

void ComboBox::ShowListBoxListener::onHover(Button* button) {
}

void ComboBox::ShowListBoxListener::resetState(Button * button) {
}

void ComboBox::ListBoxListener::onClick(ListBox* listbox, size_t selectedItemIndex) {
	comboBox->onClick();
	comboBox->show();
}

void ComboBox::ListBoxListener::onHover(ListBox* listbox, int hoveredItemIndex) {
	comboBox->onHover();
}

