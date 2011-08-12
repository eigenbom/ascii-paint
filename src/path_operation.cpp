#include <cstdio>
#include "libtcod.hpp"
#include "gui/gui.hpp"
#include "app.h"
#include "brush.h"
#include "path_operation.h"

PathOperation::PathOperation() {
}

PathOperation::~PathOperation() {
}

void PathOperation::start() {
	app->gui->pathLengthLabel->setVisible(true);
	app->gui->pathDiagonalToggleButton->setVisible(true);

	map = new TCODMap(app->canvasWidth, app->canvasHeight);
	for(int x = 0; x < app->canvasWidth; x++) {
		for(int y = 0; y < app->canvasHeight; y++) {
			if(app->solidCon->getCharBackground(x, y) == TCODColor(255, 255, 255)) // No Solid
				map->setProperties(x, y, true, true);
			else
				map->setProperties(x, y, false, false);
		}
	}

	allowDiagonal = app->gui->pathDiagonalToggleButton->isPressed();

	path = new TCODPath(map, allowDiagonal);
	pointX = -1;
	pointY = -1;
	
        app->setOverlayFade(0.25);
}

void PathOperation::update() {
	TCOD_mouse_t mouse = app->mouse;
	int mouseX = app->canvasMouseX;
	int mouseY = app->canvasMouseY;

	// If the mouse is inside the canvas area...
	if(mouseX >= 0 && mouseX < app->canvasWidth &&
			mouseY >= 0 && mouseY < app->canvasHeight) {

		if(mouse.lbutton_pressed) {
			pointX = mouseX;
			pointY = mouseY;
		}

		// If allowDiagonal was changed then make a new path
		if(allowDiagonal != app->gui->pathDiagonalToggleButton->isPressed()) {
			allowDiagonal = app->gui->pathDiagonalToggleButton->isPressed();
			delete path;
			path = new TCODPath(map, allowDiagonal);
		}


                if(pointX != -1 && pointY != -1) {
                    path->compute(pointX, pointY, mouseX, mouseY);
                }

		app->clearOverlay();
		Brush pathBrush;
		pathBrush.fore = TCODColor(0, 0, 0);
		pathBrush.back = TCODColor(255, 255, 0);
		pathBrush.symbol = ' ';


		static char* pathLengthLabelString = new char[20];
		sprintf(pathLengthLabelString, "Length: %i", path->size());
		app->gui->pathLengthLabel->setValue(pathLengthLabelString);

		for(int i = 0; i < path->size(); i++) {
			int x, y;
			path->get(i, &x, &y);
			app->applyBrushToOverlayCell(x, y, &pathBrush);
		}

		pathBrush.back = TCODColor(128, 128, 0);
		//app->applyBrushToOverlayCell(mouseX, mouseY, &pathBrush);
	}

}

void PathOperation::end() {
	delete map;
	delete path;
        app->setOverlayFade(1.0);
}
