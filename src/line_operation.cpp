#include "libtcod.hpp"
#include "gui/gui.hpp"
#include "app.h"
#include "brush.h"
#include "line_operation.h"

LineOperation::LineOperation() {
}

LineOperation::~LineOperation() {
}

void LineOperation::start() {
	justCleared = false;

	app->gui->useSymbolToggleButton->setVisible(true);
	app->gui->useForegroundToggleButton->setVisible(true);
	app->gui->useBackgroundToggleButton->setVisible(true);
	app->gui->useSolidToggleButton->setVisible(true);
}

void LineOperation::update() {
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

	// If the user just started drawing the line
	if((mouse.lbutton == true  /*BP|| mouse.rbutton == true*/) && active == false) {
		lineStartX = mouseX;
		lineStartY = mouseY;
	}

	app->clearOverlay();
	if((mouse.lbutton /*BP || mouse.rbutton */) && justCleared == false) {
		active = true;

		Brush *brush;

		// If pressing the left mouse button use brush1 otherwise brush2
		mouse.lbutton == true ? brush = &app->brush1 : brush = &app->brush2;
		
		int x = lineStartX;
		int y = lineStartY;

		int lineEndX, lineEndY;

		if(TCODConsole::isKeyPressed(TCODK_SHIFT)) {
			setStraightLineEndPoint(&lineEndX, &lineEndY);
		} else {
			lineEndX = mouseX;
			lineEndY = mouseY;
		}

		// Start a line from starting mouse cell to new mouse cell
		TCODLine::init(x, y, lineEndX, lineEndY);
		do {
			app->applyBrushToOverlayCell(x, y, brush);
		} while(!TCODLine::step(&x, &y));

	} else {
		// if no button was pressed or we just cleared
		active = false;
		// Draw a preview of brush1 on the overlay
		app->applyBrushToOverlayCell(mouseX, mouseY, &app->brush1);
	}
}

void LineOperation::end() {

}

// Sets the value of the line end point when the we want a straight line
void LineOperation::setStraightLineEndPoint(int* lineEndX, int* lineEndY) {
	int mouseX = app->canvasMouseX;
	int mouseY = app->canvasMouseY;

	float dx = abs(lineStartX - mouseX);
	float dy = abs(lineStartY - mouseY);

	// NOTE: should I deal with division by zero?
	
	// If the mouse is kind of diagonal to the start position
	if(dx/dy > 0.5 && dx/dy < 2) {
		// Make a diagonal line with the end position being the closest to the mouse
		if(dx > dy) {
			*lineEndX = mouseX;
			if(mouseY > lineStartY)
				*lineEndY = lineStartY + dx;
			else
				*lineEndY = lineStartY - dx;
		} else {
			*lineEndY = mouseY;
			if(mouseX > lineStartX)
				*lineEndX = lineStartX + dy;
			else
				*lineEndX = lineStartX - dy;
		}

	} else if(dx > dy) {
		// Make a straight horizontal line
		*lineEndX = mouseX;
		*lineEndY = lineStartY;
	} else if(dy > dx) {
		// Make a straight vertical line
		*lineEndX = lineStartX;
		*lineEndY = mouseY;
	} else {
		// Otherwise return the starting position
		*lineEndX = lineStartX;
		*lineEndY = lineStartY;
	}
}
