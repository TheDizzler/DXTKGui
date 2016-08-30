#include "ComboBox.h"


ComboBox::ComboBox(const Vector2& pos, const int len) {

	position = pos;
	width = len;
	selectedDisplayPosition = Vector2(position.x + textMarginX, position.y + textMarginY);
	listBox.reset(new ListBox(
		Vector2(position.x, position.y + textMarginY), width));
}


ComboBox::~ComboBox() {
}

//#include "../assets.h"
bool ComboBox::initialize(ComPtr<ID3D11Device> device, const wchar_t* fontFile) {

	//listBox->initialize(device, fontFile);

	   /*buttonSprite.reset(new Sprite());
	   if (!buttonSprite->load(device, Assets::comboButton)) {
	   MessageBox(0, L"Le Error", L"Error loading Combo button dds file!", MB_OK);
	   return false;
	   }
	   buttonSprite->setRotation(XM_PI);
	   buttonSprite->setPosition(
	   Vector2(position.x + width - buttonSprite->getWidth(), position.y));*/

	//buttonClosed.reset(new ImageButton());
	//if (!buttonClosed->load(device,
	//	Assets::comboButtonClosedFile, Assets::comboButtonPressedClosedFile))
	//	return false;
	//button->action = ButtonAction::CANCEL_BUTTON;
	//button->setText("Keep Playing!");
	/*buttonClosed->setPosition(
		Vector2(position.x + width - buttonClosed->getWidth(), position.y));*/


	/*buttonOpen.reset(new ImageButton());
	if (!buttonOpen->load(device,
		Assets::comboButtonOpenFile, Assets::comboButtonPressedOpenFile))
		return false;*/
	//button->action = ButtonAction::CANCEL_BUTTON;
	//button->setText("Keep Playing!");
	//buttonOpen->setPosition(
	//	Vector2(position.x + width - buttonOpen->getWidth(), position.y));

	//button = buttonClosed.get();

	return true;
}

void ComboBox::update(double deltaTime, MouseController* mouse) {

	//button->update(deltaTime, mouse);
	//if (button->clicked()) {
	//	if (isOpen)
	//		button = buttonClosed.get();
	//	else
	//		button = buttonOpen.get();
	//	isOpen != isOpen;

	//}

	//if (isOpen) {
	//	// display and check for hover
	//	listBox->update(deltaTime, mouse);
	//}

}

void ComboBox::draw(SpriteBatch* batch) {

	//if (isOpen) {
	//	listBox->draw(batch);
	//}

	//// draw the basic box, selected item and button
	//listBox->drawSelected(batch, selectedDisplayPosition);
	//button->draw(batch);
}

//void ComboBox::addItems(vector<wstring> items) {
//
//	listBox->addItems(items);
//}

void ComboBox::open() {

	/*isOpen = true;
	button = buttonOpen.get();*/
}

void ComboBox::close() {

	/*isOpen = true;
	button = buttonClosed.get();*/
}