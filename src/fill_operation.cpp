#include "libtcod.hpp"
#include "gui/gui.hpp"
#include "app.h"
#include "brush.h"
#include "fill_operation.h"

FillOperation::FillOperation() {
}

FillOperation::~FillOperation() {
}

void FillOperation::start() {
	oldMouse = app->mouse;
	justCleared = false;

	app->gui->useSymbolToggleButton->setVisible(true);
	app->gui->useForegroundToggleButton->setVisible(true);
	app->gui->useBackgroundToggleButton->setVisible(true);
	app->gui->useSolidToggleButton->setVisible(true);
}

void FillOperation::update() {
	TCOD_mouse_t mouse = app->mouse;
	int mouseX = app->canvasMouseX;
	int mouseY = app->canvasMouseY;

	if((mouse.lbutton && !oldMouse.lbutton)){
		// BP: disable mouse right
		//||}	(mouse.rbutton && !oldMouse.rbutton)) {
		active = true;

		Brush *fillBrush;

		// If pressing the left mouse button use brush1 otherwise brush2
		mouse.lbutton == true ? fillBrush = &app->brush1 : fillBrush = &app->brush2;

		Brush currentCellBrush;
		currentCellBrush.symbol = app->canvasCon->getChar(mouseX, mouseY);
		currentCellBrush.fore = app->canvasCon->getCharForeground(mouseX, mouseY);
		currentCellBrush.back = app->canvasCon->getCharBackground(mouseX, mouseY);
		if(app->solidCon->getCharBackground(mouseX, mouseY) == TCODColor(0, 0, 255))
			currentCellBrush.solid = true;
		else
			currentCellBrush.solid = false;

		if(!(currentCellBrush.symbol == fillBrush->symbol &&
					currentCellBrush.fore == fillBrush->fore &&
					currentCellBrush.back == fillBrush->back &&
					currentCellBrush.solid == fillBrush->solid))
		{
			// First make a backup for undo
			app->addUndo();

			// Do a fill on the overlay then blit the overlay onto the canvas
			app->applyCanvasToOverlay();
			doFill(mouseX, mouseY, &currentCellBrush, fillBrush);
			app->applyOverlayToCanvas();
			app->clearOverlay();
		}
	} else {
		// if no button was pressed or we just cleared
		active = false;
		// Draw a preview of brush1 on the overlay
		app->clearOverlay();
		app->applyBrushToOverlayCell(mouseX, mouseY, &app->brush1);
	}
	oldMouse = mouse;
}

void FillOperation::end() {

}

void FillOperation::doFill(int x, int y, Brush *oldBrush, Brush *newBrush) {
	if(x < app->canvasWidth && x >= 0 && y < app->canvasHeight && y >= 0) {
		if(cellUsesBrush(x, y, oldBrush)) {
			app->applyBrushToOverlayCell(x, y, newBrush);

			doFill(x - 1, y, oldBrush, newBrush);
			doFill(x + 1, y, oldBrush, newBrush);
			doFill(x, y - 1, oldBrush, newBrush);
			doFill(x, y + 1, oldBrush, newBrush);
		}
	}

}

bool FillOperation::cellUsesBrush(int x, int y, Brush* brush) {
	if(app->overlayCon->getCharBackground(x, y) == brush->back &&
			app->overlayCon->getCharForeground(x, y) == brush->fore &&
			app->overlayCon->getChar(x, y) == brush->symbol) {
		TCODColor col = app->solidOverlayCon->getCharBackground(x, y);
		if((brush->solid && app->solidOverlayCon->getCharBackground(x, y) == TCODColor(0, 0, 255)) ||
				(!brush->solid && app->solidOverlayCon->getCharBackground(x, y) == TCODColor(255, 255, 255))) {
			return true;
		}
	}

	return false;
}
