#ifndef ASCIIPAINT_SLIDER2_H
#define ASCIIPAINT_SLIDER2_H

#include "libtcod.hpp"
#include "gui/gui.hpp"

/**
 * This slider fixes the bug in Slider where the textbox has to contain
 * a value. sometimes when editing we want to delete everything.
 */
class Slider2: public Slider {
public:
	Slider2(int x,int y,int w, float min, float max, const char *label, const char *tip=NULL):
		Slider(x,y,w,min,max,label,tip){}
	void update(TCOD_key_t k);
};

#endif
