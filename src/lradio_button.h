#ifndef ASCIIPAINT_LRADIO_H
#define ASCIIPAINT_LRADIO_H

#include "gui/gui.hpp"
#include "lbutton.h"

class LRadioButton : public LButton {
public :
	LRadioButton(const char *label, const char *tip, widget_callback_t cbk, void *userData=NULL)
		: LButton(label,tip,cbk,userData),group(defaultGroup) {}
	LRadioButton(int x, int y, int width, int height, const char *label, const char *tip, widget_callback_t cbk, void *userData=NULL)
		: LButton(x,y,width,height,label,tip,cbk,userData),group(defaultGroup) {}

	void setGroup(int group) { this->group=group; }
	void render();
	void select();
	void unSelect();
	static void unSelectGroup(int group);
	static void setDefaultGroup(int group) { defaultGroup=group; }
protected :
	static int defaultGroup;
	int group;
	static LRadioButton *groupSelect[512];

	void onButtonClick();
};


#endif
