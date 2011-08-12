#include <cstdio>
#include <cstring>
#include "libtcod.hpp"
#include "gui/gui.hpp"
#include "app.h"
#include "message_box.h"
#include "save_operation.h"

SaveOperation::SaveOperation() {
}

SaveOperation::~SaveOperation() {
}

void SaveOperation::start() {
	doSave(app);
	app->changeOperation(app->previousOperation);
}

void SaveOperation::update() {

}

void SaveOperation::end() {

}

bool doSave(App *app) {
	FILE *fp;

	MessageBox msgBox("Save", "Enter filename to save to", 2, true);

	if(!app->filename.empty())
		msgBox.setString(app->filename.c_str());

	msgBox.show();

	// If user presses ESCAPE or cancel
	if(msgBox.getButtonPressed() == 0 || msgBox.getButtonPressed() == 2) {
		return false;
	}

	if(msgBox.getButtonPressed() == 1 && msgBox.getString() != NULL) {
		app->filename = msgBox.getString();

		fp = fopen(app->filename.c_str(), "w");
		if(fp == NULL) {
			printf("The file %s could not be saved.\n", app->filename.c_str());
			MessageBox msgBox2("Error", "The file could not be saved", 1);
			msgBox2.show();
			return false;
		} else {

			// If the filename ends with a z (for example test.ascz) then use the compressed format
			if(app->filename.at(app->filename.length() - 1) == 'z') {
				fclose(fp); // Don't need fp anymore

				TCODZip zipFile;

				char *fileHeader = new char[100];
				sprintf(fileHeader, "ASCII-Paint v%g\n", app->mapVersion);

				zipFile.putString(fileHeader);
				zipFile.putInt(app->canvasWidth);
				zipFile.putInt(app->canvasHeight);

				// Get image from canvas
				CanvasImage *img = app->getCanvasImage();

				// Write the brush data for every brush in the image
				for(int x = 0; x < app->canvasWidth; x++) {
					for(int y = 0; y < app->canvasHeight; y++) {
						Brush b = (*img)[x * app->canvasHeight + y];
						zipFile.putChar(b.symbol);
						zipFile.putChar(b.fore.r);
						zipFile.putChar(b.fore.g);
						zipFile.putChar(b.fore.b);
						zipFile.putChar(b.back.r);
						zipFile.putChar(b.back.g);
						zipFile.putChar(b.back.b);
						zipFile.putChar(b.solid);
						zipFile.putChar(b.walkable);
					}
				}

				zipFile.saveToFile(app->filename.c_str());
			} else {

				fprintf(fp, "ASCII-Paint v%g\n", app->mapVersion);
				fprintf(fp, "%i %i\n", app->canvasWidth, app->canvasHeight);

				// Use the # character as a marker for the start of image data
				fputc('#', fp);

				// Get image from canvas
				CanvasImage *img = app->getCanvasImage();

				// Write the brush data for every brush in the image
				for(int x = 0; x < app->canvasWidth; x++) {
					for(int y = 0; y < app->canvasHeight; y++) {
						Brush b = (*img)[x * app->canvasHeight + y];
						fputc(b.symbol, fp);
						fputc(b.fore.r, fp);
						fputc(b.fore.g, fp);
						fputc(b.fore.b, fp);
						fputc(b.back.r, fp);
						fputc(b.back.g, fp);
						fputc(b.back.b, fp);
						fputc(b.solid, fp);
						fputc(b.walkable, fp);
					}
				}

				fclose(fp);
			}

			app->canvasModified = false;
		}
	}
	return true;
}

