#include "app.h"
#include "color_box.h"
#include <cstdio>

ColorBox::ColorBox(int x,int y,int w, int h, const char *tip) : Image(x, y, w, h, tip) {
	satRows = 1;
	satCols = 2;
        hueCols = app->windowWidth - 16;
	valRows = app->windowHeight;
}

ColorBox::~ColorBox() {

}

void ColorBox::setColorToChange(TCODColor *col) {
	// colorToChange is the color that gets set when the user picks his color
	colorToChange = col;
}

void ColorBox::drawColorGrid(TCODConsole *con, float saturation) {
	int x;
	int y;
	TCODColor col;

	// Draw a grid of colors as follows:
	// A table of saturation groups with satRows rows and satCols columns
	// In the columns of each cell should hueCols columns of hues
	// In the rows of each cell should valRows rows of values(brightness)
	for(int sr = 0; sr < satRows; sr++) {
		for(int sc = 0; sc < satCols; sc++) {
                        for(int hc = 0; hc < hueCols; hc++) {
				for(int vr = 0; vr < valRows; vr++) {
                                        x = (sc * hueCols) + hc;
                                        y = (sr * valRows) + vr;

					float hue = (float)hc / hueCols * 360.0;
					//float sat = 1 - (float)(sr * satCols + sc) / (satRows * satCols);
					float sat = saturation;
					float val = 1 - (float)vr / (valRows - 1); // the valRows-1 ensures we get 0,0,0 in the grid

					// Make sure the last saturation cell is grayscale
					if(sr == satRows - 1 && sc == satCols - 1)
						sat = 0;

					col.setHSV(hue, sat, val);

                                        if(x < app->windowWidth && y < app->windowHeight)
                                        con->setCharBackground(x, y, col);
				}
			}
		}
	}
}

void ColorBox::onButtonClick() {
	int windowWidth = app->windowWidth;
	int windowHeight = app->windowHeight;
	TCODConsole *con = new TCODConsole(windowWidth, windowHeight);

	con->clear();

	drawColorGrid(con, 1);


	con->printEx(app->windowWidth - 8, app->windowHeight - 1,  TCOD_BKGND_SET, TCOD_CENTER, "Saturation");

	con->flush();

	TCOD_key_t key;
	TCOD_mouse_t mouse;
	int infoX = 0;
	int infoY = 0;
	int infoW = 15;
	int infoH = 10;
	TCODConsole *con2 = new TCODConsole(infoW, infoH);

	do {
		TCODSystem::sleepMilli(10); // To not stress out the processor too much

		key = TCODConsole::checkForKeypress(true);
		mouse = TCODMouse::getStatus();


		// If the mouse is on the gray zone on the right
		if(mouse.cx > windowWidth - 16 && mouse.rbutton) {
			// Change the saturation of the colors
			drawColorGrid(con, 1 - ((float)mouse.cy)/windowHeight);
		}

		TCODColor col = con->getCharBackground(mouse.cx, mouse.cy);

		// Draw the info box
		con2->setDefaultBackground(TCODColor::white);
		con2->printFrame(0, 0, infoW, infoH, true, TCOD_BKGND_NONE, "Info");
		con2->printEx(1, 2,  TCOD_BKGND_NONE,TCOD_LEFT,
				"r: %i\ng: %i\nb: %i",
				col.r, col.g, col.b);

		float h, s, v;
		col.getHSV(&h, &s, &v);
		con2->printEx(1, 6, TCOD_BKGND_NONE,TCOD_LEFT, 
				"h: %.0f\ns: %.0f\nv: %.0f\n",
				h/360*255, s * 255, v * 255);

		con2->setDefaultBackground(col);
		con2->rect(7, 2, 7, 7, true);

		con2->flush();


		// Make sure that the info window does not go out of the screen
		if(mouse.cx >= windowWidth - infoW - 2)
			infoX = mouse.cx - infoW - 2;
		else
			infoX = mouse.cx + 2;

		if(mouse.cy >= windowHeight - infoH)
			infoY = windowHeight - infoH;
		else
			infoY = mouse.cy + 2;

		TCODConsole::blit(con, 0, 0, windowWidth, windowHeight, TCODConsole::root, 0, 0);
		TCODConsole::blit(con2, 0, 0, infoW, infoH, TCODConsole::root, infoX,  infoY);

		// Pressing left button chooses the color
		if(mouse.lbutton_pressed) {
			*colorToChange = con->getCharBackground(mouse.cx, mouse.cy);
			break;
		}
	} while(key.vk != TCODK_ESCAPE);

	delete con;
	delete con2;

}
