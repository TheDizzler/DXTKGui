#pragma once

#include <pugixml.hpp>

#include "../BaseGraphics/FontSet.h"
#include "../BaseGraphics/GraphicsAsset.h"
#include "../Controllers/MouseController.h"

class GUIFactory;
interface GUIControl : public IElement2D {
public:

	GUIControl(GUIFactory* factory,
		MouseController* mouseController) {
		guiFactory = factory;
		mouse = mouseController;
		

		translationMatrix = [&]() -> Matrix { return Matrix::Identity; };
		cameraZoom = [&]() -> float { return 1; };
	}

	virtual ~GUIControl();

	virtual void reloadGraphicsAsset() = 0;

	/** Return true if GUIControl has refreshed its texture and parent GUIControls
		need to redraw themselves. */
	virtual bool update(double deltaTime) = 0;

	virtual void setFont(const pugi::char_t* font = "Default Font") = 0;
	virtual void setText(wstring text) = 0;
	virtual const wchar_t* getText();
	virtual const Vector2 XM_CALLCONV measureString() const = 0;

	virtual void setScale(const Vector2& scl);

	virtual void moveBy(const Vector2& moveVector);
	virtual void setPosition(const Vector2& position) override;
	virtual void setOrigin(const Vector2& origin) override;
	/** Remember: Rotation is around the origin! */
	virtual void setRotation(const float rotation) override;
	virtual void setTint(const XMFLOAT4 color) override;
	virtual void setTint(const Color& color) override;
	virtual void setTint(const XMVECTORF32 color) override;
	virtual void setAlpha(const float alpha) override;
	virtual void setLayerDepth(const float depth, bool frontToBack = true) override;
	/** Warning: calling setPosition() or setScale() will reset the HitArea! */
	virtual void setHitArea(HitArea* newHitArea);

	virtual const Vector2& getPosition() const = 0;
	virtual const Vector2& getOrigin() const override;
	virtual const Vector2& getScale() const override;
	virtual const float getRotation() const override;
	virtual const Color& getTint() const override;
	virtual const float getAlpha() const override;
	virtual const int getWidth() const = 0;
	virtual const int getHeight() const = 0;
	virtual const float getLayerDepth() const override;

	const HitArea& getHitArea() const;

	/** Usage example:
		control->setMatrixFunction([&]() -> Matrix { return camera->translationMatrix(); }); */
	void setMatrixFunction(function<Matrix()> translationFunction) {
		translationMatrix = translationFunction;
	}
	/** Usage example:
		control->setCameraZoom([&]()->float { return camera->getZoom(); }); */
	void setCameraZoom(function<float()> zoomFunction) {
		cameraZoom = zoomFunction;
	}



	virtual void updateProjectedHitArea();

	virtual const Vector2 getScreenPosition(Matrix viewProjectionMatrix) const;
	virtual unique_ptr<HitArea> getScreenHitArea(Matrix viewProjectionMatrix) const;

	virtual bool contains(const Vector2& point) override;

	/** Action to perform when mouse button released over control. */
	virtual void onClick() = 0;
	/** Action to perform when mouse button held over control. */
	virtual void onPress() = 0;
	/** Action to perform when mouse over control. */
	virtual void onHover() = 0;
	/** Action to perform to return control back to neutral state. */
	virtual void resetState() = 0;

	virtual bool clicked() = 0;
	/* Is Mouse Button down over control? */
	virtual bool pressed() = 0;
	virtual bool hovering() = 0;


protected:
	function<Matrix()> translationMatrix;
	function<float()> cameraZoom;
	HitArea hitArea;

	HitArea projectedHitArea;

	Vector2 position = Vector2::Zero;
	Vector2 scale = Vector2(1, 1);


	Vector2 origin = Vector2(0, 0);
	Color tint = DirectX::Colors::White;
	float rotation = 0.0f;
	float layerDepth = 0.9f;

	bool isHover = false;
	/** Button is held down over control but has not been released. */
	bool isPressed = false;
	/** While still hovering over control, button has been pressed and released. */
	bool isClicked = false;


	GUIFactory* guiFactory;
	MouseController* mouse;

};



/** A GUI control that can hold other GUI controls.
		Not actually implemented.... */
interface GUIControlBox : public GUIControl {
public:
	GUIControlBox(GUIFactory* factory, MouseController* mouseController)
		: GUIControl(factory, mouseController) {
	}
	virtual size_t addControl(unique_ptr<GUIControl> control) = 0;
	virtual void addControls(vector<unique_ptr<GUIControl> > controls) = 0;

	virtual GUIControl* getControl(size_t controlPosition) const = 0;
};


/** A GUIControl that can be used in a SelectionManager. */
interface Selectable : public GUIControl {
public:
	Selectable(GUIFactory* factory, MouseController* mouseController)
		: GUIControl(factory, mouseController) {
	}
	virtual bool updateSelect(double deltaTime) = 0;
};
