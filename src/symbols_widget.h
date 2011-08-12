#ifndef ASCIIPAINT_SYMBOLS_WIDGET_H
#define ASCIIPAINT_SYMBOLS_WIDGET_H

#include "gui/gui.hpp"

/**
 * An ascii symbol selector widget.
 */
class SymbolsWidget: public Widget {
public:
	SymbolsWidget(int x, int y, widget_callback_t cbk=NULL,  void *userData=NULL);
	~SymbolsWidget();
	void render();
	int getSymbol(){return symbol;}
protected:
	void onButtonClick();
	int sx, sy; // selected x,y
	int symbol;
	widget_callback_t cbk;
	void* userData;
};

#endif
