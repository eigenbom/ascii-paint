#include <cstdio>
#include <string>
#include <sstream>
#include "libtcod.hpp"
#include "gui/gui.hpp"

#include "app.h"
#include "asc_file.hpp"
#include "cell_operation.h"
#include "color_box.h"
#include "copy_operation.h"
#include "fill_operation.h"
#include "line_operation.h"
#include "misc.h"
#include "new_operation.h"
#include "open_operation.h"
#include "oval_operation.h"
#include "paste_operation.h"
#include "pick_operation.h"
#include "quit_operation.h"
#include "rect_operation.h"
#include "redo_operation.h"
#include "save_operation.h"
#include "text_operation.h"
#include "undo_operation.h"

App* App::instance = NULL;

App::App()
:currentPaletteIndex(-1)
{
        AppUser::setApp(this);
        gui = new AppGui;
        data = new AppData;
        // set singleton instance
        instance = this;
}

int App::exec() {
	mapVersion = 0.3;

	overlayFade = 1.0;

	inTypingMode = false;

	data->load();
	
	brush1 = initBrush1;
	brush2 = initBrush2;

	filename = "";
	quit = false;

	TCODConsole::setCustomFont(fontFilename.c_str(), fontType | fontLayout);
	TCODConsole::initRoot(windowWidth, windowHeight, "Ascii Paint v0.3.3", fullscreen, TCOD_RENDERER_SDL);
	TCODMouse::showCursor(true);
	TCODSystem::setFps(fpsGoal);

	TCODConsole::root->setDefaultBackground(windowBackgroundColor);

	/*
	// Print credits here
	int w = 30;
	const char* credits = "%cDevs%c\n"
			"2011- Ben Porter\n"
			"2009-2010 Shafqat Bhuiyan & Jice\n"
			"\n\n%cPress any key to skip...%c";

	int h = TCODConsole::root->getHeightRect(0, 0, w, 20, credits);
	int x = (windowWidth-w)/2;
	int y = (windowHeight-h)/2;

	TCODConsole::root->printFrame(x-1,y-1,w+2,h+20, true, TCOD_BKGND_DEFAULT, "Ascii Paint");

	TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::red,TCODColor::black);
	TCODConsole::setColorControl(TCOD_COLCTRL_2,TCODColor::darkRed,TCODColor::black);
	TCODConsole::root->printRectEx(x,y,w,h, TCOD_BKGND_DEFAULT, TCOD_LEFT,
			credits, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, TCOD_COLCTRL_STOP
	);
	bool finished = false;
    while(!finished)
    {
    	finished = TCODConsole::renderCredits(x+w/2-10,y+10,false);
		TCODConsole::root->flush();
		key = TCODConsole::checkForKeypress();
		if ( key.vk != TCODK_NONE ) break;
    }
    */

	canvasCon = NULL;
	solidCon = NULL;
	overlayCon = NULL;
	solidOverlayCon = NULL;

	// grid defaults loaded using cfg file
	/*
	showGrid = false;
	gridW = 8;
	gridH = 8;
	*/
	clipboardImage = CanvasImage();

	initCanvas();

	gui->build();
	gui->hideAllOptions();

	initOperations();

	while(!quit && !TCODConsole::isWindowClosed()) {
		key = TCODConsole::checkForKeypress();
		mouse = TCODMouse::getStatus();

		if(!inTypingMode)
			moveMouseWithKeyBoard();

		gui->update();

		canvasMouseX = mouse.cx - canvasOffsetX;
		canvasMouseY = mouse.cy - canvasOffsetY;

		if(pastImages.empty())
			canvasModified = false;
		//} else {
			//canvasModified = true;
		//}

		if(shouldOperationUpdate())
			currentOperation->update();

		handleKeyboardShortcuts();
		handleCanvasDragging();

		TCODConsole::root->setDefaultBackground(windowBackgroundColor);
		TCODConsole::root->setDefaultForeground(TCODColor::darkGrey);
		TCODConsole::root->clear();
		/*
		for(int i=0;i<windowWidth;i++){
			for(int j=0;j<windowWidth;j++){
				TCODConsole::root->putChar(i,j,TCOD_CHAR_BLOCK1);
			}
		}
		*/
		// TCODConsole::root->putChar(i,j,TCOD_CHAR_CROSS);

		int blitSrcX;
		int blitSrcY;
		int blitWidth;
		int blitHeight;
		int blitDestX;
		int blitDestY;

		canvasOffsetX = min(canvasOffsetX, windowWidth - 1);
		canvasOffsetY = min(canvasOffsetY, windowHeight - 1);

		canvasOffsetX = max(canvasOffsetX, 1 - canvasWidth);
		canvasOffsetY = max(canvasOffsetY, 1 - canvasHeight);

		blitDestX = max(0, canvasOffsetX);
		blitDestY = max(0, canvasOffsetY);

		blitSrcX = max(0, -canvasOffsetX);
		blitSrcY = max(0, -canvasOffsetY);

		blitWidth = min(canvasWidth - blitSrcX, windowWidth - blitDestX);
		blitHeight = min(canvasHeight - blitSrcY, windowHeight - blitDestY);

		if(gui->viewImageToggleButton->isPressed())
                        TCODConsole::blit(canvasCon, blitSrcX, blitSrcY, blitWidth, blitHeight, TCODConsole::root, blitDestX, blitDestY);
		if(gui->viewSolidToggleButton->isPressed())
                        TCODConsole::blit(solidCon, blitSrcX, blitSrcY, blitWidth, blitHeight, TCODConsole::root, blitDestX, blitDestY, 0.5, 0.5);
        TCODConsole::blit(overlayCon, blitSrcX, blitSrcY, blitWidth, blitHeight, TCODConsole::root, blitDestX, blitDestY, overlayFade, overlayFade);

        if (gui->viewGridToggleButton->isPressed())
        	showGrid = true;
        else
        	showGrid = false;

		if (showGrid){

			if (gui->gridModeToggleButton->isPressed())
				gridMode = GRID_MODE_BULLETS;
			else
				gridMode = GRID_MODE_CORNERS;

			// draw grid points at the intersections
			int maxx = canvasOffsetX+canvasWidth, maxy = canvasOffsetY+canvasHeight;
			for(int x=canvasOffsetX;x<maxx;x+=gridW){
				for(int y=canvasOffsetY;y<maxy;y+=gridH){
					// grid mode
					if (gridMode==GRID_MODE_BULLETS){
						TCODConsole::root->putChar(x,y,TCOD_CHAR_BULLET, TCOD_BKGND_NONE);
					}
					else {
						TCODConsole::root->putChar(x,y,TCOD_CHAR_NW,TCOD_BKGND_NONE);
						if (x+gridW-1<maxx)
							TCODConsole::root->putChar(x+gridW-1,y,TCOD_CHAR_NE,TCOD_BKGND_NONE);
						if (y+gridH-1<maxy)
							TCODConsole::root->putChar(x,y+gridH-1,TCOD_CHAR_SW,TCOD_BKGND_NONE);
						if (x+gridW-1<maxx and y+gridH-1<maxy)
							TCODConsole::root->putChar(x+gridW-1,y+gridH-1,TCOD_CHAR_SE,TCOD_BKGND_NONE);
					}
				}
			}
		}

		// Show frame corners
		TCODConsole::root->putChar(canvasOffsetX-1,canvasOffsetY-1,TCOD_CHAR_NW, TCOD_BKGND_NONE);
		TCODConsole::root->putChar(canvasOffsetX+canvasWidth,canvasOffsetY-1,TCOD_CHAR_NE, TCOD_BKGND_NONE);
		TCODConsole::root->putChar(canvasOffsetX-1,canvasOffsetY+canvasHeight,TCOD_CHAR_SW, TCOD_BKGND_NONE);
		TCODConsole::root->putChar(canvasOffsetX+canvasWidth,canvasOffsetY+canvasHeight,TCOD_CHAR_SE, TCOD_BKGND_NONE);

		gui->draw();
		TCODConsole::flush();
	}

	data->save();

	return 0;
}

App::~App() {
	for(int i=0;i<palettes.size();i++){
		delete palettes.get(i);
	}
	palettes.clear();
}

void App::initBrushes() {
	brush1.fore = TCODColor::black;
	brush1.back = TCODColor::white;
	brush1.symbol = 'a';
        brush1.solid = true;
        brush1.walkable = true;

	brush2.fore = TCODColor::white;
	brush2.back = TCODColor::black;
	brush2.symbol = 'b';
        brush2.solid = true;
        brush2.walkable = true;

}

	void App::initCanvas() {
		if(canvasCon != NULL)
			delete canvasCon;

		if(solidCon != NULL)
			delete solidCon;

		if(overlayCon != NULL)
			delete overlayCon;

		if(solidOverlayCon != NULL)
			delete solidOverlayCon;

		canvasCon = new TCODConsole(canvasWidth, canvasHeight);
		canvasCon->setDefaultForeground(brush1.fore);
		canvasCon->setDefaultBackground(brush1.back);
		canvasCon->clear();

		solidCon = new TCODConsole(canvasWidth, canvasHeight);
		solidCon->setDefaultForeground(TCODColor(0, 0, 0));
		solidCon->setDefaultBackground(TCODColor(255, 255, 255));
		solidCon->clear();

		overlayCon = new TCODConsole(canvasWidth, canvasHeight);
		overlayCon->setDefaultBackground(TCODColor(1, 2, 3)); // 1,2,3 is an uncommon color ;)
		overlayCon->setKeyColor(TCODColor(1, 2, 3));
		overlayCon->clear();

		solidOverlayCon = new TCODConsole(canvasWidth, canvasHeight);
		solidOverlayCon->setDefaultBackground(TCODColor(1, 2, 3));
		solidOverlayCon->setKeyColor(TCODColor(1, 2, 3));
		solidOverlayCon->clear();

		for(int x = 0; x < canvasWidth; x++) {
			for(int y = 0; y < canvasHeight; y++) {
				canvasCon->setChar(x, y, ' ');
				canvasCon->setCharForeground(x, y, brush1.fore);
				canvasCon->setCharBackground(x, y, brush1.back);
			}
		}
		canvasModified = false;
		isCanvasBeingDragged = false;

		// Clear undo and redo stacks
		pastImages.clear();
		futureImages.clear();

		resetCanvasView();
	}

void App::initOperations() {
	currentOperation = NULL;
	changeOperation(new CellOperation);
}

void App::resizeCanvas(int newWidth, int newHeight){
	// Create new smaller canvases
	// Blit the old canvases onto the new ones
	// And then assign the old ones to the new ones
	// And delete the old ones

	int copyW = std::min(newWidth,canvasWidth);
	int copyH = std::min(newHeight,canvasHeight);
	canvasWidth = newWidth;
	canvasHeight = newHeight;

	TCODConsole* newCanvasCon = new TCODConsole(newWidth, newHeight);
	newCanvasCon->setDefaultForeground(brush1.fore);
	newCanvasCon->setDefaultBackground(brush1.back);
	newCanvasCon->clear();
	TCODConsole::blit(canvasCon,0,0,copyW,copyH,newCanvasCon,0,0);
	delete canvasCon;
	canvasCon = newCanvasCon;

	TCODConsole* newSolidCon = new TCODConsole(newWidth, newHeight);
	newSolidCon->setDefaultForeground(TCODColor(0, 0, 0));
	newSolidCon->setDefaultBackground(TCODColor(255, 255, 255));
	newSolidCon->clear();
	TCODConsole::blit(solidCon,0,0,copyW,copyH,newSolidCon,0,0);
	delete solidCon;
	solidCon = newSolidCon;

	TCODConsole* newOverlayCon = new TCODConsole(newWidth, newHeight);
	newOverlayCon->setDefaultBackground(TCODColor(1, 2, 3)); // 1,2,3 is an uncommon color ;)
	newOverlayCon->setKeyColor(TCODColor(1, 2, 3));
	newOverlayCon->clear();
	TCODConsole::blit(overlayCon,0,0,copyW,copyH,newOverlayCon,0,0);
	delete overlayCon;
	overlayCon = newOverlayCon;

	TCODConsole* newSolidOverlayCon = new TCODConsole(newWidth, newHeight);
	newSolidOverlayCon->setDefaultBackground(TCODColor(1, 2, 3));
	newSolidOverlayCon->setKeyColor(TCODColor(1, 2, 3));
	newSolidOverlayCon->clear();
	TCODConsole::blit(solidOverlayCon,0,0,copyW,copyH,newSolidOverlayCon,0,0);
	delete solidOverlayCon;
	solidOverlayCon = newSolidOverlayCon;

	// resetCanvasView();
}

void App::changeOperation(Operation *newOperation) {
	if(currentOperation != NULL) {
		currentOperation->end();
	}

	previousOperation = currentOperation;

	overlayCon->clear();
	gui->hideAllOptions();

	// Refresh mouse(might remove this later)
	//mouse = TCODMouse::getStatus();
	// TCODConsole::root->setDefaultBackground(gui->backgroundColor1);
	// TCODConsole::root->setDefaultForeground(gui->foregroundColor1);

	currentOperation = newOperation;
	currentOperation->start();
}

	bool App::shouldOperationUpdate() {
		if(TCODConsole::isKeyPressed(TCODK_CONTROL))
			return false;
		if(gui->hideType == AppGui::TOTAL_HIDE)
			return true;
		if(Widget::focus && !currentOperation->isActive())
			return false;
		return true;
	}


CanvasImage* App::getCanvasImage() {
	CanvasImage* canvasImg = new CanvasImage;
	Brush brush;

	for(int x = 0; x < canvasWidth; x++) {
		for(int y = 0; y < canvasHeight; y++) {
			brush.symbol = canvasCon->getChar(x, y);
			brush.fore = canvasCon->getCharForeground(x, y);
			brush.back = canvasCon->getCharBackground(x, y);
			if(solidCon->getCharBackground(x, y) == TCODColor(0, 0, 255))
				brush.solid = true;
			else
				brush.solid = false;

			canvasImg->push_back(brush);
		}
	}

	return canvasImg;

}

void App::setCanvasImage(CanvasImage& canvasImg) {
	for(int x = 0; x < canvasWidth; x++) {
		for(int y = 0; y < canvasHeight; y++) {
			canvasCon->setChar(x, y, canvasImg[x * canvasHeight + y].symbol);
			canvasCon->setCharForeground(x, y, canvasImg[x * canvasHeight + y].fore);
			canvasCon->setCharBackground(x, y, canvasImg[x * canvasHeight + y].back);
			if(canvasImg[x * canvasHeight + y].solid)
				solidCon->setCharBackground(x, y, TCODColor(0, 0, 255));
			else
				solidCon->setCharBackground(x, y, TCODColor(255, 255, 255));
		}
	}

}

void App::setGridDimensions(int w, int h){
	gridW = w;
	gridH = h;

	gui->gwSlider->setValue(gridW);
	gui->ghSlider->setValue(gridH);
}

void App::setShowGrid(bool on){
	showGrid = on;
	gui->viewGridToggleButton->setPressed(showGrid);
}

// Note: These undo methods might not be very effecient memory wise
// need to look at the possible solutions later...
void App::addUndo() {
	canvasModified = true;
	pastImages.push_back(*getCanvasImage());
	// Clear the redo list. However not doing this can be helpful at times.
	// But most people are not use to this.
	futureImages.clear();
}

void App::doUndo() {
	if(pastImages.size() > 0) {
		futureImages.push_back(*getCanvasImage());
		setCanvasImage(pastImages[pastImages.size() - 1]);
		pastImages.pop_back();
	}
}

void App::doRedo() {
	if(futureImages.size() > 0) {
		pastImages.push_back(*getCanvasImage());
		setCanvasImage(futureImages[futureImages.size() - 1]);
		futureImages.pop_back();
	}
}


void App::copyCanvasToClipboard(int x, int y, int w, int h){
	clipboardImage.clear();
	Brush brush;

	clipboardWidth = w;
	clipboardHeight = h;
	for(int dx = 0; dx < w; dx++) {
		for(int dy = 0; dy < h; dy++) {
			brush.symbol = canvasCon->getChar(x+dx, y+dy);
			brush.fore = canvasCon->getCharForeground(x+dx, y+dy);
			brush.back = canvasCon->getCharBackground(x+dx, y+dy);
			if(solidCon->getCharBackground(x+dx, y+dy) == TCODColor(0, 0, 255))
				brush.solid = true;
			else
				brush.solid = false;

			clipboardImage.push_back(brush);
		}
	}
}

void App::applyClipboardToOverlay(int x, int y){
	if (clipboardImage.empty()) return;

	for(int dx = 0; dx < clipboardWidth; dx++) {
		for(int dy = 0; dy < clipboardHeight; dy++) {
			overlayCon->setChar(x+dx, y+dy, clipboardImage[dx * clipboardHeight + dy].symbol);
			overlayCon->setCharForeground(x+dx, y+dy, clipboardImage[dx * clipboardHeight + dy].fore);
			overlayCon->setCharBackground(x+dx, y+dy, clipboardImage[dx * clipboardHeight + dy].back);
			if(clipboardImage[dx * clipboardHeight + dy].solid)
				solidOverlayCon->setCharBackground(x+dx, y+dy, TCODColor(0, 0, 255));
			else
				solidOverlayCon->setCharBackground(x+dx, y+dy, TCODColor(255, 255, 255));
		}
	}
}

void App::applyClipboardToCanvas(int x, int y){
	if (clipboardImage.empty()) return;

	for(int dx = 0; dx < clipboardWidth; dx++) {
		for(int dy = 0; dy < clipboardHeight; dy++) {
			canvasCon->setChar(x+dx, y+dy, clipboardImage[dx * clipboardHeight + dy].symbol);
			canvasCon->setCharForeground(x+dx, y+dy, clipboardImage[dx * clipboardHeight + dy].fore);
			canvasCon->setCharBackground(x+dx, y+dy, clipboardImage[dx * clipboardHeight + dy].back);
			if(clipboardImage[dx * clipboardHeight + dy].solid)
				solidCon->setCharBackground(x+dx, y+dy, TCODColor(0, 0, 255));
			else
				solidCon->setCharBackground(x+dx, y+dy, TCODColor(255, 255, 255));
		}
	}
}

// This function applies the brush to the selected cell of the console
// con. However if the user has not set use symbol, use fore or use back
// then it will not apply that part of the brush to the console but instead
// leave that part (symbol, fore or back) the way it is
	void App::applyBrushToOverlayCell(int x, int y, Brush *brush) {
                if(gui->useSymbolToggleButton->isPressed())
                        overlayCon->setChar(x, y, brush->symbol);
                else
                        overlayCon->setChar(x, y, canvasCon->getChar(x, y));

                if(gui->useForegroundToggleButton->isPressed())
                        overlayCon->setCharForeground(x, y, brush->fore);
                else
                        overlayCon->setCharForeground(x, y, canvasCon->getCharForeground(x, y));

                if(gui->useBackgroundToggleButton->isPressed())
                        overlayCon->setCharBackground(x, y, brush->back);
                else
                        overlayCon->setCharBackground(x, y, canvasCon->getCharBackground(x, y));

                if(gui->useSolidToggleButton->isPressed()) {
                        if(brush->solid)
                                solidOverlayCon->setCharBackground(x, y, TCODColor(0, 0, 255));
                        else
                                solidOverlayCon->setCharBackground(x, y, TCODColor(255, 255, 255));
                }
	}

void App::clearOverlay() {
	overlayCon->setDefaultBackground(TCODColor(1, 2, 3));
	overlayCon->clear();

	solidOverlayCon->setDefaultBackground(TCODColor(1, 2, 3));
	solidOverlayCon->clear();
}

void App::applyOverlayToCanvas() {
        TCODConsole::blit(overlayCon, 0, 0, canvasWidth, canvasHeight, canvasCon, 0, 0);
        TCODConsole::blit(solidOverlayCon, 0, 0, canvasWidth, canvasHeight, solidCon, 0, 0);
}

void App::applyCanvasToOverlay() {
        TCODConsole::blit(canvasCon, 0, 0, canvasWidth, canvasHeight, overlayCon, 0, 0);
        TCODConsole::blit(solidCon, 0, 0, canvasWidth, canvasHeight, solidOverlayCon, 0, 0);
}

void App::setOverlayFade(float fade) {
	overlayFade = fade;
}

void App::resetCanvasView() {
	canvasOffsetX = (windowWidth - canvasWidth) / 2;
	canvasOffsetY = (windowHeight - canvasHeight) / 2;
}

App* App::getSingleton(){return instance;}

void App::handleKeyboardShortcuts() {
	// Shortcuts
	if((key.lctrl || key.rctrl) && key.c == 'q') { // CTRL+Q 
		changeOperation(new QuitOperation);
	} else if((key.lctrl || key.rctrl) && key.c == 'n') { // CTRL+N
		changeOperation(new NewOperation);
	} else if((key.lctrl || key.rctrl) && key.c == 'o') { // CTRL+O
		changeOperation(new OpenOperation);
	} else if((key.lctrl || key.rctrl) && key.c == 's') { // CTRL+S
		changeOperation(new SaveOperation);
	} else if((key.lctrl || key.rctrl) && key.c == 'z') { // CTRL+Z
		changeOperation(new UndoOperation);
	} else if((key.lctrl || key.rctrl) && key.c == 'z') { // CTRL+Z
		changeOperation(new UndoOperation);
	} else if((key.lctrl || key.rctrl) && key.c == 'y') { // CTRL+Y 
		changeOperation(new RedoOperation);
	} else if((key.lctrl || key.rctrl) && key.c == 'h') { // CTRL+H
		TCODMouse::isCursorVisible() ? TCODMouse::showCursor(false) : TCODMouse::showCursor(true);
	} else if((key.lctrl || key.rctrl) && key.c == 'c') { // CTRL+C
		gui->copyOperationButton->select();
		changeOperation(new CopyOperation);
	} else if((key.lctrl || key.rctrl) && key.c == 'v') { // CTRL+V
		gui->pasteOperationButton->select();
		changeOperation(new PasteOperation);
	}

	if(!(key.lctrl && !key.rctrl)) {
		if(!inTypingMode) { // Disable the shortcuts when we are typing
			if(key.c == 'c') {
				gui->cellOperationButton->select();
				changeOperation(new CellOperation);
			} else if(key.c == 'l') {
				gui->lineOperationButton->select();
				changeOperation(new LineOperation);
			} else if(key.c == 'r') {
				gui->rectOperationButton->select();
				changeOperation(new RectOperation);
			} else if(key.c == 'o') {
				gui->ovalOperationButton->select();
				changeOperation(new OvalOperation);
			} else if(key.c == 'f') {
				gui->fillOperationButton->select();
				changeOperation(new FillOperation);
			} else if(key.c == 't') {
				gui->textOperationButton->select();
				changeOperation(new TextOperation);
			} else if(key.c == 'p') {
				changeOperation(new PickOperation);
			}
		}
	}

	// ALT+ENTER = toggle full screen
	if(key.vk == TCODK_ENTER && (key.lalt || key.ralt)) {
		TCODConsole::setFullscreen(!TCODConsole::isFullscreen());
		TCODMouse::showCursor(true);
	}


}

void App::handleCanvasDragging() {
	static TCOD_mouse_t mouseOnDragStart;
	static int canvasOffsetXBeforeDrag;
	static int canvasOffsetYBeforeDrag;

	if(TCODConsole::isKeyPressed(TCODK_CONTROL) && mouse.lbutton) {
		canvasOffsetX = canvasOffsetXBeforeDrag + mouse.cx - mouseOnDragStart.cx;
		canvasOffsetY = canvasOffsetYBeforeDrag + mouse.cy - mouseOnDragStart.cy;
		isCanvasBeingDragged = true;
	} else {
		mouseOnDragStart = mouse;
		canvasOffsetXBeforeDrag = canvasOffsetX;
		canvasOffsetYBeforeDrag = canvasOffsetY;
		isCanvasBeingDragged = false;
	}

	// Pressing CTRL and clicking right mouse button resets the canvas view
	if(TCODConsole::isKeyPressed(TCODK_CONTROL) && mouse.rbutton_pressed) {
		resetCanvasView();
	}
}

void App::moveMouseWithKeyBoard() {
	static int cellWidth = 8;
	static int cellHeight = 8;

	if(mouse.cx != 0 && mouse.cy != 0) {
		cellWidth = mouse.x / mouse.cx;
		cellHeight = mouse.y / mouse.cy;
	}

	if(key.vk == TCODK_UP || key.vk == TCODK_KP8) {
		TCODMouse::move(mouse.x, mouse.y - cellHeight);
	} else if(key.vk == TCODK_DOWN || key.vk == TCODK_KP2) {
		TCODMouse::move(mouse.x, mouse.y + cellHeight);
	} else if(key.vk == TCODK_LEFT || key.vk == TCODK_KP4) {
		TCODMouse::move(mouse.x - cellWidth, mouse.y);
	} else if(key.vk == TCODK_RIGHT || key.vk == TCODK_KP6) {
		TCODMouse::move(mouse.x + cellWidth, mouse.y);
	} else if(key.vk == TCODK_KP1) {
		TCODMouse::move(mouse.x - cellWidth, mouse.y + cellHeight);
	} else if(key.vk == TCODK_KP3) {
		TCODMouse::move(mouse.x + cellWidth, mouse.y + cellHeight);
	} else if(key.vk == TCODK_KP7) {
		TCODMouse::move(mouse.x - cellWidth, mouse.y - cellHeight);
	} else if(key.vk == TCODK_KP9) {
		TCODMouse::move(mouse.x + cellWidth, mouse.y - cellHeight);
	}
}
