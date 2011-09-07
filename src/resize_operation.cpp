#include "resize_operation.h"

#include "libtcod.hpp"
#include "gui/gui.hpp"
#include "app.h"
#include "message_box.h"

#include <iostream>

class AnchorWidget: public Widget, AppUser {
public:
	ResizeOperation* ro;

	AnchorWidget(ResizeOperation* l):Widget(0,0,3,3){
		ro = l;
		pressed = false;
		mX = -1;
		mY = -1;
		pX = 0;
		pY = 0;
	}

	~AnchorWidget(){
	}

	void render(){
		con->setDefaultBackground(mouseIn ? backFocus : back);
		con->setDefaultForeground(mouseIn ? foreFocus : fore);

		if ( w > 0 && h > 0 ) con->rect(x,y,w,h,true,TCOD_BKGND_SET);

		if (mouseIn) {
			mX = mouse.cx - x;
			mY = mouse.cy - y;
		}
		else {
			mX = -1;
			mY = -1;
		}

		// Draw a 3x3 panel of buttons
		int chars[3][3] = {
				{92, 24, 47},
				{27, 4, 26},
				{47, 25, 92}
		};

		con->setDefaultForeground(fore);
		for(int dx=0;dx<3;dx++){
			for(int dy=0;dy<3;dy++){
				TCODColor buttonBack = back;
				if (pX==dx and pY==dy){
					buttonBack = backFocus;
				}

				if (mX==dx and mY==dy){
					con->putCharEx(x+dx,y+dy,chars[dy][dx],foreFocus,buttonBack);
				}
				else {
					con->putCharEx(x+dx,y+dy,chars[dy][dx],fore,buttonBack);
				}

			}
		}
	}

	void update(const TCOD_key_t k){
		Widget::update(k);
	}

	void onButtonPress() {
		pressed=true;
	}

	void onButtonRelease() {
		pressed=false;
	}

	void onButtonClick() {
		if (mouseIn) {
			int dx = mouse.cx - x;
			int dy = mouse.cy - y;

			if (dx>=0 and dx<3 and dy>=0 and dy<3){
				pX = dx;
				pY = dy;

				// and notify ResizeOperation...
				ResizeInfo* ri = new ResizeInfo();

				// Posmap
				static const ResizePos posmap[3][3] = {
						{RP_NW,RP_N,RP_NE},
						{RP_W,RP_CENTER,RP_E},
						{RP_SW,RP_S,RP_SE}
				};
				ri->pos = posmap[pY][pX];
				ro->setResizeInfo(ri);
			}
		}
	}

	// protected
	bool pressed;

	int mX,mY;
	int pX,pY;
};

ResizeOperation::ResizeOperation():resizeInfo(NULL) {
}

ResizeOperation::~ResizeOperation() {
	delete resizeInfo;
}

void ResizeOperation::start() {
	if (resizeInfo!=NULL) delete resizeInfo;
	resizeInfo = NULL;

	bool makeNew = true;
	if(makeNew) {
		MessageBox msgBox2("Resize", "New image dimensions", 2);
		// Add Width and Height sliders

		// I intentionally add a space after width so that it aligns nicely with
		// the height slider
		Slider widthSlider(0, 0, 4, 1.0f, 2048.0f, "width ", "New width of the image");
		widthSlider.setFormat("%.0f");
		widthSlider.setSensitivity(0.2);
		widthSlider.setValue(app->canvasWidth);

		Slider heightSlider(0, 0, 4, 1.0f, 2048.0f, "height", "New height of the image");
		heightSlider.setFormat("%.0f");
		heightSlider.setSensitivity(0.2);
		heightSlider.setValue(app->canvasHeight);

		msgBox2.addCustomWidget(&widthSlider);
		msgBox2.addCustomWidget(&heightSlider);

		// add label: msgBox2.addCustomWidget( )
		Label* sep = new Label(0,0,"");
		Label* label = new Label(0,0,"Anchor:");
		msgBox2.addCustomWidget(sep);
		msgBox2.addCustomWidget(label);
		msgBox2.addCustomWidget(sep);

		AnchorWidget* aw = new AnchorWidget(this);

		msgBox2.addCustomWidget(aw);
		msgBox2.show();

		if(msgBox2.getButtonPressed() == 1) { // OK
			// Resize
			if (resizeInfo==NULL){ resizeInfo = new ResizeInfo();}
			app->resizeCanvas(atoi(widthSlider.getValue()),atoi(heightSlider.getValue()), *resizeInfo);
		}
		delete sep;
		delete label;
		delete aw;
	}

	app->changeOperation(app->previousOperation);
}

void ResizeOperation::update() {

}

void ResizeOperation::end() {

}

void ResizeOperation::setResizeInfo(ResizeInfo* ri){if (resizeInfo) delete resizeInfo; resizeInfo = ri;}


