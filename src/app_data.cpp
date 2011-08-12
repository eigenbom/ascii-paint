#include <cstdio>
#include <cstring>
#include "app.h"
#include "app_data.h"

AppData::AppData() {
	filename = "ascii-paint.cfg";
}

AppData::~AppData() {
}

void AppData::load() {
	TCODParser parser;

	TCODParserStruct *windowTypeStruct = parser.newStructure("window");
	windowTypeStruct->addProperty("width", TCOD_TYPE_INT, true);
	windowTypeStruct->addProperty("height", TCOD_TYPE_INT, true);
	windowTypeStruct->addProperty("fullscreen", TCOD_TYPE_BOOL, true);
	windowTypeStruct->addProperty("background_color", TCOD_TYPE_COLOR, true);
	windowTypeStruct->addProperty("fps", TCOD_TYPE_INT, true);

	windowTypeStruct->addProperty("show_grid", TCOD_TYPE_BOOL, true);
	windowTypeStruct->addProperty("grid_width", TCOD_TYPE_INT, true);
	windowTypeStruct->addProperty("grid_height", TCOD_TYPE_INT, true);

	TCODParserStruct *canvasTypeStruct = parser.newStructure("canvas");
	canvasTypeStruct->addProperty("width", TCOD_TYPE_INT, true);
	canvasTypeStruct->addProperty("height", TCOD_TYPE_INT, true);
	
	TCODParserStruct *fontTypeStruct = parser.newStructure("font");
	fontTypeStruct->addProperty("file", TCOD_TYPE_STRING, true);
	fontTypeStruct->addProperty("layout", TCOD_TYPE_STRING, true);
	fontTypeStruct->addProperty("type", TCOD_TYPE_STRING, true);

	TCODParserStruct *brushesTypeStruct = parser.newStructure("brushes");
	brushesTypeStruct->addProperty("fore1", TCOD_TYPE_COLOR, true);
	brushesTypeStruct->addProperty("back1", TCOD_TYPE_COLOR, true);
	brushesTypeStruct->addProperty("symbol1", TCOD_TYPE_CHAR, true);
	brushesTypeStruct->addProperty("solid1", TCOD_TYPE_BOOL, true);
	brushesTypeStruct->addProperty("fore2", TCOD_TYPE_COLOR, true);
	brushesTypeStruct->addProperty("back2", TCOD_TYPE_COLOR, true);
	brushesTypeStruct->addProperty("symbol2", TCOD_TYPE_CHAR, true);
	brushesTypeStruct->addProperty("solid2", TCOD_TYPE_BOOL, true);
	
	TCODParserStruct *guiTypeStruct = parser.newStructure("gui");
	guiTypeStruct->addProperty("background_color1", TCOD_TYPE_COLOR, true);
	guiTypeStruct->addProperty("background_color2", TCOD_TYPE_COLOR, true);
	guiTypeStruct->addProperty("foreground_color1", TCOD_TYPE_COLOR, true);
	guiTypeStruct->addProperty("foreground_color2", TCOD_TYPE_COLOR, true);

	parser.run(filename.c_str(), NULL);

	app->windowWidth = parser.getIntProperty("window.width");
	app->windowHeight = parser.getIntProperty("window.height");
	app->fullscreen = parser.getBoolProperty("window.fullscreen");
	app->windowBackgroundColor = parser.getColorProperty("window.background_color");
	app->fpsGoal = parser.getIntProperty("window.fps");

	app->showGrid = parser.getBoolProperty("window.show_grid");
	app->gridW = parser.getIntProperty("window.grid_width");
	app->gridH = parser.getIntProperty("window.grid_height");

	app->fontFilename = parser.getStringProperty("font.file");
	if(!strcmp(parser.getStringProperty("font.layout"), "tcod")) {
		app->fontLayout = TCOD_FONT_LAYOUT_TCOD;
	} else if(!strcmp(parser.getStringProperty("font.layout"), "inrow")) {
		app->fontLayout = TCOD_FONT_LAYOUT_ASCII_INROW;
	} else {
		app->fontLayout = TCOD_FONT_LAYOUT_ASCII_INCOL;
	}
	
	if(!strcmp(parser.getStringProperty("font.type"), "greyscale")) {
		app->fontType = TCOD_FONT_TYPE_GREYSCALE;
	} else {
		app->fontType = 0;
	}

	app->canvasWidth = parser.getIntProperty("canvas.width");
	app->canvasHeight = parser.getIntProperty("canvas.height");

	app->initBrush1.fore = parser.getColorProperty("brushes.fore1");
	app->initBrush1.back = parser.getColorProperty("brushes.back1");
	app->initBrush1.symbol = parser.getCharProperty("brushes.symbol1");
	app->initBrush1.solid = parser.getBoolProperty("brushes.solid1");
	app->initBrush2.fore = parser.getColorProperty("brushes.fore2");
	app->initBrush2.back = parser.getColorProperty("brushes.back2");
	app->initBrush2.symbol = parser.getCharProperty("brushes.symbol2");
	app->initBrush2.solid = parser.getBoolProperty("brushes.solid2");

	app->gui->backgroundColor1 = parser.getColorProperty("gui.background_color1");
	app->gui->backgroundColor2 = parser.getColorProperty("gui.background_color2");
	app->gui->foregroundColor1 = parser.getColorProperty("gui.foreground_color1");
	app->gui->foregroundColor2 = parser.getColorProperty("gui.foreground_color2");
}

void AppData::save() {
	FILE *fp = fopen(filename.c_str(), "w");

	if(fp == NULL) {
		printf("Could not write config file.\n");
		return;
	}

	fprintf(fp, "// ASCII-Paint 0.3 / bp.io_fork config file\n");
	fprintf(fp, "window {\n"); {
		fprintf(fp, "\twidth = %i\n", app->windowWidth);
		fprintf(fp, "\theight = %i\n", app->windowHeight);
		fprintf(fp, "\tfullscreen = %s\n", app->fullscreen ? "true" : "false");
		fprintf(fp, "\tbackground_color = \"%i, %i, %i\"\n", app->windowBackgroundColor.r, app->windowBackgroundColor.g, app->windowBackgroundColor.b);
		fprintf(fp, "\tfps = %i\n\n", app->fpsGoal);

		fprintf(fp, "\tshow_grid = %s\n", app->showGrid? "true" : "false");
		fprintf(fp, "\tgrid_width = %i\n", app->gridW);
		fprintf(fp, "\tgrid_height = %i\n", app->gridH);

	} fprintf(fp, "}\n");

	fprintf(fp, "\n");

	fprintf(fp, "font {\n"); {
		fprintf(fp, "\tfile = \"%s\"\n", app->fontFilename.c_str());
		if(app->fontLayout == TCOD_FONT_LAYOUT_ASCII_INCOL)
			fprintf(fp, "\tlayout = \"%s\"\n", "incol");
		else if(app->fontLayout == TCOD_FONT_LAYOUT_ASCII_INROW)
			fprintf(fp, "\tlayout = \"%s\"\n", "inrow");
		else if(app->fontLayout == TCOD_FONT_LAYOUT_TCOD)
			fprintf(fp, "\tlayout = \"%s\"\n", "incol");
		
		if(app->fontType == TCOD_FONT_TYPE_GREYSCALE)
			fprintf(fp, "\ttype = \"%s\"\n", "greyscale");
		else
			fprintf(fp, "\ttype = \"%s\"\n", "normal");
		
	} fprintf(fp, "}\n");

	fprintf(fp, "\n");

	fprintf(fp, "canvas {\n"); {
		fprintf(fp, "\twidth = %i\n", app->canvasWidth);
		fprintf(fp, "\theight = %i\n", app->canvasHeight);
	} fprintf(fp, "}\n");

	fprintf(fp, "\n");

        fprintf(fp, "brushes {\n"); {
                fprintf(fp, "\tfore1 = \"%i, %i, %i\"\n", app->initBrush1.fore.r, app->initBrush1.fore.g, app->initBrush1.fore.b);
                fprintf(fp, "\tback1 = \"%i, %i, %i\"\n", app->initBrush1.back.r, app->initBrush1.back.g, app->initBrush1.back.b);
                fprintf(fp, "\tsymbol1 = '%c'\n", app->initBrush1.symbol);
                fprintf(fp, "\tsolid1 = %s\n", app->initBrush1.solid ? "true" : "false");
                fprintf(fp, "\tfore2 = \"%i, %i, %i\"\n", app->initBrush2.fore.r, app->initBrush2.fore.g, app->initBrush2.fore.b);
                fprintf(fp, "\tback2 = \"%i, %i, %i\"\n", app->initBrush2.back.r, app->initBrush2.back.g, app->initBrush2.back.b);
                fprintf(fp, "\tsymbol2 = '%c'\n", app->initBrush2.symbol);
                fprintf(fp, "\tsolid2 = %s\n", app->initBrush2.solid ? "true" : "false");
	} fprintf(fp, "}\n");

	fprintf(fp, "\n");

	fprintf(fp, "gui {\n"); {
		fprintf(fp, "\tbackground_color1 = \"%i, %i, %i\"\n", app->gui->backgroundColor1.r, app->gui->backgroundColor1.g, app->gui->backgroundColor1.b);
		fprintf(fp, "\tbackground_color2 = \"%i, %i, %i\"\n", app->gui->backgroundColor2.r, app->gui->backgroundColor2.g, app->gui->backgroundColor2.b);
		fprintf(fp, "\tforeground_color1 = \"%i, %i, %i\"\n", app->gui->foregroundColor1.r, app->gui->foregroundColor1.g, app->gui->foregroundColor1.b);
		fprintf(fp, "\tforeground_color2 = \"%i, %i, %i\"\n", app->gui->foregroundColor2.r, app->gui->foregroundColor2.g, app->gui->foregroundColor2.b);
	} fprintf(fp, "}\n");

	fclose(fp);
}

void AppData::setFilename(char *name) {
}
