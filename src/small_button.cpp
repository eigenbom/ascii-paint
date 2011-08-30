#include <cstring>
#include "libtcod.hpp"
#include "gui/gui.hpp"
#include "small_button.h"

SmallButton::SmallButton(const char *label,const char *tip,widget_callback_t cbk, void *userData)
	: pressed(false),label(NULL) {
	if ( label != NULL ) {
		setLabel(label);
	}
	if ( tip != NULL ) setTip(tip);
	this->x=0;
	this->y=0;
	this->userData=userData;
	this->cbk=cbk;
}

SmallButton::SmallButton(int labelSymbol,const char *tip,widget_callback_t cbk, void *userData)
	: pressed(false),label(NULL) {
	this->labelSymbol = labelSymbol;
	if ( tip != NULL ) setTip(tip);
	this->x=0;
	this->y=0;
	this->userData=userData;
	this->cbk=cbk;
}

SmallButton::SmallButton(int x,int y,int width, int height,const char *label,const char *tip,widget_callback_t cbk, void *userData)
	: pressed(false), label(NULL) {
	if ( label != NULL ) setLabel(label);
	if ( tip != NULL ) setTip(tip);
	w=width;
	h=height;
	this->x=x;
	this->y=y;
	this->userData=userData;
	this->cbk=cbk;
}

SmallButton::~SmallButton() {
	if ( label ) free(label);
}

void SmallButton::setLabel(const char *newLabel) {
	if ( label ) free(label);
	label=TCOD_strdup(newLabel);
}

void SmallButton::render() {
	con->setDefaultBackground(mouseIn ? backFocus : back);
	con->setDefaultForeground(mouseIn ? foreFocus : fore);
	if ( w > 0 && h > 0 ) con->rect(x,y,w,h,true,TCOD_BKGND_SET);
	if ( label ) {
		con->printEx(x+w/2,y,TCOD_BKGND_NONE,TCOD_CENTER,label);
	}
	else {
		con->putCharEx(x+w/2,y,labelSymbol,mouseIn ? foreFocus : fore,mouseIn ? backFocus : back);
	}
}

void SmallButton::computeSize() {
	if ( label != NULL ) {
		w=strlen(label);
	} else {
		w=1;
	}
	h=1;
}

void SmallButton::expand(int width, int height) {
	if ( w < width ) w = width;
}

void SmallButton::onButtonPress() {
	pressed=true;
}

void SmallButton::onButtonRelease() {
	pressed=false;
}

void SmallButton::onButtonClick() {
	if ( cbk ) cbk(this,userData);
}



