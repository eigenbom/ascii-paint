#include <cstdio>
#include <cstring>
#include "libtcod.hpp"
#include "gui/gui.hpp"
#include "app.h"
#include "message_box.h"
#include "misc.h"
#include "export_operation.h"

ExportOperation::ExportOperation() {
}

ExportOperation::~ExportOperation() {
}

void ExportOperation::start() {
	app->gui->exportPngButton->setVisible(true);
	app->gui->exportBmpButton->setVisible(true);
	app->gui->exportTextButton->setVisible(true);
	app->gui->exportForeButton->setVisible(true);
	app->gui->exportBackButton->setVisible(true);
}

void ExportOperation::update() {
}

void ExportOperation::end() {
}

void doExportPngCbk(Widget *wid, void *data) {
	App *app = AppUser::app;
	MessageBox msgBox("Export Png", "Enter filename to export to", 2, true);

	if(!app->filename.empty()) {
		std::string baseFilename = getBaseFilename(app->filename.c_str());
		baseFilename.append(".png");
		msgBox.setString(baseFilename.c_str());
	}

	msgBox.show();

	// If user presses ESCAPE or cancel
	if(msgBox.getButtonPressed() == 0 || msgBox.getButtonPressed() == 2) {
		return;
	}

	if(msgBox.getButtonPressed() == 1 && msgBox.getString() != NULL) {
		std::string exportFilename = msgBox.getString();

		TCODImage exportImg = TCODImage(app->canvasCon);
		exportImg.save(exportFilename.c_str());
	}

}

void doExportBmpCbk(Widget *wid, void *data) {
	App *app = AppUser::app;
	MessageBox msgBox("Export Bmp", "Enter filename to export to", 2, true);

	if(!app->filename.empty()) {
		std::string baseFilename = getBaseFilename(app->filename.c_str());
		baseFilename.append(".bmp");
		msgBox.setString(baseFilename.c_str());
	}

	msgBox.show();

	// If user presses ESCAPE or cancel
	if(msgBox.getButtonPressed() == 0 || msgBox.getButtonPressed() == 2) {
		return;
	}

	if(msgBox.getButtonPressed() == 1 && msgBox.getString() != NULL) {
		std::string exportFilename = msgBox.getString();

		TCODImage exportImg = TCODImage(app->canvasCon);
		exportImg.save(exportFilename.c_str());
	}

}

void doExportTextCbk(Widget *wid, void *data) {
	App *app = AppUser::app;
	MessageBox msgBox("Export Text", "Enter filename to export to", 2, true);

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
		std::string exportFilename = msgBox.getString();

		FILE *fp = fopen(exportFilename.c_str(), "w");

		if(fp == NULL) {
			printf("The file %s could not be saved.\n", app->filename.c_str());
			MessageBox msgBox2("Error", "The file could not be saved", 1);
			msgBox2.show();
			return;
		}
		
		// Get image from canvas
		CanvasImage *img = app->getCanvasImage();

		for(int y = 0; y < app->canvasHeight; y++) {
			for(int x = 0; x < app->canvasWidth; x++) {
				Brush b = (*img)[x * app->canvasHeight + y];
				fprintf(fp, "%c", b.symbol);
			}
			fprintf(fp, "\n");
		}
		fclose(fp);
	}

}

void doExportForeCbk(Widget *wid, void *data) {
	App *app = AppUser::app;
	MessageBox msgBox("Export Foreground Png", "Enter filename to export to", 2, true);

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
		std::string exportFilename = msgBox.getString();

		TCODImage exportImg = TCODImage(app->canvasWidth, app->canvasHeight);
		
		// Get image from canvas
		CanvasImage *img = app->getCanvasImage();

		for(int y = 0; y < app->canvasHeight; y++) {
			for(int x = 0; x < app->canvasWidth; x++) {
				Brush b = (*img)[x * app->canvasHeight + y];
				exportImg.putPixel(x, y, b.fore);
			}
		}
		exportImg.save(exportFilename.c_str());
	}

}

void doExportBackCbk(Widget *wid, void *data) {
	App *app = AppUser::app;
	MessageBox msgBox("Export Background Png", "Enter filename to export to", 2, true);

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
		std::string exportFilename = msgBox.getString();

		TCODImage exportImg = TCODImage(app->canvasWidth, app->canvasHeight);
		
		// Get image from canvas
		CanvasImage *img = app->getCanvasImage();

		for(int y = 0; y < app->canvasHeight; y++) {
			for(int x = 0; x < app->canvasWidth; x++) {
				Brush b = (*img)[x * app->canvasHeight + y];
				exportImg.putPixel(x, y, b.back);
			}
		}
		exportImg.save(exportFilename.c_str());
	}

}

