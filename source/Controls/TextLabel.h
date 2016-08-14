#pragma once

#include <tchar.h>
#include <sstream>
#include <strsafe.h>

#include "../BaseGraphics/FontSet.h"
#include "GUIControl.h"


using namespace std;

namespace Controls {
	class TextLabel : public GUIControl {
	public:
		TextLabel(Vector2 position, FontSet* font, wstring text);
		TextLabel(Vector2 position, FontSet* font);
		/** A Textlabel with no set position. */
		TextLabel(FontSet* font);
		~TextLabel();


		virtual void update(double deltaTime, MouseController* mouse) override;
		virtual void setPosition(Vector2& position) override;
		virtual const Vector2& getPosition() override;
		virtual int getWidth() override;
		virtual int getHeight() override;

		void draw(SpriteBatch* batch);
		void draw(SpriteBatch* batch, Color tint);

		void setText(string text);
		void setText(wostringstream& text);
		void setText(wstring text);

		const wchar_t* getText();

		

	private:

		wstring label;
		FontSet* font;

		Vector2 position;
		Vector2 size;
		

	};
};
