#include "symbols_widget.h"

#include "libtcod.hpp"
#include "gui/gui.hpp"

SymbolsWidget::SymbolsWidget(int x, int y,  widget_callback_t _cbk, void* _userData)
:Widget(x,y,18,18)
,sx(3),sy(2),symbol(-1)
,cbk(_cbk),userData(_userData)
{}

SymbolsWidget::~SymbolsWidget(){

}

void SymbolsWidget::render(){
	// Draw a grid of ASCII characters
	con->setDefaultBackground(back);
	con->setDefaultForeground(fore);

	for(int dx = 0; dx < 16; dx++) {
		for(int dy = 0; dy < 16; dy++) {
			con->putChar(x + dx + 1, y + dy + 1, dx + dy*16);
		}
	}

	if (sx!=-1 and sy!=-1){
		// draw arrows on selection
		con->putChar(x,sy+y+1,TCOD_CHAR_ARROW_E);
		con->putChar(sx+x+1,y,TCOD_CHAR_ARROW_S);
		con->putChar(x+w-1,sy+y+1,TCOD_CHAR_ARROW_W);
		con->putChar(sx+x+1,y+h-1,TCOD_CHAR_ARROW_N);

		// and draw highlight bars
		for(int dx = 0; dx < w-2; dx++){
			con->setCharForeground(dx+x+1,sy+y+1,foreFocus);
		}
		for(int dy = 0; dy < h-2; dy++){
			con->setCharForeground(sx+x+1,dy+y+1,foreFocus);
		}
	}
}

void SymbolsWidget::onButtonClick(){
	 if (mouseIn) {
		int dx = mouse.cx - x - 1;
		int dy = mouse.cy - y - 1;
		if (dx>=0 and dy>=0) {
			sx = dx;
			sy = dy;
			symbol = dx + dy*16;

			if (cbk) cbk(this,userData);
		}
	 }
}
