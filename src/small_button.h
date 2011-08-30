#ifndef ASCIIPAINT_SMALL_BUTTON_H
#define ASCIIPAINT_SMALL_BUTTON_H

#include "gui/gui.hpp"
#include "libtcod.hpp"

// Like a button, but smaller!
// Hover over just changes colour
// Also can specify a symbol to use instead of a label
class SmallButton : public Widget {
public :
	SmallButton(const char *label, const char *tip, widget_callback_t cbk, void *userData=NULL);
	SmallButton(int labelSymbol, const char *tip, widget_callback_t cbk, void *userData=NULL);
	SmallButton(int x, int y, int width, int height, const char *label, const char *tip, widget_callback_t cbk, void *userData=NULL);
	virtual ~SmallButton();
	void render();
	void setLabel(const char *newLabel);
	void computeSize();
	inline bool isPressed() { return pressed; }
protected :
	bool pressed;

	// one or the other...
	char *label;
	int labelSymbol;

	widget_callback_t cbk;

	void onButtonPress();
	void onButtonRelease();
	void onButtonClick();
	void expand(int width, int height);
};

#endif
