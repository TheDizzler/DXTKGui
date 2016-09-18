# DXTKGui v0.31b

A GUI Framework for DirectX ToolKit.
This developed out of one part not being able to find any existing frameworks and one part me procrastinating on developing the game I'm working on. I had some hacky GUI features (button, dialog box, listbox) and I was bothered by how inflexible and cumbersome they were. So I started this project to make an extensible and re-usable GUI framework (re-usable in my game engines anyway).

This tool kit requires DirectXTK, or a SpriteBatch that works identically to the DirectXTK SpriteBatch. It also requires the mouse controller from my engine (does not take keyboard input. Yet, if ever). I wil probably make a mouse interface class so that the required mouse functions can be implemented in your (probably better) mouse controller.

### The primary goal of this project was to have GUI controls at my fingertips with minimal hassle. This was never meant to be a professional looking, visually appealing framework, although I am trying to allow every control to have custom, imported graphics.


Included in this project is my custom 2D game engine using DirectX Tool Kit and the test project for trying out all the features.
I literally ripped this out of one of my current projects and I'm in the process of hacking it into a usable state.
This is still a massive mess....


##3rd party utilities used:
>- DirectXTK (https://blogs.msdn.microsoft.com/chuckw/2012/03/02/directxtk/)
>- PugiXML (http://pugixml.org/)


##DONE:
>- Primitve Shapes - filled rectangle; un-filled rectangle; line
>- Text Labels - normal and clickable
>- Button - text and image
>- Dialog Box - draggable and non-draggable; no image loading yet;
>- Check Box - with onClickListener
>- ListBox - with onClickListener; auto-size to content
>- Scrollbar - now with customizable sprites!
>- Combo Box


##TODO:
>- Dialog Box - allow custom bg image; transition effects
>- Scrollbar - that can be slapped on to anything
>- List Box - multi-select; test adding, removing, clearing items live
>- Radio Buttons
>- Slider
>- add triggers for sfx
>- screen transition effects
>- allow the use of sprite sheets
>- Combo Box - allow custom frame/bg image
>- ListBox - allow custom frame/bg image
>- tooltips?
>- More Primitive Shapes - if I feel they might be useful
>- Other stuff I can't think of at the moment
>- Some kind of document to explain what's going on under the hood and how to use it effectivily....

