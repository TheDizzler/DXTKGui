# DXTKGui v1.05b

A GUI Framework for DirectX ToolKit.
This developed out of one part not being able to find any existing frameworks and one part me procrastinating on developing the game I'm working on. I had some hacky GUI features (button, dialog box, listbox) and I was bothered by how inflexible and cumbersome they were. So I started this project to make an extensible, easy to use, and re-usable GUI toolkit.
I have tried to prioritize run-time efficiency over start-up, so at times a lot of redundant functions will be called multiple times when constructing and updating new controls. The best work around to these redundancies (that I can think of) would put more work on the end-user; however, reducing work for the end-user has been my number one goal for this project, so decreasing control construction/manipulate time will remain a mission for the future (perhaps).
Just keep that in mind if you are trying to create and manipulate a couple dozen controls in real-time! :)

This tool kit requires DirectXTK and PugiXML.

### The primary goal of this project was to have GUI controls at my fingertips with minimal hassle. This was never meant to be a professional looking, visually appealing framework, although I am trying to allow every control to have custom, imported graphics.


~~Included in this project is my custom 2D game engine using DirectX Tool Kit and the test project for trying out all the features.~~ Moved to another repo. This ONLY has the toolkit gui code.


You are free to use this code in anyway you see fit. However, if you do, I would really appreciate if you dropped me a line letting me know. And, of course, if you have any bug reports, suggestions, or requests, please send them along! Thanks!
Email: tgillon.sdk@gmail.com

##3rd party utilities used:
>- DirectXTK (https://blogs.msdn.microsoft.com/chuckw/2012/03/02/directxtk/)
>- PugiXML (http://pugixml.org/)


##How to set up: (Note: may be outdated)
	Start Win32 project
		set to window application, empty project, no SDL

	Install latest DirectXTK_desktop_2015 (2017.12.13.1 as of writing)
	Install pugixml							(1.8.5 as of writing)

	Copy /source and /assets
	Copy X3DAudio.lib & XAPOFX.lib

	Debug && x64
	
	Property Pages (All Configs && All Platforms)
	set precompiledheader && hid.lib
		Config Properties
			C/C++
				Precompiled Header
					Create (/Yc)
					File pch.h
				Advanced (optional?)
					Forced Include
						pch.h
			Linker
				Input
					Additional Dep
						hid.lib


	Add
		mouse->saveMouseState();
		slotManager->updateGamePads();
	to GameEngine::update()


How to Button:
	unique_ptr<Button> confirmButton;
	confirmButton.reset(guiFactory->createButton());
	confirmButton->setText(L"Le Text");
	confirmButton->setOnClickListener(new ButtonOnClickListener(this));

How to OnClickListener:
	class ButtonOnClickListener : public Button::OnClickListener {
	public:
		ButtonOnClickListener(Context* cntxt) : context(cntxt) {
		}
		virtual void onClick(Button* button) override;

	private:
		Context* context;
	};

How to Dialog:
	use to factory to create new
		dialog.reset(guiFactory->createDialog());
	set dimensions (mandatory), tint, buttons and listeners if desired
		dialog->setConfirmOnClickListener(new GenerateBuildingListener(this));
		dialog->setTitle(L"Building Setup");
	set text
	
	
	if want open
		dialog->open();
		
	add to update and draw

How to extend/sub-class:
	class ControllerDialog : public Dialog
		ControllerDialog(HWND hwnd,MouseController* mouse);
			initializeControl(guiFactory, mouse);
			initialize(guiFactory->getAsset("White Pixel"));
			setDimensions(position, size, frameThickness);
		


How to ScreenTransition:
	create a class that extends Screen
	
	unique_ptr<ScreenTransitions::ScreenTransitionManager> transitionManager;

	transitionManager.reset(
		new ScreenTransitions::ScreenTransitionManager(
			guiFactory.get(), nameOfBGHereIfNeeded));
	transitionManager->setTransition(
		some kind of Transition here (ex: ScreenTransitions::SquareFlipScreenTransition() )
				);
			
			

##DONE:
>- Optimized control draw speed (for almost everything)
>- Primitve Shapes - filled rectangle; un-filled rectangle; triangle (needs work); line
>- Text Labels - normal and clickable
>- Button - text, image, animated (animated buttons are hacky and ~~due for overhaul~~ probably shouldn't be used at all)
>- TexturePanel - can take a ShaderResourceView, display it, and scroll vertically
>- Dialog Box - draggable and non-draggable; implements TexturePanel to allow text scrolling; no image (bg) loading yet;
>- Dynamic Dialog that takes 9 images to make a variable sized dialog box
>- Check Box - with onClickListener
>- ListBox - with onClickListener; auto-size to content
>- Scrollbar - now with customizable sprites!
>- Combo Box
>- Spinner - can take any vector of wstring
>- transition effects (tested on screens and dialog only)
>- controls can store a tranlsation matrix function to allow their use with a camera
(world space vs screen space)
>- Controller controlled controls (joysticks && keyboard) via a SelectorManager (not at all elegant but works for multiple purposes)

##KNOWN ISSUES:
>- controls overlapping each other may all call their onClick functions instead of the just the topmost
>- when a control with a scrollbar has items removed from it dynamically, the scrubber doesn't resize properly
>- when using keyboard selector and mouse is over a selectable control, moused-over control will stay selected
>- lots of warnings to clean up (mostly possible data loss due to unchecked data type change)
##TODO:
>- Enable 2D array of selectable controls in SelectorManager. Currently, SelectorManager only provides a single line of selectable controls. 
>- A Panel for adding controls - this could be used to greatly reduce draw times
	when using multiple controls
>- ~~find out how to purge all the reported live objects ':/~~ 
Live objects ARE being released but after the debug report. <- FIXED
~~Still have some outstanding internal references.~~ Most appear gone
>- ~~Controller controlled controls (joysticks && keyboard)~~
>- Automated Button needs overhaul (or removal completely)
>- Dialog Box - allow hyphenating of words that can't fit on one line with option to turn on/off
(currently puts whole word on next line or puts break in word)
>- Horizontal scrollbar
>- TexturePanel - add horizontal scrolling
>- Scrollbars that can be slapped on to anything
>- Dialog Box - allow custom bg image;
>- List Box - multi-select; test adding, removing, clearing items live
>- Radio Buttons
>- Slider control
>- Combo Box - allow custom frame/bg image
>- ListBox - allow custom frame/bg image
>- ~~put default images into a single spritesheet~~
>- More Primitive Shapes - if I feel they might be useful
>- Other stuff I can't think of at the moment
>- demo and how-to videos
>- Some kind of document to explain what's going on under the hood and how to use it effectivily....
>- create .DLL version