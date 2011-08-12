#include "libtcod.hpp"
#include "gui/gui.hpp"
#include "app.h"
#include "brush.h"
#include "cell_operation.h"

CellOperation::CellOperation() {
}

CellOperation::~CellOperation() {
}

void CellOperation::start() {
	oldMouseX = app->canvasMouseX;
	oldMouseY = app->canvasMouseY;

	justCleared = false;

	app->gui->useSymbolToggleButton->setVisible(true);
	app->gui->useForegroundToggleButton->setVisible(true);
	app->gui->useBackgroundToggleButton->setVisible(true);
	app->gui->useSolidToggleButton->setVisible(true);
}

void CellOperation::update() {
	TCOD_mouse_t mouse = app->mouse;
	int mouseX = app->canvasMouseX;
	int mouseY = app->canvasMouseY;
	
	// If the user just let go of the mouse buttons
	if(mouse.lbutton == false && mouse.rbutton == false && active == true) {
		// First make a backup for undo
		app->addUndo();
		// Then blit the overlay onto the canvas
		app->applyOverlayToCanvas();
	}

	// if the user press the left and right mouse button at the same time then
	// erase the current stroke
	if(mouse.lbutton == true && mouse.rbutton == true && active == true) {
		app->clearOverlay();
		justCleared = true; // justCleared is needed so that we don't get a dot at
	   // the end. Just try and comment out that line to see what happens. :)
	}

	if(mouse.lbutton == false && mouse.rbutton == false) {
		justCleared = false;
	}

	// if((mouse.lbutton || mouse.rbutton) && justCleared == false) {
	if(mouse.lbutton and justCleared == false) {
		active = true;

		Brush *brush;

		// If pressing the left mouse button use brush1 otherwise brush2

		// BP: Disabled right mouse
		// mouse.lbutton == true ? brush = &app->brush1 : brush = &app->brush2;
		brush = &app->brush1;

		// In order to achieve continuous strokes we need to draw a line between the
		// previous mouse position and the new one.
		int x = oldMouseX;
		int y = oldMouseY;

		// Start a line from old mouse cell to new mouse cell
		TCODLine::init(x, y, mouseX, mouseY);
		do {
			app->applyBrushToOverlayCell(x, y, brush);
		} while(!TCODLine::step(&x, &y));

	} else {
		// if no button was pressed or we just cleared
		active = false;
		// Clear the overlay and draw a preview of brush1 on it
                app->clearOverlay();
		app->applyBrushToOverlayCell(mouseX, mouseY, &app->brush1);
	}

	oldMouseX = app->canvasMouseX;
	oldMouseY = app->canvasMouseY;
}

void CellOperation::end() {

}
