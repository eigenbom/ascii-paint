#include "libtcod.hpp"
#include "gui/gui.hpp"
#include "app.h"
#include "brush.h"
#include "misc.h"
#include "oval_operation.h"
#include <cmath>

OvalOperation::OvalOperation() {
}

OvalOperation::~OvalOperation() {
}

void OvalOperation::start() {
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

void OvalOperation::update() {
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
		// the end. Just try and comment out that oval to see what happens. :)
	}

	if(mouse.lbutton == false && mouse.rbutton == false) {
		justCleared = false;
	}

	// If the user just started drawing the oval
	if((mouse.lbutton == true /*BP|| mouse.rbutton == true*/) && active == false) {
		ovalStartX = mouseX;
		ovalStartY = mouseY;
	}

	app->clearOverlay();
	if((mouse.lbutton /* BP || mouse.rbutton */) && justCleared == false) {
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
		
		int ovalEndX, ovalEndY;

		if(TCODConsole::isKeyPressed(TCODK_SHIFT)) {
			setCircleEndPoint(&ovalEndX, &ovalEndY);
		} else {
			ovalEndX = mouseX;
			ovalEndY = mouseY;
		}

		bool drawOutline = app->gui->outlineToggleButton->isPressed();
		bool drawFill = app->gui->filledToggleButton->isPressed();
		
		// If just drawing a fill then use the outline brush as the fill brush
		if(drawFill && !drawOutline) {
			fillBrush = outlineBrush;
		}

		float xRadius = abs(ovalStartX - ovalEndX) / 2.0;
		float yRadius = abs(ovalStartY - ovalEndY) / 2.0;
		float xCenter = (ovalStartX + ovalEndX) / 2.0;
		float yCenter = (ovalStartY + ovalEndY) / 2.0;

		if(drawFill) {
			// Draw the outline of an oval
			for(int x = min(ovalStartX, ovalEndX); x <= max(ovalEndX, ovalStartX); x++) {
				app->applyBrushToOverlayCell(x, round(sqrt(xRadius*xRadius*yRadius*yRadius - (yRadius*yRadius)*(x-xCenter)*(x-xCenter))/xRadius + yCenter), outlineBrush);
				app->applyBrushToOverlayCell(x, round(-sqrt(xRadius*xRadius*yRadius*yRadius - (yRadius*yRadius)*(x-xCenter)*(x-xCenter))/xRadius + yCenter), outlineBrush);
			}

			for(int y = min(ovalStartY, ovalEndY); y <= max(ovalEndY, ovalStartY); y++) {
				app->applyBrushToOverlayCell(round(sqrt(yRadius*yRadius*xRadius*xRadius - (xRadius*xRadius)*(y-yCenter)*(y-yCenter))/yRadius + xCenter), y, outlineBrush);
				app->applyBrushToOverlayCell(round(-sqrt(yRadius*yRadius*xRadius*xRadius - (xRadius*xRadius)*(y-yCenter)*(y-yCenter))/yRadius + xCenter), y, outlineBrush);
			}

			// Now draw the filled oval
			for(int x = min(ovalStartX, ovalEndX); x < max(ovalEndX, ovalStartX); x++) {
				int y1 = round(sqrt(xRadius*xRadius*yRadius*yRadius - (yRadius*yRadius)*(x-xCenter)*(x-xCenter))/xRadius + yCenter);
				int y2 = round(-sqrt(xRadius*xRadius*yRadius*yRadius - (yRadius*yRadius)*(x-xCenter)*(x-xCenter))/xRadius + yCenter);
				for(int i = y1; i > y2; i--) {
					app->applyBrushToOverlayCell(x, i, fillBrush);
				}
			}
			for(int y = min(ovalStartY, ovalEndY); y < max(ovalEndY, ovalStartY); y++) {
				int x1 = round(sqrt(yRadius*yRadius*xRadius*xRadius - (xRadius*xRadius)*(y-yCenter)*(y-yCenter))/yRadius + xCenter);
				int x2 = round(-sqrt(yRadius*yRadius*xRadius*xRadius - (xRadius*xRadius)*(y-yCenter)*(y-yCenter))/yRadius + xCenter);
				for(int i = x1; i > x2; i--) {
					app->applyBrushToOverlayCell(i, y, fillBrush);
				}
			}
		}

		if(drawOutline) {
			for(int x = min(ovalStartX, ovalEndX); x <= max(ovalEndX, ovalStartX); x++) {
				app->applyBrushToOverlayCell(x, round(sqrt(xRadius*xRadius*yRadius*yRadius - (yRadius*yRadius)*(x-xCenter)*(x-xCenter))/xRadius + yCenter), outlineBrush);
				app->applyBrushToOverlayCell(x, round(-sqrt(xRadius*xRadius*yRadius*yRadius - (yRadius*yRadius)*(x-xCenter)*(x-xCenter))/xRadius + yCenter), outlineBrush);
			}

			for(int y = min(ovalStartY, ovalEndY); y <= max(ovalEndY, ovalStartY); y++) {
				app->applyBrushToOverlayCell(round(sqrt(yRadius*yRadius*xRadius*xRadius - (xRadius*xRadius)*(y-yCenter)*(y-yCenter))/yRadius + xCenter), y, outlineBrush);
				app->applyBrushToOverlayCell(round(-sqrt(yRadius*yRadius*xRadius*xRadius - (xRadius*xRadius)*(y-yCenter)*(y-yCenter))/yRadius + xCenter), y, outlineBrush);
			}
		}
	} else {
		// if no button was pressed or we just cleared
		active = false;
		// Clear the overlay and draw a preview of the current outlineBrush on it
		app->applyBrushToOverlayCell(mouseX, mouseY, &app->brush1);
	}
}

void OvalOperation::end() {

}

// Sets the value of the oval end point when the we want a circle
void OvalOperation::setCircleEndPoint(int* ovalEndX, int* ovalEndY) {
	int mouseX = app->canvasMouseX;
	int mouseY = app->canvasMouseY;

	float dx = abs(ovalStartX - mouseX);
	float dy = abs(ovalStartY - mouseY);

	// Make a circle with the end position being as large as possible
	if(dx > dy) {
		*ovalEndX = mouseX;
		if(mouseY > ovalStartY)
			*ovalEndY = ovalStartY + dx;
		else
			*ovalEndY = ovalStartY - dx;
	} else {
		*ovalEndY = mouseY;
		if(mouseX > ovalStartX)
			*ovalEndX = ovalStartX + dy;
		else
			*ovalEndX = ovalStartX - dy;
	}

}

void OvalFOperation::start() {
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
