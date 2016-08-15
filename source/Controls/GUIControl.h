#pragma once

#include "../Engine/MouseController.h"

namespace Controls {
	interface GUIControl {

		enum ClickAction {
			EXIT, PLAY, SETTINGS, CANCEL, OK, UP, DOWN

		};

		virtual void update(double deltaTime, MouseController* mouse) = 0;
		virtual void draw(SpriteBatch* batch) = 0;

		void setPosition(Vector2& pos) {
			position = pos;
			hitArea->position = Vector2(pos.x - getWidth()*scale.x / 2,
				pos.y - getHeight()*scale.y / 2);
			hitArea->size = Vector2(getWidth()*scale.x, getHeight()*scale.y);
		};

		virtual const Vector2& getPosition() = 0;

		void setScale(const Vector2& scl) {
			scale = scl;
			setPosition(position);
		};

		virtual int getWidth() = 0;
		virtual int getHeight() = 0;


		virtual bool clicked() = 0;
		virtual bool selected() = 0;
		virtual bool hovering() = 0;

		ClickAction action;


protected:
	unique_ptr<HitArea> hitArea;
	Vector2 position;
	Vector2 scale;

	bool isHover = false;
	/** Button is held down over control but has not been released. */
	bool isSelected = false;
	/** While still hovering over control, button has been pressed and released. */
	bool isClicked = false;

	};

};