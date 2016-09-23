#pragma once

#include <pugixml.hpp>
#include "../Engine/MouseController.h"
#include "../BaseGraphics/FontSet.h"



interface GUIControl : public IElement2D {
public:

	void setFactory(GUIFactory* factory);

	/* Deprecating */
	enum ClickAction {
		EXIT, PLAY, SETTINGS, CANCEL, OK, UP, DOWN, NONE, CONFIRM,
		NEUTRAL, SELECTION_CHANGED
	};

	virtual void update(double deltaTime, MouseController* mouse) = 0;

	virtual void setFont(const pugi::char_t* font = "Default Font") = 0;
	virtual void setText(wstring text) = 0;
	virtual const wchar_t* getText();
	virtual const Vector2& XM_CALLCONV measureString() const = 0;

	virtual void setScale(const Vector2& scl);

	virtual void moveBy(const Vector2& moveVector);
	virtual void setPosition(const Vector2& position) override;
	virtual void setOrigin(const Vector2& origin) override;
	/** Remember: Rotation is around the origin! */
	virtual void setRotation(const float rotation) override;
	virtual void setTint(const Color& color) override;
	virtual void setAlpha(const float alpha) override;

	virtual const Vector2& getPosition() const = 0;
	virtual const Vector2& getOrigin() const override;
	virtual const Vector2& getScale() const override;
	virtual const float getRotation() const override;
	virtual const Color& getTint() const override;
	virtual const float getAlpha() const override;
	virtual const int getWidth() const = 0;
	virtual const int getHeight() const = 0;
	bool contains(const Vector2& point);

	virtual bool clicked() = 0;
	/* Is Mouse Button down over control? */
	virtual bool pressed() = 0;
	virtual bool hovering() = 0;

	/* Deprecating this method of control. Use ActionListeners instead. */
	ClickAction action;


protected:
	unique_ptr<HitArea> hitArea;
	Vector2 position = Vector2::Zero;
	Vector2 scale = Vector2(1, 1);


	Vector2 origin = Vector2(0, 0);
	Color tint = DirectX::Colors::White.v;
	float alpha = 1.0f;
	float rotation = 0.0f;
	float layerDepth = 0.0f;

	bool isHover = false;
	/** Button is held down over control but has not been released. */
	bool isPressed = false;
	/** While still hovering over control, button has been pressed and released. */
	bool isClicked = false;


	GUIFactory* guiFactory;


};



/** A GUI control that can hold other GUI controls.
		Not actually implemented.... */
interface GUIControlBox : public GUIControl {
public:
	virtual void addItem(unique_ptr<GUIControl> control) = 0;
	virtual void addItems(vector<unique_ptr<GUIControl> > controls) = 0;

};
