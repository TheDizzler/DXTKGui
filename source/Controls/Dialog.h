#pragma once

#include <vector>


#include "../BaseGraphics/Sprite.h"
#include "Button.h"

namespace Controls {


	/** Creates a Dialog Box using a custom dds background file. */
	class CustomBGDialog : public Dialog {
	public:
		CustomBGDialog(const Vector2& position);

		bool initialize(ID3D11Device* device, const wchar_t* fontFile,
			const wchar_t* bgFile);

	private:

		unique_ptr<Sprite> bgSprite;


	};

	class Dialog {
	public:

		enum DialogResult {
			NONE, CONFIRM, CANCEL
		};

		Dialog(const Vector2& position);
		~Dialog();

		bool initialize(ID3D11Device* device, const wchar_t* fontFile);

		virtual void update(double deltaTime, MouseController* mouse);
		virtual void draw(SpriteBatch* batch);

		void add(GUIControl control);

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
};