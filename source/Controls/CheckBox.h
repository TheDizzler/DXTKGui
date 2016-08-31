#pragma once

#include "TextLabel.h"


class CheckBox : public GUIControl {
public:
	CheckBox(unique_ptr<Sprite> uncheckedSprite,
		unique_ptr<Sprite> checkedSprite, unique_ptr<FontSet> font);
	~CheckBox();


	virtual void draw(SpriteBatch* batch) override;
	virtual void update(double deltaTime, MouseController* mouse) override;

	/* Should probably changed the way this works. */
	virtual void setFont(const pugi::char_t* font = "Default Font") override;
	virtual void setText(wstring text) override;
	virtual void setPosition(const Vector2& position) override;
	virtual void setScale(const Vector2& scale) override;

	virtual XMVECTOR XM_CALLCONV measureString() const override;
	virtual const Vector2& getPosition() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;

	virtual bool clicked() override;
	virtual bool selected() override;
	virtual bool hovering() override;

	/** Colors for text on button. */
	Color normalColorText = Color(Vector3(0, 0, 0));
	Color hoverColorText = Color(Vector3(.5, .75, 1));
	//Color selectedColorText = Color(Vector3(0, .5, 1));
private:

	virtual void setToUncheckedState();
	virtual void setToHoverState();
	virtual void setToCheckedState();

	/** Helper function to center text in check sprite. */
	void centerText();

	Vector2 labelOffset = Vector2(10, 0);
	unique_ptr<TextLabel> label;

	unique_ptr<Sprite> uncheckedSprite;
	unique_ptr<Sprite> checkedSprite;
	/* The sprite used in draw(). */
	Sprite* drawSprite;
};