#pragma once

#include "TextLabel.h"


class CheckBox : public GUIControl {
public:

	CheckBox(GUIFactory* factory, shared_ptr<MouseController> mouseController,
		unique_ptr<Sprite> uncheckedSprite, unique_ptr<Sprite> checkedSprite,
		const pugi::char_t* font);
	virtual ~CheckBox();

	virtual void update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;

	/* Should probably changed the way this works. */
	virtual void setFont(const pugi::char_t* font = "Default Font") override;
	virtual void setText(wstring text) override;
	virtual void setPosition(const Vector2& position) override;
	virtual void setScale(const Vector2& scale) override;

	virtual const Vector2& XM_CALLCONV measureString() const override;
	virtual const Vector2& getPosition() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;

	virtual bool clicked() override;
	virtual bool pressed() override;
	virtual bool hovering() override;

	Color normalColor = Color(Vector3(0, 0, 0));
	Color hoverColor = Color(Vector4(.5, .75, 1, 1));

	/** Colors for text beside checkbox. */
	Color normalColorText = Color(Vector3(0, 0, 0));
	Color hoverColorText = Color(Vector3(.5, .75, 1));


	void setChecked(bool checked);

	class ActionListener {
	public:
	/** checkbox: The CheckBox this ActionListener is attached to.
		isChecked: whether box is checked or no*/
		virtual void onClick(CheckBox* checkbox, bool isChecked) = 0;
	};

	

	void setActionListener(ActionListener* iOnC) {
		if (actionListener != NULL)
			delete actionListener;
		onClickFunction = &ActionListener::onClick;
		actionListener = iOnC;
	}

	void onClick() {
		if (actionListener != NULL)
			(actionListener->*onClickFunction)(this, isClicked);

		if (isClicked)
			texture = checkedSprite->getTexture().Get();
		else
			texture = uncheckedSprite->getTexture().Get();
	}

private:
	typedef void (ActionListener::*OnClickFunction) (CheckBox*, bool);
	ActionListener* actionListener = NULL;
	OnClickFunction onClickFunction;
	

	ID3D11ShaderResourceView* texture;
	/** Helper function to center text in check sprite. */
	void centerText();

	Vector2 labelOffset = Vector2(10, 0);
	unique_ptr<TextLabel> label;

	unique_ptr<Sprite> uncheckedSprite;
	unique_ptr<Sprite> checkedSprite;
	
	bool firstHover;
};