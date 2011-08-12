#ifndef ASCIIPAINT_COLOUR_WIDGET_H
#define ASCIIPAINT_COLOUR_WIDGET_H

#include "libtcod.hpp"
#include "gui/gui.hpp"

class ColourWidget: public Widget {
public:
	ColourWidget(int x, int y, widget_callback_t cbk=NULL,  void *userData=NULL);
	~ColourWidget();
	void render();
	TCODColor getForeColour(){return sfore;}
	TCODColor getBackColour(){return sback;}
	void update(const TCOD_key_t k);
protected:
	void onButtonClick();
	void onRButtonClick();
	int sfi, sbi; // selected fore and back indexes
	TCODColor sfore, sback;
	// bool sfores, sbacks; // is the color solid?
	widget_callback_t cbk;
	void* userData;
};

#endif
