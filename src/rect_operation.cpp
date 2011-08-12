#include "libtcod.hpp"
#include "gui/gui.hpp"
#include "app.h"
#include "brush.h"
#include "misc.h"
#include "rect_operation.h"

RectOperation::RectOperation() {
}

RectOperation::~RectOperation() {
}

void RectOperation::start() {
	justCleared = false;

	//app->gui->outlineToggleButton->setVisible(true);
	//app->gui->filledToggleButton->setVisible(true);
	app->gui->outlineToggleButton->setPressed(true);
	app->gui->filledToggleButton->setPressed(false);

	app->gui->useSymbolToggleButton->setVisible(true);
	app->gui->useForegroundToggleButton->setVisible(true);
	app->gui->useBackgroundToggleButton->setVisible(true);
	app->gui->useSolidToggleButton->setVisible(true);
}

void RectOperation::update() {
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

	app->clearOverlay();
	if((mouse.lbutton /*BP || mouse.rbutton */) && justCleared == false) {
		active = true;

		Brush *outlineBrush;
		Brush *fillBrush;

		// If pressing the left mouse button use brush1 otherwise brush2
		if(mouse.lbutton == true) {
		   outlineBrush = &app->brush1;
		   fillBrush = &app->brush2;
		} else {
		   outlineBrush = &app->brush2;
		   fillBrush = &app->brush1;
		}

		int rectEndX, rectEndY;

		if(TCODConsole::isKeyPressed(TCODK_SHIFT)) {
			setSquareEndPoint(&rectEndX, &rectEndY);
		} else {
			rectEndX = mouseX;
			rectEndY = mouseY;
		}

		bool drawOutline = app->gui->outlineToggleButton->isPressed();
		bool drawFill = app->gui->filledToggleButton->isPressed();

		// If just drawing a fill then use the outline brush as the fill brush
		if(drawFill && !drawOutline) {
			fillBrush = outlineBrush;
		}

		if(drawFill) {
			// Draw a filled rectangle
			// Note: we use min and max so that the for loop always executes
			for(int x = min(rectStartX, rectEndX); x <= max(rectStartX, rectEndX); x++) {
				for(int y = min(rectStartY, rectEndY); y <= max(rectStartY, rectEndY); y++) {
					// Draw on the overlay using the fill brush
					app->applyBrushToOverlayCell(x, y, fillBrush);
				}
			}
		}

		if(drawOutline) {
			// Draw the outline of a rect

			int x = rectStartX;
			int y = rectStartY;

			// Draw the top side of rect
			TCODLine::init(x, y, rectEndX, y);
			do {
				// Draw on the overlay using the outlineBrush
				app->applyBrushToOverlayCell(x, y, outlineBrush);
			} while(!TCODLine::step(&x, &y));

			// Start a rect from starting mouse cell to new mouse cell
			TCODLine::init(x, y, rectEndX, rectEndY);
			do {
				// Draw on the overlay using the outlineBrush
				app->applyBrushToOverlayCell(x, y, outlineBrush);
			} while(!TCODLine::step(&x, &y));

			// Start a rect from starting mouse cell to new mouse cell
			TCODLine::init(x, y, rectStartX, y);
			do {
				// Draw on the overlay using the outlineBrush
				app->applyBrushToOverlayCell(x, y, outlineBrush);
			} while(!TCODLine::step(&x, &y));

			// Start a rect from starting mouse cell to new mouse cell
			TCODLine::init(x, y, x, rectStartY);
			do {
				// Draw on the overlay using the outlineBrush
				app->applyBrushToOverlayCell(x, y, outlineBrush);
			} while(!TCODLine::step(&x, &y));
		}
	} else {
		// if no button was pressed or we just cleared
		active = false;
		// Clear the overlay and draw a preview of the current outlineBrush on it
		app->applyBrushToOverlayCell(mouseX, mouseY, &app->brush1);
	}

}

void RectOperation::end() {

}

// Sets the value of the rect end point when the we want a square
void RectOperation::setSquareEndPoint(int* rectEndX, int* rectEndY) {
	int mouseX = app->canvasMouseX;
	int mouseY = app->canvasMouseY;

	float dx = abs(rectStartX - mouseX);
	float dy = abs(rectStartY - mouseY);

	// Make a square with the end position being as large as possible
	if(dx > dy) {
		*rectEndX = mouseX;
		if(mouseY > rectStartY)
			*rectEndY = rectStartY + dx;
		else
			*rectEndY = rectStartY - dx;
	} else {
		*rectEndY = mouseY;
		if(mouseX > rectStartX)
			*rectEndX = rectStartX + dy;
		else
			*rectEndX = rectStartX - dy;
	}

}

void FilledRectOperation::start() {
	justCleared = false;

	//app->gui->outlineToggleButton->setVisible(true);
	//app->gui->filledToggleButton->setVisible(true);
	app->gui->outlineToggleButton->setPressed(false);
	app->gui->filledToggleButton->setPressed(true);

	app->gui->useSymbolToggleButton->setVisible(true);
	app->gui->useForegroundToggleButton->setVisible(true);
	app->gui->useBackgroundToggleButton->setVisible(true);
	app->gui->useSolidToggleButton->setVisible(true);
}
