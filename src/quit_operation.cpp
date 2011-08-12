#include "libtcod.hpp"
#include "gui/gui.hpp"
#include "app.h"
#include "message_box.h"
#include "quit_operation.h"
#include "save_operation.h"

QuitOperation::QuitOperation() {
}

QuitOperation::~QuitOperation() {
}

void QuitOperation::start() {
	if(app->canvasModified) {
		MessageBox msgBox("New", "Save changes before quitting?", 3);
		msgBox.show();

		int choice = msgBox.getButtonPressed();

		if(choice == 1) { // Yes
			if(doSave(app) == true) {
				app->quit = true;
			}
		}

		if(choice == 2) { // User pressed Yes or No
			app->quit = true;
		}

	} else {
		app->quit = true;
	}
	app->changeOperation(app->previousOperation); 
}

void QuitOperation::update() {

}

void QuitOperation::end() {

}



