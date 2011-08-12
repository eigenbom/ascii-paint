#include "lbutton.h"

LButton::LButton(const char *label, const char *tip, widget_callback_t cbk, void *userData)
:Button(label,tip,cbk,userData){}

LButton::LButton(int x, int y, int width, int height, const char *label, const char *tip, widget_callback_t cbk, void *userData)
:Button(x,y,width,height,label,tip,cbk,userData){}

void LButton::render(){
	con->setDefaultBackground(mouseIn ? backFocus : back);
	con->setDefaultForeground(mouseIn ? foreFocus : fore);
	if ( w > 0 && h > 0 ) con->rect(x,y,w,h,true,TCOD_BKGND_SET);
	if ( label ) {
		if ( pressed && mouseIn ) {
			con->printEx(x,y,TCOD_BKGND_NONE,TCOD_LEFT,"-%s-",label);
		} else {
			con->printEx(x,y,TCOD_BKGND_NONE,TCOD_LEFT," %s ",label);
		}
	}
}
