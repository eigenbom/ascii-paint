#include "colour_widget.h"

#include "symbols_widget.h"

#include "libtcod.hpp"
#include "gui/gui.hpp"

static const int NUM_COLS = 16;
static TCODColor cols[NUM_COLS] = {
		TCODColor::red,
		TCODColor::darkerRed,
		TCODColor::green,
		TCODColor::darkerGreen,
		TCODColor::blue,
		TCODColor::darkerBlue,
		TCODColor::cyan,
		TCODColor::darkerCyan,
		TCODColor::yellow,
		TCODColor::darkerYellow,
		TCODColor::magenta,
		TCODColor::darkerMagenta,
		TCODColor::black,
		TCODColor::darkGrey,
		TCODColor::lightGrey,
		TCODColor::white
};

ColourWidget::ColourWidget(int x, int y,  widget_callback_t _cbk, void* _userData)
:Widget(x,y,1+NUM_COLS,3)
,sfi(NUM_COLS),sbi(NUM_COLS-3),sfore(),sback()
,cbk(_cbk),userData(_userData)
{}

ColourWidget::~ColourWidget(){

}

void ColourWidget::render(){
	// Draw a grid of colours in the palette
	con->setDefaultBackground(back);
	con->setDefaultForeground(fore);

	con->putChar(x,y+1,'T'); // transparent (i.e., pink)
	for(int i = 0; i < NUM_COLS; i++) {
		con->putCharEx(x+i+1, y+1, 219, cols[i], back);
	}

	// draw down and up arrows on selection
	con->putChar(x+sfi,y,TCOD_CHAR_ARROW_S);
	con->putChar(x+sbi,y+2,TCOD_CHAR_ARROW_N);
}

void ColourWidget::update(const TCOD_key_t k){
	Widget::update(k);

	// detect right click
	if (mouseIn){
		if (mouse.rbutton) onRButtonClick();
	}
}

void ColourWidget::onButtonClick(){
	 if (mouseIn) {
		int dx = mouse.cx - x;
		int dy = mouse.cy - y;
		if (dy==1){
			sfi = dx;
			if (dx==0)
				sfore = TCODColor(255,0,255);
			else
				sfore = cols[sfi-1];
			// back = TCODColor::black;
			if (cbk) cbk(this,userData);
		}
	 }
}

void ColourWidget::onRButtonClick(){
	 if (mouseIn) {
		int dx = mouse.cx - x;
		int dy = mouse.cy - y;
		if (dy==1){
			sbi = dx;
			if (dx==0)
				sback = TCODColor(255,0,255);
			else
				sback = cols[sbi-1];
			// back = TCODColor::black;
			if (cbk) cbk(this,userData);
		}
	 }
}
