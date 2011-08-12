#include "lradio_button.h"
#include "lbutton.h"

#include "libtcod.hpp"
#include "gui/gui.hpp"

LRadioButton *LRadioButton::groupSelect[512];
int LRadioButton::defaultGroup=0;
static bool init=false;

void LRadioButton::select() {
	if (!init) {
		memset(groupSelect,0,sizeof(LRadioButton *)*512);
		init=true;
	}
	groupSelect[group]=this;
}

void LRadioButton::unSelect() {
	if (!init) {
		memset(groupSelect,0,sizeof(LRadioButton *)*512);
		init=true;
	}
	groupSelect[group]=NULL;
}

void LRadioButton::unSelectGroup(int group) {
	groupSelect[group]=NULL;
}

void LRadioButton::render() {
	LButton::render();
	if ( groupSelect[group] == this ) {
		con->setDefaultForeground(TCODColor::white);
		con->setChar(x,y,'>');
	}
}

void LRadioButton::onButtonClick() {
	select();
	LButton::onButtonClick();
}

