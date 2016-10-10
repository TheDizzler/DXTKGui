#pragma once

#include "TextLabel.h"


class CheckBox : public GUIControl {
public:

	CheckBox(unique_ptr<Sprite> uncheckedSprite,
		unique_ptr<Sprite> checkedSprite, unique_ptr<FontSet> font);
	~CheckBox();

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

	class OnClickListener {
	public:
	/** checkbox: The CheckBox this OnClickListener is attached to.
		isChecked: whether box is checked or no*/
		virtual void onClick(CheckBox* checkbox, bool isChecked) = 0;
	};

	typedef void (OnClickListener::*OnClickFunction) (CheckBox*, bool);

	void setOnClickListener(OnClickListener* iOnC) {
		if (onClickListener != NULL)
			delete onClickListener;
		onClickFunction = &OnClickListener::onClick;
		onClickListener = iOnC;
	}

	void onClick() {
		if (onClickListener != NULL) 
			(onClickListener->*onClickFunction)(this, isClicked);

		if (isClicked)
			texture = checkedSprite->getTexture().Get();
		else
			texture = uncheckedSprite->getTexture().Get();
	}

private:

	OnClickFunction onClickFunction;
	OnClickListener* onClickListener = NULL;
	
	ID3D11ShaderResourceView* texture;
	/** Helper function to center text in check sprite. */
	void centerText();

	Vector2 labelOffset = Vector2(10, 0);
	unique_ptr<TextLabel> label;

	unique_ptr<Sprite> uncheckedSprite;
	unique_ptr<Sprite> checkedSprite;
	/* The sprite used in draw(). */
	//Sprite* drawSprite;
};