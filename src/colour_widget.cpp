#include "colour_widget.h"

#include "libtcod.hpp"
#include "gui/gui.hpp"
#include "app.h"

/*
 * A colour widget is laid out as follows:
 *
 * +----------------+
 * |CGA           <>|
 * |................|
 * |.....colours....|
 * |             TCS|
 * +----------------+
 *
 * where T selects 255,0,255, and c opens the any colour chooser,
 * and S swaps the foreground and background colours
 *
 * Cliking the <> buttons changes the palette
 *
 * The current fg and bg colours are given by x,y coords into the
 * colour matrix. An 'f' appears on the foreground color, and a 'b'
 * appears on the bg colour. An 'x' appears if both are at the same spot.
 *
 */


static const int HEADER_HEIGHT = 1;
static const int FOOTER_HEIGHT = 1;
static const int MIN_WIDTH = 16;

ColourWidget::ColourWidget(int x, int y,  widget_callback_t _cbk, void* _userData)
:Widget(x,y,1+MIN_WIDTH,HEADER_HEIGHT + 2)
,sfore(),sback()
,fx(0),fy(HEADER_HEIGHT),bx(1),by(HEADER_HEIGHT)
,cbk(_cbk),userData(_userData),palette(NULL),pChange(0)
{
	satRows = 1;
	satCols = 2;
	hueCols = App::getSingleton()->windowWidth - 16;
	valRows = App::getSingleton()->windowHeight;
}

ColourWidget::~ColourWidget(){

}

void ColourWidget::setPaletteCallback(widget_callback_t cbk, void* userData){
	pcbk = cbk;
	puserData = userData;
}

void ColourWidget::setPalette(Palette* p){
	palette = p;

	// update dimensions of widget
	w = std::max(palette->width,MIN_WIDTH);
	h = HEADER_HEIGHT + FOOTER_HEIGHT + palette->height;
	fx = 0; fy = HEADER_HEIGHT;
	bx = 1; by = HEADER_HEIGHT;
	sfore = getColourAt(fx,fy);
	sback = getColourAt(bx,by);
	if (cbk){
		cbk(this,userData);
	}
}

void ColourWidget::render(){
	// Draw a grid of colours in the palette
	con->setDefaultBackground(back);
	con->setDefaultForeground(fore);

	if (palette){
		// display header
		con->print(x,y,"%s", palette->name.c_str());
		con->putChar(x+w-2,y,'<');
		con->putChar(x+w-1,y,'>');
		// display colours
		int numCols = palette->colours.size();
		for(int ci=0;ci<numCols;ci++){
			int dx = ci%palette->width;
			int dy = ci/palette->width;
			con->putCharEx(x+dx,y+dy+HEADER_HEIGHT,219,getColourAt(dx,dy+HEADER_HEIGHT),back); // palette->colours.get(ci),back);
		}
		// display footer
		con->putCharEx(x+palette->width-3,y+HEADER_HEIGHT+palette->height,
				isTransparentColourLocation(fx,fy)?(isTransparentColourLocation(bx,by)?'X':'F'):isTransparentColourLocation(bx,by)?'B':'T',
				fore,back);

		bool fc = isChooserColourLocation(fx,fy), bc = isChooserColourLocation(bx,by);
		con->putCharEx(x+palette->width-2,y+HEADER_HEIGHT+palette->height,
				fc?(bc?'X':'F'):bc?'B':'C',fore,back);
		con->putCharEx(x+palette->width-1,y+HEADER_HEIGHT+palette->height,'S',fore,back);

		// show f and b highlights
		if (not (fx==bx and fy==by)){
			if (isNormalColourLocation(fx,fy)){
				TCODColor bg = getColourAt(fx,fy); // palette->colours.get(fx + fy*palette->width);
				TCODColor fg = TCODColor::white;
				// make the 'f' either bright or dark depending on the brightness of the bg colour
				float val = bg.getValue ();
				if (val >= .7) {
					fg = TCODColor::black;
				}
				con->putCharEx(x+fx,y+fy,'F',fg,bg);
			}
			if (isNormalColourLocation(bx,by)){
				TCODColor bg = getColourAt(bx,by); // palette->colours.get(fx + fy*palette->width);
				TCODColor fg = TCODColor::white;
				// make the 'b' either bright or dark depending on the brightness of the bg colour
				float val = bg.getValue ();
				if (val >= .7) {
					fg = TCODColor::black;
				}
				con->putCharEx(x+bx,y+by,'B',fg,bg);
			}
		}
		else {
			if (isNormalColourLocation(fx,fy)){
				TCODColor bg = getColourAt(fx,fy); // palette->colours.get(fx + fy*palette->width);
				TCODColor fg = TCODColor::white;
				// make the 'b' either bright or dark depending on the brightness of the bg colour
				float val = bg.getValue ();
				if (val >= .7) {
					fg = TCODColor::black;
				}
				con->putCharEx(x+bx,y+by,'X',fg,bg);
			}
		}
	}
	else {
		con->print(x,y,"No palettes!");
	}

}

void ColourWidget::update(const TCOD_key_t k){
	Widget::update(k);

	// detect right click
	if (mouseIn){
		if (mouse.rbutton) onRButtonClick();
	}
}

void ColourWidget::onButtonClick(){
	 if (mouseIn) {
		int dx = mouse.cx - x;
		int dy = mouse.cy - y;
		if (isNormalColourLocation(dx, dy)){
			fx = dx;
			fy = dy;
			sfore = getColourAt(fx,fy);
			if (cbk) cbk(this,userData);
		}
		else if (doPaletteSelection(dx,dy)) return;
		else if (isSpecialColourLocation(dx, dy)){
			if (isTransparentColourLocation(dx,dy)){
				fx = dx;
				fy = dy;
				sfore = TCODColor(255,0,255);
				if (cbk) cbk(this,userData);
			}
			//
			else if (isSwapColourLocation(dx,dy)){
				TCODColor tmp = sfore;
				sfore = sback;
				sback = tmp;
				int tx = fx, ty = fy;
				fx = bx; fy = by;
				bx = tx; by = ty;
				if (cbk) cbk(this,userData);
			}
			//
			else if (isChooserColourLocation(dx,dy)){
				fx = dx; fy = dy;
				setColorToChange(&sfore);
				showChooser();
				if (cbk) cbk(this,userData);
			}
		}
	 }
}

void ColourWidget::onRButtonClick(){
	 if (mouseIn) {
		int dx = mouse.cx - x;
		int dy = mouse.cy - y;
		if (isNormalColourLocation(dx, dy)){
			bx = dx;
			by = dy;
			sback = getColourAt(bx,by);
			if (cbk) cbk(this,userData);
		}
		else if (doPaletteSelection(dx,dy)) return;
		else if (isSpecialColourLocation(dx,dy)){

			if (isTransparentColourLocation(dx,dy)){
				bx = dx;
				by = dy;
				sback = TCODColor(255,0,255);
				if (cbk) cbk(this,userData);
			}

			//
			else if (isSwapColourLocation(dx,dy)){
				TCODColor tmp = sfore;
				sfore = sback;
				sback = tmp;
				int tx = fx, ty = fy;
				fx = bx; fy = by;
				bx = tx; by = ty;
				if (cbk) cbk(this,userData);
			}

			//
			else if (isChooserColourLocation(dx,dy)){
				bx = dx; by = dy;
				setColorToChange(&sback);
				showChooser();
				if (cbk) cbk(this,userData);
			}
		}
	 }
}

TCODColor ColourWidget::getColourAt(int x, int y){
	if (palette==NULL) return TCODColor();

	if (y < HEADER_HEIGHT+palette->height) {
		// then we are choosing from the palette
		return palette->colours.get(x + (y-HEADER_HEIGHT)*palette->width);
	}
	else {
		// we are selecting one of the special colour swatches

	}
}

bool ColourWidget::isNormalColourLocation(int x, int y){
	if (palette){
		if (x>=0 and x<palette->width and y>=HEADER_HEIGHT and y<HEADER_HEIGHT+palette->height){
			int dx = x, dy = y-HEADER_HEIGHT;
			if ((dx+dy*palette->width) < palette->colours.size()) return true;
			else return false;
		}
		else return false;
	}
	else return false;
}

bool ColourWidget::isSpecialColourLocation(int x, int y){
	int DH = HEADER_HEIGHT;
	if (palette){
		DH += palette->height;
	}
	if ((x>=palette->width-3 and x<palette->width) and y==DH){
		return true;
	}
}

bool ColourWidget::isSwapColourLocation(int x, int y){
	int DH = HEADER_HEIGHT;
	if (palette){
		DH += palette->height;
	}
	return x==(palette->width-1) and y==DH;
}

bool ColourWidget::isTransparentColourLocation(int x, int y){
	int DH = HEADER_HEIGHT;
	if (palette){
		DH += palette->height;
	}
	return x==(palette->width-3) and y==DH;
}

bool ColourWidget::isChooserColourLocation(int x, int y){
	int DH = HEADER_HEIGHT;
	if (palette){
		DH += palette->height;
	}
	return x==(palette->width-2) and y==DH;
}

bool ColourWidget::doPaletteSelection(int x, int y){ // returns true if did it
	int dir = isPaletteSelectorLocation(x,y);
	if (dir==0) return false;
	else {
		// switch the palette (will result in setPalette being called)
		pChange = dir;
		if (pcbk) pcbk(this,puserData);
		return true;
	}
}

int ColourWidget::isPaletteSelectorLocation(int x, int y){ // returns 0 if not, -1 if go back, +1 if go forward
	if (y==0){
		if (x==(w-2)) return -1;
		else if (x==(w-1)) return 1;
		else return 0;
	}
	else return 0;
}

void ColourWidget::setColorToChange(TCODColor *col) {
	// colorToChange is the color that gets set when the user picks his color
	colorToChange = col;
}

void ColourWidget::drawColorGrid(TCODConsole *con, float saturation) {
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

                    if(x < App::getSingleton()->windowWidth && y < App::getSingleton()->windowHeight)
					con->setCharBackground(x, y, col);
				}
			}
		}
	}
}

void ColourWidget::showChooser() {
	int windowWidth = App::getSingleton()->windowWidth;
	int windowHeight = App::getSingleton()->windowHeight;
	TCODConsole *con = new TCODConsole(windowWidth, windowHeight);

	con->clear();

	drawColorGrid(con, 1);

	con->printEx(App::getSingleton()->windowWidth - 8, App::getSingleton()->windowHeight - 1,  TCOD_BKGND_SET, TCOD_CENTER, "Saturation");

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
