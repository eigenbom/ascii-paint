#include "copy_operation.h"

#include "libtcod.hpp"
#include "gui/gui.hpp"
#include "app.h"
#include "brush.h"
#include "misc.h"

#include <cstdio>

CopyOperation::CopyOperation() {
}

CopyOperation::~CopyOperation() {
}

void CopyOperation::start() {
	justCleared = false;

	//app->gui->outlineToggleButton->setVisible(true);
	//app->gui->filledToggleButton->setVisible(true);
	//app->gui->outlineToggleButton->setPressed(true);
	//app->gui->filledToggleButton->setPressed(false);

	app->gui->useSymbolToggleButton->setVisible(false);
	app->gui->useForegroundToggleButton->setVisible(false);
	app->gui->useBackgroundToggleButton->setVisible(false);
	app->gui->useSolidToggleButton->setVisible(false);
}

void CopyOperation::update() {
	TCOD_mouse_t mouse = app->mouse;
	int mouseX = app->canvasMouseX;
	int mouseY = app->canvasMouseY;

	// If the user just let go of the mouse buttons
	if(mouse.lbutton == false && mouse.rbutton == false && active == true) {
		// TODO: Copy region to clipboard
		int rectEndX = mouseX;
		int rectEndY = mouseY;

		int left = std::min(rectEndX,rectStartX);
		int right = std::max(rectEndX,rectStartX);
		int top = std::min(rectEndY,rectStartY);
		int bottom = std::max(rectEndY,rectStartY);

		int width = right-left+1;
		int height = bottom-top+1;

		// printf("%d %d %d %d\n",left,top,width,height);

		app->copyCanvasToClipboard(left,top,width,height);

		// Clear overlay to signify that we've copied the region
		app->clearOverlay();
		justCleared = true;
	}

	// if the user press the left and right mouse button at the same time then
	// erase the current stroke
	if(mouse.lbutton == true && mouse.rbutton == true && active == true) {
		app->clearOverlay();
		justCleared = true; // justCleared is needed so that we don't get a dot at
		// the end. Just try and comment out that rect to see what happens. :)
	}

	if(mouse.lbutton == false && mouse.rbutton == false) {
		justCleared = false;
	}

	// If the user just started drawing the rect
	if((mouse.lbutton == true /*BP|| mouse.rbutton == true*/) && active == false) {
		rectStartX = mouseX;
		rectStartY = mouseY;
		}

	static Brush outlineBrush = {TCODColor::white,TCODColor::black,TCOD_CHAR_BLOCK1,true,true};

	app->clearOverlay();
	if((mouse.lbutton /*BP || mouse.rbutton */) && justCleared == false) {
		active = true;
		int rectEndX, rectEndY;
		rectEndX = mouseX;
		rectEndY = mouseY;

		// Draw the outline of a rect

		int x = rectStartX;
		int y = rectStartY;

		// Draw the top side of rect
		TCODLine::init(x, y, rectEndX, y);
		do {
			// Draw on the overlay using the outlineBrush
			app->applyBrushToOverlayCell(x, y, &outlineBrush);
		} while(!TCODLine::step(&x, &y));

		// Start a rect from starting mouse cell to new mouse cell
		TCODLine::init(x, y, rectEndX, rectEndY);
		do {
			// Draw on the overlay using the outlineBrush
			app->applyBrushToOverlayCell(x, y, &outlineBrush);
		} while(!TCODLine::step(&x, &y));

		// Start a rect from starting mouse cell to new mouse cell
		TCODLine::init(x, y, rectStartX, y);
		do {
			// Draw on the overlay using the outlineBrush
			app->applyBrushToOverlayCell(x, y, &outlineBrush);
		} while(!TCODLine::step(&x, &y));

		// Start a rect from starting mouse cell to new mouse cell
		TCODLine::init(x, y, x, rectStartY);
		do {
			// Draw on the overlay using the outlineBrush
			app->applyBrushToOverlayCell(x, y, &outlineBrush);
		} while(!TCODLine::step(&x, &y));

	} else {
		// if no button was pressed or we just cleared
		active = false;
		// Clear the overlay and draw a preview of the current outlineBrush on it
		app->applyBrushToOverlayCell(mouseX, mouseY, &outlineBrush);
	}

}

void CopyOperation::end() {

}
