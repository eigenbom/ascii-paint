This is a fork of Ascii-Paint ( http://code.google.com/p/ascii-paint/ ), revamping the GUI and adding a number of features.

Introduction
============
ASCII-Paint is a simple open source painting program which uses ASCII characters instead of pixels. It is made for roguelike developers but it can be used by anyone. It uses the excellent libtcod. 

Quick Start
-----------

The program is quite self-explanatory though there are a few things that are not entirely obvious.

Left clicking draws with the primary brush, right clicking draws with the secondary brush 
Press Tab to hide/show the GUI 
When drawing text, type the text, click the position where you want to place it then press enter to confirm 
Press CTRL+Left Click to drag the canvas 
Saving a file ending with a 'z' makes a compressed file(eg. myfile.ascz, myfile.acz, myfile.gz)

Also take a look at the color selector section

Features
-----------
As of version 0.3.2 the following features are available:

GUI
*  GUI based interface with tooltips 
*  Keyboard shortcuts 
*  Palette color chooser 
*  Fullscreen color chooser 
*  Symbol chooser 
*  Fullscreen support 
*  Hiding/Unhiding GUI (pressing TAB) 
*  Move mouse with keyboard (arrows and num pad) 
*  Image and solid view modes
*  Grid

Drawing
*  Arbitrary sized canvases
*  Draw single cells, lines, rectangles, ovals and text 
*  Holding shift draws straight lines/squares/ovals 
*  Flood filling 
*  Color picker 
*  Copy/Paste
*  Undo/redo 
*  Roguelike Tools
*  Cells can be solid
*  Field of view tool 
*  4 FOV algorithms 
*  Path finding tool

File handling
*  Opening and saving .asc files
*  Opening and saving compressed files (files ending with a 'z') 
*  Importing text, foreground and background colors 
*  Exporting .png, .bmp, text, foreground and background colors
*  Exporting to animated .gif (the program dices up the canvas into frames)

Configuration
*  Configurable options (ascii-paint.cfg) 
*  Palettes (palettes.cfg)
*  Window size 
*  Full screen 
*  Window Background color 
*  FPS (frames per second)
*  Font
*  Canvas Size 
*  Initial brush properties 
*  GUI colors

Shortcuts
---------

* _CTRL+N_ New
 
CTRL+O
 Open
 
CTRL+S
 Save
 
CTRL+Q
 Quit
 
CTRL+Z
 Undo
 
CTRL+Y Redo
 
CTRL+H Hide/Show mouse
 
TAB
 Hide/Show GUI
 
c
 Cell
 
l
 Line
 
r
 Rect
 
o
 Oval
 
f
 Fill
 
t
 Text
 
p
 Pick
 
Arrow Keys
 Move the mouse
 
Numpad
 Move the mouse
 
CTRL+Left Click
 Drag the canvas view
 
CTRL+Right Click
 Reset canvas view
 
ALT+ENTER
 Switch to Fullscreen/Windowed
 
SHIFT
 Draw straight line/square/circle
 


Color Selector
You can open up the full screen color selector by clicking on a color box.
To select a color click on it.
To change the saturation of the colors, right click a color from the gray colors.


Export/Import
You can import and export text, foreground and background colors. In addition you can also export the current image to bmp and png.
Importing and exporting text is done by .txt files.
Foreground and background colors are .pngs where each pixel represents the foreground or background color of a cell in the image.

FOV
FOV stands for Field Of View. FOV is used to see what someone standing at some point can see. ASCII-Paint currently offers 4 types of algorithms:
(taken from libtcod docs)
Basic : classic libtcod fov algorithm (ray casted from the player to all the cells on the submap perimeter)
Diamond : based on this algorithm
Shadow : based on this algorithm
Permissive : based on this this algorithm 
Configuration
ASCII-Paint is configured using the ascii-paint.cfg file. Most of the options are quite obvious. In case you screw up the file, just copy ascii-paint.cfg.bak into ascii-paint.cfg.
Colors are specified by "#RRGGBB" or "rrr, ggg, bbb".

For the font layout the following three options are available: 

"incol" = the font has the characters going down in columns. ie b is underneath a

"inrow" = the font has the characters going across in rows. ie. b is to the right of a

"tcod" = for custom characters. Is not fully implemented yet. 
The font type is either "greyscale" or "normal". "greyscale" means that gray pixels will be considered transparent. "normal" means all non-black pixels will be considered opaque.

Extra tips
During the middle of a drawing operation, press the opposite mouse button erases the drawing. eg if you are holding down the left mouse button while drawing a line and then press the right mouse button at the same time, then that line is erased. 


Building

To build use cmake:

    mkdir build
    cd build
    cmake ..
    make
    make install

Then run from within bin/
