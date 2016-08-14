#pragma once


#include "TextLabel.h"
//#include "../../globals.h"

#include "Button.h"



class TextButton : public Button {
public:
	TextButton();
	~TextButton();

	bool load(ID3D11Device* device, const wchar_t* fontFile,
		const wchar_t* upButtonFile, const wchar_t* downButtonFile);
	

	virtual void setPosition(Vector2& position);

	void setText(string text);

	virtual void draw(SpriteBatch* batch);



	
private:

	unique_ptr<FontSet> buttonFont;
	unique_ptr<TextLabel> buttonLabel;
};