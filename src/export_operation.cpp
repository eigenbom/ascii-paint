#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <gd.h> // libgd for gif export

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
	app->gui->exportGifButton->setVisible(true);
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

// TODO: Make palettised gifs to conserve bandwith
void doExportGifCbk(Widget *wid, void *data) {
	App *app = AppUser::app;
	MessageBox msgBox("Export Animated Gif", "Enter filename to export to", 2, true);

	if(!app->filename.empty()) {
		std::string baseFilename = getBaseFilename(app->filename.c_str());
		baseFilename.append(".gif");
		msgBox.setString(baseFilename.c_str());
	}

	// I intentionally add a space after width so that it aligns nicely with
	// the height slider
	Slider widthSlider(0, 0, 4, 1.0f, 1024.0f, "width ", "Width of each sprite");
	widthSlider.setFormat("%.0f");
	widthSlider.setSensitivity(0.2);
	widthSlider.setValue(app->gridW);

	Slider heightSlider(0, 0, 4, 1.0f, 1024.0f, "height", "Height of each sprite");
	heightSlider.setFormat("%.0f");
	heightSlider.setSensitivity(0.2);
	heightSlider.setValue(app->gridH);

	Slider numSlider(0, 0, 4, 1.0f, 1024.0f, "frames", "Number of frames (processed left-to-right, top-to-bottom)");
	numSlider.setFormat("%.0f");
	numSlider.setSensitivity(0.2);
	numSlider.setValue((app->canvasHeight/app->gridH) * (app->canvasWidth/app->gridW));

	Slider delaySlider(0, 0, 4, 1.0f, 1024.0f, "delay", "Delay between frames (ms)");
	delaySlider.setFormat("%.0f");
	delaySlider.setSensitivity(0.2);
	delaySlider.setValue(50);

	msgBox.addCustomWidget(&widthSlider);
	msgBox.addCustomWidget(&heightSlider);
	msgBox.addCustomWidget(&numSlider);
	msgBox.addCustomWidget(&delaySlider);

	msgBox.show();

	// If user presses ESCAPE or cancel
	if(msgBox.getButtonPressed() == 0 || msgBox.getButtonPressed() == 2) {
		return;
	}

	if(msgBox.getButtonPressed() == 1 && msgBox.getString() != NULL) {
		std::string exportFilename = msgBox.getString();

		// render entire canvas to an image
		TCODImage exportImg = TCODImage(app->canvasCon);
		int width, height;
		exportImg.getSize(&width,&height);

		// get dimension, number of frames, delay, etc...
		int fontWidth, fontHeight;
		TCODSystem::getCharSize(&fontWidth, &fontHeight);
		int spriteWidth = fontWidth*atoi(widthSlider.getValue()); // NB! in pixels
		int spriteHeight = fontHeight*atoi(heightSlider.getValue()); // NB! in pixels
		int spriteNum = atoi(numSlider.getValue());
		int spriteDelay = atoi(delaySlider.getValue());

		int i;
		FILE * out;

		gdImagePtr im;
		gdImagePtr prev = NULL;
		int white, black;

		im = gdImageCreateTrueColor(spriteWidth, spriteHeight);
		if (!im) {
			fprintf(stderr, "can't create image");
			return;
		}

		out = fopen(exportFilename.c_str(), "wb");
		if (!out) {
			fprintf(stderr, "can't create file %s", exportFilename.c_str());
			return;
		}

		// white = gdImageColorAllocate(im, 255, 255, 255);
		// black = gdImageColorAllocate(im, 0, 0, 0);
		gdImagePtr imP = gdImageCreatePaletteFromTrueColor (im, 0, 256);
		gdImagePtr prevP = gdImageCreatePaletteFromTrueColor (im, 0, 256);
		gdImageGifAnimBegin(imP, out, 1, 0);

		for(i = 0; i < spriteNum; i++) {
			// compute coordinates of sprite
			int unx = i*spriteWidth; // unnormalised x

			int dx = unx%width; // image coords
			int dy = spriteHeight * (unx/width); // image coords

			int r,g,b;
			im = gdImageCreateTrueColor(spriteWidth, spriteHeight);
			//white = gdImageColorAllocate(im, 255, 255, 255);
			//black = gdImageColorAllocate(im, 0, 0, 0);

			for(int x=dx;x<dx+spriteWidth;x++){
				for(int y=dy;y<dy+spriteHeight;y++){
					TCODColor col = exportImg.getPixel(x,y);
					// int c = gdImageColorAllocate(im,col.r,col.g,col.b);
					//gdImageSetPixel(im,x-dx,y-dy,x%2==0?
					//		gdTrueColor(255,255,255):gdTrueColor(0,0,0));
					gdImageSetPixel(im,x-dx,y-dy,gdTrueColor(col.r,col.g,col.b));
				}
			}

			prevP = imP;
			imP = gdImageCreatePaletteFromTrueColor (im, 0, 256);

			gdImageGifAnimAdd(imP, out, 1, 0, 0, spriteDelay, 1, prevP);
			if(prev) {
				gdImageDestroy(prev);
				gdImageDestroy(prevP);
			}
			prev = im;
		}

		gdImageGifAnimEnd(out);
		fclose(out);
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

