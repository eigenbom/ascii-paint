#include "libtcod.hpp"
#include "gui/gui.hpp"
#include "app.h"
#include "brush.h"
#include "pick_operation.h"

PickOperation::PickOperation() {
}

PickOperation::~PickOperation() {
}

void PickOperation::start() {
	app->gui->pickSymbolToggleButton->setVisible(true);
	app->gui->pickForegroundToggleButton->setVisible(true);
	app->gui->pickBackgroundToggleButton->setVisible(true);
	app->gui->pickSolidToggleButton->setVisible(true);
}

void PickOperation::update() {
	TCOD_mouse_t mouse = app->mouse;
	int mouseX = app->canvasMouseX;
	int mouseY = app->canvasMouseY;

	if(mouse.lbutton_pressed /*BP|| mouse.rbutton_pressed*/) {
		Brush *brush;
		// If pressing the left mouse button use brush1 otherwise brush2
		mouse.lbutton_pressed == true ? brush = &app->brush1 : brush = &app->brush2;

		// Pick the cell according to the options
		if(app->gui->pickSymbolToggleButton->isPressed())
			brush->symbol = app->canvasCon->getChar(mouseX, mouseY);
		if(app->gui->pickForegroundToggleButton->isPressed())
			brush->fore = app->canvasCon->getCharForeground(mouseX, mouseY);
		if(app->gui->pickBackgroundToggleButton->isPressed())
			brush->back = app->canvasCon->getCharBackground(mouseX, mouseY);
		if(app->gui->pickSolidToggleButton->isPressed()) {
			if(app->solidCon->getCharBackground(mouseX, mouseY) == TCODColor(0, 0, 255)) {
				brush->solid = true;
			} else {
				brush->solid = false;
			}
		}

		// Go back to previous operation
		app->changeOperation(app->previousOperation);
	}

}

void PickOperation::end() {

}

