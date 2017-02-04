#pragma once

#include <pugixml.hpp>

#include "../BaseGraphics/FontSet.h"
#include "../BaseGraphics/GraphicsAsset.h"
#include "../Controllers/MouseController.h"

interface GUIControl : public IElement2D {
public:

	void initializeControl(GUIFactory* factory,
		shared_ptr<MouseController> mouseController);

	/* Deprecating */
	enum ClickAction {
		EXIT, PLAY, SETTINGS, CANCEL, OK, UP, DOWN, NONE, CONFIRM,
		NEUTRAL, SELECTION_CHANGED
	};


	virtual void update(double deltaTime) = 0;

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
	virtual void setTint(const XMFLOAT4 color) override;
	virtual void setAlpha(const float alpha) override;
	virtual void setLayerDepth(const float depth) override;

	virtual const Vector2& getPosition() const = 0;
	virtual const Vector2& getOrigin() const override;
	virtual const Vector2& getScale() const override;
	virtual const float getRotation() const override;
	virtual const Color& getTint() const override;
	virtual const float getAlpha() const override;
	virtual const int getWidth() const = 0;
	virtual const int getHeight() const = 0;
	virtual const float getLayerDepth() const override;

	const HitArea* getHitArea() const;

	/* example of use:
		control->setMatrixFunction([&]() -> Matrix { return camera->translationMatrix(); }); */
	void setMatrixFunction(function<Matrix()> translationMat) {
		translationMatrix = translationMat;
	}

	virtual void updateProjectedHitArea();

	virtual const Vector2& getScreenPosition(Matrix viewProjectionMatrix) const;
	virtual unique_ptr<HitArea> getScreenHitArea(Matrix viewProjectionMatrix) const;

	bool contains(const Vector2& point);

	GraphicsAsset* createTexture();

	virtual bool clicked() = 0;
	/* Is Mouse Button down over control? */
	virtual bool pressed() = 0;
	virtual bool hovering() = 0;

	/* Deprecating this method of control. Use ActionListeners instead. */
	ClickAction action;


protected:
	function<Matrix()> translationMatrix;
	unique_ptr<HitArea> hitArea;

	unique_ptr<HitArea> projectedHitArea;

	Vector2 position = Vector2::Zero;
	Vector2 scale = Vector2(1, 1);


	Vector2 origin = Vector2(0, 0);
	Color tint = DirectX::Colors::White;
	float rotation = 0.0f;
	float layerDepth = 1.0f;

	bool isHover = false;
	/** Button is held down over control but has not been released. */
	bool isPressed = false;
	/** While still hovering over control, button has been pressed and released. */
	bool isClicked = false;


	GUIFactory* guiFactory;
	shared_ptr<MouseController> mouse;

};



/** A GUI control that can hold other GUI controls.
		Not actually implemented.... */
interface GUIControlBox : public GUIControl {
public:
	virtual size_t addControl(unique_ptr<GUIControl> control) = 0;
	virtual void addControls(vector<unique_ptr<GUIControl> > controls) = 0;

	virtual GUIControl* getControl(size_t controlPosition) const = 0;
};
