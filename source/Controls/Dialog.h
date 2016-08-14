#pragma once

#include <vector>


#include "../BaseGraphics/Sprite.h"
#include "TextButton.h"


class Dialog : public Sprite {
public:

	enum DialogResult {
		NONE, CONFIRM, CANCEL
	};

	Dialog(const Vector2& position);
	~Dialog();

	bool initialize(ID3D11Device* device, const wchar_t* fontFile);

	virtual void update(double deltaTime, MouseController* mouse);
	virtual void draw(SpriteBatch* batch);

	void open();
	void close();
	DialogResult getResult();

	bool isOpen = false;

private:

	unique_ptr<FontSet> font;
	vector<TextLabel*> labels;
	vector<TextButton*> buttons;

	
	DialogResult result = NONE;

};