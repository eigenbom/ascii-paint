#include "resize_operation.h"

#include "libtcod.hpp"
#include "gui/gui.hpp"
#include "app.h"
#include "message_box.h"


ResizeOperation::ResizeOperation() {
}

ResizeOperation::~ResizeOperation() {
}

void ResizeOperation::start() {
	bool makeNew = true;
	if(makeNew) {
		MessageBox msgBox2("Resize", "New image dimensions", 2);
		// Add Width and Height sliders

		// I intentionally add a space after width so that it aligns nicely with
		// the height slider
		Slider widthSlider(0, 0, 4, 1.0f, 2048.0f, "width ", "New width of the image");
		widthSlider.setFormat("%.0f");
		widthSlider.setSensitivity(0.2);
		widthSlider.setValue(app->canvasWidth);

		Slider heightSlider(0, 0, 4, 1.0f, 2048.0f, "height", "New height of the image");
		heightSlider.setFormat("%.0f");
		heightSlider.setSensitivity(0.2);
		heightSlider.setValue(app->canvasHeight);

		msgBox2.addCustomWidget(&widthSlider);
		msgBox2.addCustomWidget(&heightSlider);

		msgBox2.show();

		if(msgBox2.getButtonPressed() == 1) { // OK
			// Resize
			app->resizeCanvas(atoi(widthSlider.getValue()),atoi(heightSlider.getValue()));
		}
	}

	app->changeOperation(app->previousOperation);
}

void ResizeOperation::update() {

}

void ResizeOperation::end() {

}


