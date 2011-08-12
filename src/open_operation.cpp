#include <cstdio>
#include "libtcod.hpp"
#include "gui/gui.hpp"
#include "app.h"
#include "message_box.h"
#include "open_operation.h"

OpenOperation::OpenOperation() {
}

OpenOperation::~OpenOperation() {
}

void OpenOperation::start() {
	FILE *fp;

	MessageBox msgBox("Open", "Enter filename to open", 2, true);

	msgBox.show();

	if(msgBox.getButtonPressed() == 1 && msgBox.getString() != NULL) {
		app->filename = msgBox.getString();

		fp = fopen(app->filename.c_str(), "r");
		if(fp == NULL) {
			printf("The file %s could not be opened.\n", app->filename.c_str());
			MessageBox msgBox2("Error", "The file could not be opened", 1);
			msgBox2.show();
		} else {
			if(app->filename.at(app->filename.length() - 1) == 'z') {
				fclose(fp); // Don't need fp anymore

				TCODZip zipFile;
				zipFile.loadFromFile(app->filename.c_str());
				
				const char *fileHeader = new char[100];
				fileHeader = zipFile.getString();
				
				float version;
				sscanf(fileHeader, "ASCII-Paint v%g", &version);
				
				app->canvasWidth = zipFile.getInt();
				app->canvasHeight = zipFile.getInt();
				
				app->initCanvas();

				CanvasImage *img = new CanvasImage;

				for(int x = 0; x < app->canvasWidth; x++) {
					for(int y = 0; y < app->canvasHeight; y++) {
						Brush b;
						b.symbol = zipFile.getChar();
						b.fore.r = zipFile.getChar();
						b.fore.g = zipFile.getChar();
						b.fore.b = zipFile.getChar();
						b.back.r = zipFile.getChar();
						b.back.g = zipFile.getChar();
						b.back.b = zipFile.getChar();
						// Solid and walkable added in version 0.3
						if(version >= 0.3f) {
							b.solid = zipFile.getChar();
							b.walkable = zipFile.getChar();
						} else {
							// For older maps set the default solidness
							b.solid = false;
							b.walkable = true;
						}
							
						img->push_back(b);
					}
				}

				app->setCanvasImage(*img);
			} else {
				float version;
				fscanf(fp, "ASCII-Paint v%g", &version);

				// Check if we have the required version of ASCII paint to open the file
				if(version > app->mapVersion) {
					printf("The file was made with version %g whereas we are running version %g\n",
							version, app->mapVersion);
					return;
				}

				fscanf(fp, "%i %i", &app->canvasWidth, &app->canvasHeight);

				app->initCanvas();

				CanvasImage *img = new CanvasImage;

				// Scan until we hit #(the marker for the start of image data)
				while(fgetc(fp) != '#');

				for(int x = 0; x < app->canvasWidth; x++) {
					for(int y = 0; y < app->canvasHeight; y++) {
						Brush b;
						b.symbol = fgetc(fp);
						b.fore.r = fgetc(fp);
						b.fore.g = fgetc(fp);
						b.fore.b = fgetc(fp);
						b.back.r = fgetc(fp);
						b.back.g = fgetc(fp);
						b.back.b = fgetc(fp);
						// Solid and walkable added in version 0.3
						if(version >= 0.3f) {
							b.solid = fgetc(fp);
							b.walkable = fgetc(fp);
						} else {
							// For older maps set the default solidness
							b.solid = false;
							b.walkable = true;
						}
							
						img->push_back(b);
					}
				}

				app->setCanvasImage(*img);
			}
		}
	}

	app->changeOperation(app->previousOperation);
}

void OpenOperation::update() {

}

void OpenOperation::end() {

}




