# DXTKGui v0.8b

A GUI Framework for DirectX ToolKit.
This developed out of one part not being able to find any existing frameworks and one part me procrastinating on developing the game I'm working on. I had some hacky GUI features (button, dialog box, listbox) and I was bothered by how inflexible and cumbersome they were. So I started this project to make an extensible, easy to use, and re-usable GUI toolkit.
I have tried to prioritize run-time efficiency over start-up, so at times a lot of redundant functions will be called multiple times when constructing and updating new controls. The best work around to these redundancies (that I can think of) would put more work on the end-user; however, reducing work for the end-user has been my number one goal for this project, so decreasing control construction/manipulate time will remain a mission for the future (perhaps).
Just keep that in mind if you are trying to create and manipulate a couple dozen controls in real-time! :)

This tool kit requires DirectXTK.

### The primary goal of this project was to have GUI controls at my fingertips with minimal hassle. This was never meant to be a professional looking, visually appealing framework, although I am trying to allow every control to have custom, imported graphics.


Included in this project is my custom 2D game engine using DirectX Tool Kit and the test project for trying out all the features.


You are free to use this code in anyway you see fit. However, if you do, I would really appreciate if you dropped me a line letting me know. And, of course, if you have any bug reports, suggestions, or requests, please send them along! Thanks!
Email: tgillon.sdk@gmail.com

##3rd party utilities used:
>- DirectXTK (https://blogs.msdn.microsoft.com/chuckw/2012/03/02/directxtk/)
>- PugiXML (http://pugixml.org/)


##DONE:
>- Optimized draw speed for most controls
>- Primitve Shapes - filled rectangle; un-filled rectangle; triangle (needs work); line
>- Text Labels - normal and clickable
>- Button - text, image, animated
>- TexturePanel - can take a ShaderResourceView, display it, and scroll vertically
>- Dialog Box - draggable and non-draggable; implements TexturePanel to allow text scrolling; no image (bg) loading yet;
>- Dynamic Dialog that takes 9 images to make a variable sized dialog box
>- Check Box - with onClickListener
>- ListBox - with onClickListener; auto-size to content
>- Scrollbar - now with customizable sprites!
>- Combo Box
>- Spinner - can take any vector of wstring
>- can load images and animations from spritesheets
>- transition effects (tested on screens and dialog only)
>- controls can store a tranlsation matrix function to allow their use with a camera
(world space vs screen space)

##KNOWN ISSUES:
>- controls overlapping each other may all call their onClick functions instead of the just the topmost
>- when a control with a scrollbar has items removed from it dynamically, the scrubber doesn't resize properly

##TODO:
>- find out how to purge all the reported live objects ':/
>- Need to go check for circular shared_ptr references
>- Controller controlled controls (joysticks)
>- Button doesn't show hover again after click and release (fixed?)
>- Dialog Box - allow hyphenating of words that can't fit on one line with option to turn on/off
(currently puts whole word on next line or puts break in word)
>- ~~control contents should adjust for frame thickness~~
>- ~~Dialog Box buttons don't position properly when adding new buttons manually (only cancel button?)~~
>- TexturePanel - add horizontal scrolling
>- A Panel for adding controls?
>- Dialog Box - allow custom bg image;
>- Scrollbar that can be slapped on to anything; horizontal scrolling
>- List Box - multi-select; test adding, removing, clearing items live
>- Radio Buttons
>- Slider
>- add triggers for sfx (can use actionlisteners...good enough?)
>- Combo Box - allow custom frame/bg image
>- ListBox - allow custom frame/bg image
>- put default images into a spritesheet
>- tooltips?
>- More Primitive Shapes - if I feel they might be useful
>- Other stuff I can't think of at the moment
>- demo and how-to videos
>- Some kind of document to explain what's going on under the hood and how to use it effectivily....
>- create .DLL version