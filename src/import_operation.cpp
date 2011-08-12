#include <cstdio>
#include <cstring>
#include "libtcod.hpp"
#include "gui/gui.hpp"
#include "app.h"
#include "message_box.h"
#include "misc.h"
#include "import_operation.h"

ImportOperation::ImportOperation() {
}

ImportOperation::~ImportOperation() {
}

void ImportOperation::start() {
	app->gui->importTextButton->setVisible(true);
	app->gui->importForeButton->setVisible(true);
	app->gui->importBackButton->setVisible(true);
}

void ImportOperation::update() {
}

void ImportOperation::end() {
}

void doImportTextCbk(Widget *wid, void *data) {
	App *app = AppUser::app;
	MessageBox msgBox("Import Text", "Enter filename to import from", 2, true);

	if(!app->filename.empty()) {
		std::string baseFilename = getBaseFilename(app->filename.c_str());
		baseFilename.append(".txt");
		msgBox.setString(baseFilename.c_str());
	}

	msgBox.show();

	// If user presses ESCAPE or cancel
	if(msgBox.getButtonPressed() == 0 || msgBox.getButtonPressed() == 2) {
		return;
	}

	if(msgBox.getButtonPressed() == 1 && msgBox.getString() != NULL) {
		std::string importFilename = msgBox.getString();

		FILE *fp = fopen(importFilename.c_str(), "r");

		// Check if file exists
		if(fp == NULL) {
			printf("The file %s could not be opened.\n", app->filename.c_str());
			MessageBox msgBox2("Error", "The file could not be opened", 1);
			msgBox2.show();
			return;
		}

		app->addUndo();

		int x = 0;
		int y = 0;
		for(int i = 0; ; i++) {
			char c = getc(fp);

			// If we reach the end of the file then exit the loop
			if(c == EOF || y >= app->canvasHeight) {
				y = app->canvasHeight;
				x = app->canvasWidth;
				break;
			}

			if(c == '\n') {
				y++;
				x = 0;
				continue;
			}

			if(x < app->canvasWidth) {
				app->canvasCon->setChar(x, y, c);
			}

			x++;
		}
		fclose(fp);
	}

}

void doImportForeCbk(Widget *wid, void *data) {
	App *app = AppUser::app;
	MessageBox msgBox("Import Foreground Png", "Enter filename to import from", 2, true);

	if(!app->filename.empty()) {
		std::string baseFilename = getBaseFilename(app->filename.c_str());
		baseFilename.append("-fore.png");
		msgBox.setString(baseFilename.c_str());
	}

	msgBox.show();

	// If user presses ESCAPE or cancel
	if(msgBox.getButtonPressed() == 0 || msgBox.getButtonPressed() == 2) {
		return;
	}

	if(msgBox.getButtonPressed() == 1 && msgBox.getString() != NULL) {
		std::string importFilename = msgBox.getString();

		FILE *fp = fopen(importFilename.c_str(), "r");

		// Check if file exists
		if(fp == NULL) {
			printf("The file %s could not be opened.\n", app->filename.c_str());
			MessageBox msgBox2("Error", "The file could not be opened", 1);
			msgBox2.show();
			return;
		}
		fclose(fp);

		TCODImage importImg = TCODImage(importFilename.c_str());

		int w, h;
		importImg.getSize(&w, &h);
		if(w != app->canvasWidth || h != app->canvasHeight) {
			MessageBox msgBox("Error", "The file does not have the same dimensions as the current image", 1);
			msgBox.show();
			return;
		}

		app->addUndo();

		for(int y = 0; y < app->canvasHeight; y++) {
			for(int x = 0; x < app->canvasWidth; x++) {
				app->canvasCon->setCharForeground(x, y, importImg.getPixel(x, y));
			}
		}
	}

}

void doImportBackCbk(Widget *wid, void *data) {
	App *app = AppUser::app;
	MessageBox msgBox("Import Background Png", "Enter filename to import from", 2, true);

	if(!app->filename.empty()) {
		std::string baseFilename = getBaseFilename(app->filename.c_str());
		baseFilename.append("-back.png");
		msgBox.setString(baseFilename.c_str());
	}

	msgBox.show();

	// If user presses ESCAPE or cancel
	if(msgBox.getButtonPressed() == 0 || msgBox.getButtonPressed() == 2) {
		return;
	}

	if(msgBox.getButtonPressed() == 1 && msgBox.getString() != NULL) {
		std::string importFilename = msgBox.getString();

		FILE *fp = fopen(importFilename.c_str(), "r");

		// Check if file exists
		if(fp == NULL) {
			printf("The file %s could not be opened.\n", app->filename.c_str());
			MessageBox msgBox2("Error", "The file could not be opened", 1);
			msgBox2.show();
			return;
		}
		fclose(fp);

		TCODImage importImg = TCODImage(importFilename.c_str());

		int w, h;
		importImg.getSize(&w, &h);
		if(w != app->canvasWidth || h != app->canvasHeight) {
			MessageBox msgBox("Error", "The file does not have the same dimensions as the current image", 1);
			msgBox.show();
			return;
		}

		app->addUndo();

		for(int y = 0; y < app->canvasHeight; y++) {
			for(int x = 0; x < app->canvasWidth; x++) {
				app->canvasCon->setCharBackground(x, y, importImg.getPixel(x, y));
			}
		}
	}

}


