#include "slider2.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "libtcod.hpp"
#include "gui/gui.hpp"

void Slider2::update(TCOD_key_t k) {
	float oldValue=value;
	TextBox::update(k);

	// BP: at this point, if the textbox contains nothing, then
	// we shouldnt try to extract the value, or call the callback
	bool emptyText = strlen(txt)==0;
	if (emptyText) {
		value = 0;
	}
	else {
		textToValue();
	}

	if ( mouse.cx >= x+w-2 && mouse.cx < x+w && mouse.cy == y ) onArrows=true;
	else onArrows=false;
	if ( drag ) {
		if ( dragy == -1 ) {
			dragx = mouse.x;
			dragy = mouse.y;
		} else {
			float mdx = (float)((mouse.x-dragx)*sensitivity) / (con->getWidth()*8);
			float mdy = (float)((mouse.y-dragy)*sensitivity) / (con->getHeight()*8);
			float oldValue=value;
			if ( fabs(mdy) > fabs(mdx) ) mdx=-mdy;
			value = dragValue+(max-min)*mdx;
			value=CLAMP(min,max,value);
			if ( value != oldValue ) {
				valueToText();
				textToValue();
			}
		}
	}
	if ( !emptyText && value != oldValue && cbk ) {
		cbk(this,value,data);
	}
}
