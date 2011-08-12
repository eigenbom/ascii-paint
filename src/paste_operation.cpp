#include "paste_operation.h"

#include "libtcod.hpp"
#include "gui/gui.hpp"
#include "app.h"
#include "brush.h"
#include "misc.h"

PasteOperation::PasteOperation() {
}

PasteOperation::~PasteOperation() {
}

void PasteOperation::start() {
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

void PasteOperation::update() {
	TCOD_mouse_t mouse = app->mouse;
	int mouseX = app->canvasMouseX;
	int mouseY = app->canvasMouseY;

	// hack
	/*
	active = true;
	app->clearOverlay();
	app->applyClipboardToOverlay(mouseX,mouseY);
	return;
	*/

	// If the user just let go of the mouse buttons
	if(mouse.lbutton == false && mouse.rbutton == false && active == true) {
		app->applyClipboardToCanvas(mouseX,mouseY);

		// Clear overlay to signify that we've copied the region
		app->clearOverlay();
		justCleared = true;
	}

	/*
	// if the user press the left and right mouse button at the same time then
	// erase the current stroke
	if(mouse.lbutton == true && mouse.rbutton == true && active == true) {
		app->clearOverlay();
		justCleared = true; // justCleared is needed so that we don't get a dot at
		// the end. Just try and comment out that rect to see what happens. :)
	}
	*/

	if(mouse.lbutton == false && mouse.rbutton == false) {
		justCleared = false;
	}

	app->clearOverlay();
	app->applyClipboardToOverlay(mouseX,mouseY);

	if((mouse.lbutton /*BP || mouse.rbutton */) && justCleared == false) {
		active = true;
	}
	else {
		active = false;
		// draw clipboard to overlay at mouseX, mouseY
		// app->applyBrushToOverlayCell(mouseX, mouseY, &outlineBrush);
	}
}

void PasteOperation::end() {

}
