#pragma once

#include <tchar.h>
#include <sstream>
#include <strsafe.h>

#include "../BaseGraphics/FontSet.h"
#include "GUIControl.h"


using namespace std;

class TextLabel : public GUIControl {
public:
	TextLabel(Vector2 position, wstring text, shared_ptr<FontSet> font);
	TextLabel(Vector2 position, shared_ptr<FontSet> font);
	/** A Textlabel with no set position. Used for ListBoxes, etc. */
	TextLabel(shared_ptr<FontSet> font);
	~TextLabel();


	virtual void update(double deltaTime, MouseController* mouse) override;
	//virtual void setPosition(Vector2& position) override;
	virtual const Vector2& getPosition() const override;
	//virtual void setScale(const Vector2 & scale) override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;

	void draw(SpriteBatch* batch);
	void draw(SpriteBatch* batch, Color tint);

	virtual void setFont(unique_ptr<FontSet> newFont) override;
	virtual void setFont(shared_ptr<FontSet> newFont);
	virtual void setTint(const Color& color) override;
	/** Warning: this will affect all text sharing this font! */
	virtual void setScale(const Vector2& scl) override;

	void setText(string text);
	void setText(wostringstream& text);
	virtual void setText(wstring text) override;

	virtual XMVECTOR XM_CALLCONV measureString() const override;

	const wchar_t* getText();

	virtual bool clicked() override;
	virtual bool selected() override;
	virtual bool hovering() override;

private:

	wstring label;
	shared_ptr<FontSet> font;



	/* Sometimes TextLabel is just a TextLabel. */
	bool isHoverable = false;

	// to do: tooltips

};
