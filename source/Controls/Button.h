#pragma once

#include "../Engine/MouseController.h"
#include "GUIControl.h"
#include "TextLabel.h"

using namespace std;


namespace Controls {
/** A visual and logical representation of a button.
Provides no actions on click; that must be handled else where.*/
	class Button : public GUIControl {
	public:

		Button();
		~Button();

		bool load(ID3D11Device* device, shared_ptr<FontSet> font,
			shared_ptr<ID3D11ShaderResourceView> pixelTexture);

		virtual void update(double deltaTime, MouseController* mouse) override;
		virtual void draw(SpriteBatch* batch) = 0;

		//virtual void setPosition(Vector2& position) override;
		virtual const Vector2& getPosition() override;

		//virtual void setScale(const Vector2 & scale) override;
		virtual int getWidth() override;
		virtual int getHeight() override;

		Color normalColor = Color((Vector3(1, 1, 1)));
		Color hoverColor = Color((Vector3(.5, .75, 1)));
		Color selectedColor = Color((Vector3(0, .5, 1)));

		virtual bool clicked() override;
		virtual bool selected() override;
		virtual bool hovering() override;

	protected:

		shared_ptr<ID3D11ShaderResourceView> pixel;

		//Vector2 position;
		/* hitArea is taken from Sprite. */
		//const HitArea* hitArea;



		///** Mouse is over button but mousebutton has not been pressed. */
		//bool isHover = false;
		///** Button is held down over mousebutton but has not been released. */
		//bool isSelected = false;
		//bool isClicked = false;



	};


	class TextButton : public Button {
	public:
		TextButton();
		~TextButton();


		/*bool load(ID3D11Device* device, const wchar_t* fontFile,
			const wchar_t* upButtonFile, const wchar_t* downButtonFile);*/


		virtual void setPosition(Vector2& position);
		void setText(string text);
		virtual void draw(SpriteBatch* batch);

	private:

		shared_ptr<FontSet> buttonFont;
		unique_ptr<TextLabel> buttonLabel;
	};


	/** A button created with dds files. Requires a Pressed button image (downButton)
		and an Unpressed image (upButton). */
	class ImageButton : public Button {
	public:
		ImageButton();
		~ImageButton();

		bool load(ID3D11Device* device,
			shared_ptr<Sprite> upButtonFile,
			shared_ptr<Sprite> downButtonFile);


		virtual void draw(SpriteBatch * batch) override;

	private:
		shared_ptr<Sprite> normalSprite;
		shared_ptr<Sprite> pressedSprite;
	};
};