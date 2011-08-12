#include "libtcod.hpp"
#include "gui/gui.hpp"
#include "app.h"
#include "message_box.h"
#include "new_operation.h"
#include "save_operation.h"

NewOperation::NewOperation() {
}

NewOperation::~NewOperation() {
}

void NewOperation::start() {
	bool makeNew = true;
	if(app->canvasModified) {
		MessageBox msgBox("New", "Save changes to file?", 3);
		msgBox.show();

		int choice = msgBox.getButtonPressed();

		if(choice == 1) { // Yes
			doSave(app);
		}
		if(choice == 0 || choice == 3) // User pressed Cancel or escape
			makeNew = false;
	}

	if(makeNew) {
		MessageBox msgBox2("New", "New image properties", 2);
		// Add Width and Height sliders

		// I intentionally add a space after width so that it aligns nicely with
		// the height slider
		Slider widthSlider(0, 0, 4, 1.0f, 1024.0f, "width ", "Width of the new image");
		widthSlider.setFormat("%.0f");
		widthSlider.setSensitivity(0.2);
		widthSlider.setValue(app->canvasWidth);

		Slider heightSlider(0, 0, 4, 1.0f, 1024.0f, "height", "Height of the new image");
		heightSlider.setFormat("%.0f");
		heightSlider.setSensitivity(0.2);
		heightSlider.setValue(app->canvasHeight);

		msgBox2.addCustomWidget(&widthSlider);
		msgBox2.addCustomWidget(&heightSlider);

		msgBox2.show();

		if(msgBox2.getButtonPressed() == 1) { // OK
			// Make a new canvas
			app->canvasWidth = atoi(widthSlider.getValue());
			app->canvasHeight = atoi(heightSlider.getValue());
			app->initCanvas();
		}
	}

	app->changeOperation(app->previousOperation);
}

void NewOperation::update() {

}

void NewOperation::end() {

}


