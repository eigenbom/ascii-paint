#ifndef ASCIIPAINT_COLOUR_WIDGET_H
#define ASCIIPAINT_COLOUR_WIDGET_H

#include "libtcod.hpp"
#include "gui/gui.hpp"
#include "palette.h"

class ColourWidget: public Widget {
public:
	ColourWidget(int x, int y, widget_callback_t cbk=NULL,  void *userData=NULL);
	~ColourWidget();

	void setPaletteCallback(widget_callback_t cbk, void* userData=NULL);
	void setPalette(Palette* p);

	void render();
	void update(const TCOD_key_t k);

	TCODColor getForeColour(){return sfore;}
	TCODColor getBackColour(){return sback;}
	int getPaletteChangeDirection(){return pChange;}

protected:

	void onButtonClick();
	void onRButtonClick();

	bool isNormalColourLocation(int x, int y);
	bool isSpecialColourLocation(int x, int y);
	bool isSwapColourLocation(int x, int y);
	bool isTransparentColourLocation(int x, int y);
	bool isChooserColourLocation(int x, int y);

	bool doPaletteSelection(int x, int y); // returns true if did it
	int isPaletteSelectorLocation(int x, int y); // returns 0 if not, -1 if go back, +1 if go forward

	/// PRE: x,y are in widget coords and are VALID
	TCODColor getColourAt(int x, int y);

	int sfi, sbi; // selected fore and back indexes
	TCODColor sfore, sback;
	// bool sfores, sbacks; // is the color solid?
	widget_callback_t cbk;
	void* userData;

	int fx, fy;
	int bx, by;

	Palette* palette;

	widget_callback_t pcbk;
	void* puserData;
	int pChange; // palette change direction

	// Color chooser stuff (ripped from color_box.h)
	int satRows;
	int satCols;
	int hueCols;
	int valRows;

	TCODColor *colorToChange;

	void setColorToChange(TCODColor *col);
	void drawColorGrid(TCODConsole *con, float saturation);
	void showChooser();
};

#endif
