#ifndef ASCIIPAINT_SMALLTOOLBAR_H
#define ASCIIPAINT_SMALLTOOLBAR_H

#include "gui/gui.hpp"

class SmallToolBar : public Container {
public :
	SmallToolBar(int x, int y, const char *name, const char *tip=NULL);
	SmallToolBar(int x, int y, int w, const char *name, const char *tip=NULL);
	~SmallToolBar();
	void render();
	void setName(const char *name);
	void addSeparator(const char *txt, const char *tip=NULL);
	void computeSize();
protected :
	char *name;
	int fixedWidth;
};



#endif
