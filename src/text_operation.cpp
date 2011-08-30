#include "libtcod.hpp"
#include "gui/gui.hpp"
#include "app.h"
#include "brush.h"
#include "text_operation.h"

TextOperation::TextOperation() {
}

TextOperation::~TextOperation() {
}

void TextOperation::start() {
	text.clear();
	positionSelected = false;
	brush = &app->brush1;

	app->gui->useForegroundToggleButton->setVisible(true);
	app->gui->useBackgroundToggleButton->setVisible(true);
	app->gui->useSolidToggleButton->setVisible(true);

	app->inTypingMode = true;
}

void TextOperation::update() {
	TCOD_mouse_t mouse = app->mouse;
	int mouseX = app->canvasMouseX;
	int mouseY = app->canvasMouseY;

	if(mouse.lbutton /*BP|| mouse.rbutton */) {
		// If pressing the left mouse button use brush1 otherwise brush2
		mouse.lbutton == true ? brush = &app->brush1 : brush = &app->brush2;
		positionSelected = true;
		textX = mouseX;
		textY = mouseY;
	}

	// When the user hasn't selected the cell to write on, follow the mouse
	if(!positionSelected) {
		textX = mouseX;
		textY = mouseY;
	}

	if(app->key.vk == TCODK_ENTER) {
		// First make a backup for undo
		app->addUndo();
		// If user presses enter then blit the overlay onto the canvas
		app->applyOverlayToCanvas();
		//TCODConsole::blit(app->overlayCon, 0, 0, app->canvasWidth, app->canvasHeight, app->canvasCon, 0, 0, 255);

	} else if(app->key.vk == TCODK_BACKSPACE) {
		// If the user presses backspace then delete last character
		if(!text.empty())
			text.pop_back();

	} else if(app->key.c != 0) {
		// If user presses a printable character then add it to our text
		text.push_back(app->key.c);
	}

	app->clearOverlay();

	if(!text.empty()) {
		// Draw text
		app->overlayCon->setDefaultBackground(brush->back);
		app->overlayCon->setDefaultForeground(brush->fore);
		for(unsigned int i = 0; i < text.size(); i++) {
			app->overlayCon->setChar(textX + i, textY, text[i]);
			// Yes this bit is pretty messy
			// Basically it is a modified form of App::applyBrushToCell
			// except that we are not concerned about the symbol
			if(app->gui->useForegroundToggleButton->isPressed())
				app->overlayCon->setCharForeground(textX + i, textY, brush->fore);
			else
				app->overlayCon->setCharForeground(textX + i, textY, app->canvasCon->getCharForeground(textX + i, textY));

			if(app->gui->useBackgroundToggleButton->isPressed())
				app->overlayCon->setCharBackground(textX + i, textY, brush->back);
			else
				app->overlayCon->setCharBackground(textX + i, textY, app->canvasCon->getCharBackground(textX + i, textY));

			/*
			if(app->gui->useSolidToggleButton->isPressed()) {
				if(brush->solid)
					app->solidOverlayCon->setCharBackground(textX + i, textY, TCODColor(0, 0, 255));
				else
					app->solidOverlayCon->setCharBackground(textX + i, textY, TCODColor(255, 255, 255));
			}
			*/
		}
	}
}

void TextOperation::end() {
	app->inTypingMode = false;
}
