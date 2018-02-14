#pragma once
#include "GUIControl.h"

/** Not yet implemented. */ 
class Panel : public GUIControlBox {
public:
	//Panel(const Vector2& position);
	~Panel();


	virtual bool update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;

	

	virtual void setFont(const pugi::char_t* font = "Default Font") override;
	virtual void setText(wstring text) override;
	/** Not used in Panel.*/
	virtual const Vector2 XM_CALLCONV measureString() const override;

	virtual const Vector2& getPosition() const override;
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;

	virtual bool clicked() override;
	virtual bool pressed() override;
	virtual bool hovering() override;

	virtual size_t addControl(unique_ptr<GUIControl> control) override;
	virtual void addControls(vector<unique_ptr<GUIControl>> controls) override;
	virtual GUIControl* getControl(size_t controlPosition) const override;
private:
	vector<unique_ptr<GUIControl> > controls;

	

};