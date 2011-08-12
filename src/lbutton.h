#ifndef ASCIIPAINT_LBUTTON_H
#define ASCIIPAINT_LBUTTON_H

#include "gui/gui.hpp"

// Renders a button aligned left
class LButton: public Button {
public:
	LButton(const char *label, const char *tip, widget_callback_t cbk, void *userData=NULL);
	LButton(int x, int y, int width, int height, const char *label, const char *tip, widget_callback_t cbk, void *userData=NULL);
	void render();
};

#endif
