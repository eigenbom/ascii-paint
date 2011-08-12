#ifndef ASCII_PAINT_COLOR_BOX_H
#define ASCII_PAINT_COLOR_BOX_H

#include "gui/gui.hpp"
#include "app_user.h"

class TCODColor;

// A ColorBox is the same as an Image except for one difference.
// When clicked a ColorBox will display a screen full of colors from which
// the user is to pick a color. The color that the user picks gets put into
// a variable which we set earlier using setColorToChange. (Hmm.. Maybe I
// should just have a member called col which will hold the color. Oh well.)
class ColorBox : public Image, public AppUser {
	public :
		ColorBox(int x,int y,int w, int h, const char *tip=NULL);
		virtual ~ColorBox();
		void setColorToChange(TCODColor *col);

		int satRows;
		int satCols;
		int hueCols;
		int valRows;
	protected :
		void onButtonClick();
		void drawColorGrid(TCODConsole *con, float saturation);

		TCODColor *colorToChange;
};

#endif
