#pragma once

#include "../Engine/MouseController.h"

namespace Controls {
	interface GUIControl {

		virtual void update(double deltaTime, MouseController* mouse) = 0;
		virtual void draw(SpriteBatch* batch) = 0;

		virtual void setPosition(Vector2& position) = 0;
		virtual const Vector2& getPosition() = 0;

		virtual int getWidth() = 0;
		virtual int getHeight() = 0;
	};

};