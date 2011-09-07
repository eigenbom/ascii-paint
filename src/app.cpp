#include <cstdio>
#include <string>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <cassert>

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

#define DEBUG_FILE_AND_LINE
//#define DEBUG_FILE_AND_LINE std::cerr << __FILE__ << ":" << __LINE__ << "\n";

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
	TCODConsole::initRoot(windowWidth, windowHeight, "Ascii Paint v0.4", fullscreen, TCOD_RENDERER_SDL);
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
	//solidCon = NULL;
	overlayCon = NULL;
	//solidOverlayCon = NULL;

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
		TCODConsole::root->setDefaultForeground(TCODColor::white);
		TCODConsole::root->clear();

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

		// Draw the layers

		// Draw the underneath pattern
		for(int x=blitDestX;x<blitDestX+blitWidth;x++){
			for(int y=blitDestY;y<blitDestY+blitHeight;y++){
				int dx = x-blitDestX;
				int dy = y-blitDestY;
				TCODConsole::root->putCharEx(x,y,0,
						(dx+dy)%2==0?TCODColor::grey:TCODColor::darkGrey,
						(dx+dy)%2==0?TCODColor::grey:TCODColor::darkGrey);
			}
		}

		// And draw the overlay at the correct depth
		for(int i=0;i<layers.size();i++){
			Layer* l = layers.get(i);

			if (l->visible)
				TCODConsole::blit(l->canvasCon, blitSrcX, blitSrcY, blitWidth, blitHeight, TCODConsole::root, blitDestX, blitDestY, l->fgalpha/255.,l->bgalpha/255.);

			if (currentLayer==l){
				TCODConsole::blit(overlayCon, blitSrcX, blitSrcY, blitWidth, blitHeight, TCODConsole::root, blitDestX, blitDestY, overlayFade, overlayFade);
			}
		}

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
	if (layers.size()>0){
		for(int i=0;i<layers.size();i++){
			Layer* l = layers.get(i);
			delete l->canvasCon;
			delete l;
		}
		layers.clear();
	}

	Layer* l = addNewLayer();

	if(overlayCon != NULL)
		delete overlayCon;

	overlayCon = new TCODConsole(canvasWidth, canvasHeight);
	overlayCon->setDefaultBackground(TCODColor(1, 2, 3)); // 1,2,3 is an uncommon color ;)
	overlayCon->setKeyColor(TCODColor(1, 2, 3));
	overlayCon->clear();

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

void App::resizeCanvas(int newWidth, int newHeight, ResizeInfo resizeInfo){
	// Create new smaller canvases
	// Blit the old canvases onto the new ones
	// And then assign the old ones to the new ones
	// And delete the old ones

	int copyW = std::min(newWidth,canvasWidth);
	int copyH = std::min(newHeight,canvasHeight);

	int nw = newWidth, cw = canvasWidth, nh = newHeight, ch = canvasHeight;

	canvasWidth = newWidth;
	canvasHeight = newHeight;

	// position to place old canvas
	int x = 0, y = 0;
	switch (resizeInfo.pos){
		case RP_N: {x=(nw-cw)/2;	y=0;		break;}
		case RP_S: {x=(nw-cw)/2;	y=nh-ch;	break;}
		case RP_W: {x=0;	y=(nh-ch)/2;	break;}
		case RP_E: {x=nw-cw;	y=(nh-ch)/2;	break;}
		case RP_NE: {x=nw-cw; y=0; break;}
		case RP_NW: {x=0; y=0; break;}
		case RP_SE: {x=nw-cw; y=nh-ch; break;}
		case RP_SW: {x=0; y=nh-ch; break;}
		case RP_CENTER: {x=(nw-cw)/2; y=(nh-ch)/2; break;}
		case RP_ABSOLUTE: {x=resizeInfo.x; y=resizeInfo.y; break;}
	}

	// resize all layers
	for(int i=0;i<layers.size();i++){
		Layer* l = layers.get(i);

		TCODConsole* newCanvasCon = new TCODConsole(newWidth, newHeight);
		newCanvasCon->setDefaultForeground(brush1.fore);
		newCanvasCon->setDefaultBackground(keyColour); // brush1.back);
		newCanvasCon->setKeyColor(keyColour); // brush1.back);
		newCanvasCon->clear();
		// TCODConsole::blit(l->canvasCon,0,0,copyW,copyH,newCanvasCon,x,y);
		TCODConsole::blit(l->canvasCon,0,0,cw,ch,newCanvasCon,x,y);
		delete l->canvasCon;
		l->canvasCon = newCanvasCon;
	}
	if (currentLayer!=NULL)
		canvasCon = currentLayer->canvasCon;

	TCODConsole* newOverlayCon = new TCODConsole(newWidth, newHeight);
	newOverlayCon->setDefaultBackground(TCODColor(1, 2, 3)); // 1,2,3 is an uncommon color ;)
	newOverlayCon->setKeyColor(TCODColor(1, 2, 3));
	newOverlayCon->clear();
	// TCODConsole::blit(overlayCon,0,0,copyW,copyH,newOverlayCon,x,y);
	TCODConsole::blit(overlayCon,0,0,cw,ch,newOverlayCon,x,y);
	delete overlayCon;
	overlayCon = newOverlayCon;
}

void App::changeOperation(Operation *newOperation) {
	DEBUG_FILE_AND_LINE

	if(currentOperation != NULL) {
		currentOperation->end();
	}

	previousOperation = currentOperation;

	overlayCon->clear();
	gui->hideAllOptions();

	currentOperation = newOperation;
	currentOperation->start();
}

bool App::shouldOperationUpdate() {
	DEBUG_FILE_AND_LINE

	if(TCODConsole::isKeyPressed(TCODK_CONTROL))
		return false;
	if(gui->hideType == AppGui::TOTAL_HIDE)
		return true;
	if(Widget::focus && !currentOperation->isActive())
		return false;
	return true;
}

CanvasImage* App::getCanvasImage(std::string layerName) {
	Layer* layer = currentLayer;
	if (layerName!="")
		layer = getLayer(layerName);
	if (layer==NULL || canvasCon==NULL)
		assert(true and "Layer or Canvas is NULL, yet calling getCanvasImage!");

	CanvasImage* canvasImg = new CanvasImage;
	Brush brush;

	for(int x = 0; x < canvasWidth; x++) {
		for(int y = 0; y < canvasHeight; y++) {
			brush.symbol = layer->canvasCon->getChar(x, y);
			brush.fore = layer->canvasCon->getCharForeground(x, y);
			brush.back = layer->canvasCon->getCharBackground(x, y);
			brush.solid = true;
			/*
			if(solidCon->getCharBackground(x, y) == TCODColor(0, 0, 255))
				brush.solid = true;
			else
				brush.solid = false;
				*/

			canvasImg->push_back(brush);
		}
	}

	return canvasImg;

}

CanvasImage* App::getMergedCanvasImage(){
	Layer* layer = currentLayer;
	if (layer==NULL || canvasCon==NULL){
		std::cerr << "Layer or Canvas is NULL, yet calling getMergedCanvasImage!\n";
		assert(true);
	}

	TCODConsole* con = getMergedCanvasConsole();
	CanvasImage* canvasImg = new CanvasImage;
	Brush brush;

	for(int x = 0; x < canvasWidth; x++) {
		for(int y = 0; y < canvasHeight; y++) {
			brush.symbol = con->getChar(x, y);
			brush.fore = con->getCharForeground(x, y);
			brush.back = con->getCharBackground(x, y);
			brush.solid = true;
			canvasImg->push_back(brush);
		}
	}

	delete con;
	return canvasImg;
}

TCODConsole* App::getMergedCanvasConsole(){
	Layer* layer = currentLayer;
	if (layer==NULL || canvasCon==NULL){
		std::cerr << "Layer or Canvas is NULL. Creating junk console.\n";
		return new TCODConsole(canvasWidth,canvasHeight);
	}

	TCODConsole* con = new TCODConsole(canvasWidth,canvasHeight);
	con->setDefaultForeground(brush1.fore);
	con->setDefaultBackground(keyColour);
	con->setKeyColor(keyColour);
	con->clear();

	for(int i=0;i<layers.size();i++){
		Layer* l = layers.get(i);

		if (l->visible)
			TCODConsole::blit(l->canvasCon, 0, 0, canvasWidth, canvasHeight, con, 0, 0, l->fgalpha/255.,l->bgalpha/255.);
	}
	return con;
}

void App::setCanvasImage(CanvasImage& canvasImg) {
	DEBUG_FILE_AND_LINE

	assert(canvasCon!=NULL and "Canvas is NULL, yet calling setCanvasImage!");

	for(int x = 0; x < canvasWidth; x++) {
		for(int y = 0; y < canvasHeight; y++) {
			canvasCon->setChar(x, y, canvasImg[x * canvasHeight + y].symbol);
			canvasCon->setCharForeground(x, y, canvasImg[x * canvasHeight + y].fore);
			canvasCon->setCharBackground(x, y, canvasImg[x * canvasHeight + y].back);

			/*
			if(canvasImg[x * canvasHeight + y].solid)
				solidCon->setCharBackground(x, y, TCODColor(0, 0, 255));
			else
				solidCon->setCharBackground(x, y, TCODColor(255, 255, 255));
			*/
		}
	}

}

// layer operations
/// \brief Add a new layer to the top.
/// Generated layer is returned and selected.
Layer* App::addNewLayer(){
	DEBUG_FILE_AND_LINE

	Layer* l = new Layer;
	l->name = generateUniqueLayerName();
	l->fgalpha = 255; // 255;
	l->bgalpha = 255; // 255;
	l->compositingMode = Normal;
	l->visible = true;
	l->canvasCon = new TCODConsole(canvasWidth, canvasHeight);
	l->canvasCon->setDefaultForeground(brush1.fore);
	l->canvasCon->setDefaultBackground(keyColour);
	l->canvasCon->setKeyColor(keyColour);
	l->canvasCon->clear();

	currentLayer = l;
	canvasCon = l->canvasCon;

	layers.push(l);

	return l;
}

/// \brief Delete a named layer.
void App::deleteLayer(std::string name){
	for(int i=0;i<layers.size();i++){
		if (layers.get(i)->name==name){
			Layer* l = layers.get(i);
			layers.remove(l);
			if (currentLayer==l){
				if (layers.isEmpty()){
					currentLayer = NULL;
					canvasCon = NULL;
				}
				else {
					selectLayer(layers.get(0)->name);
				}
			}
			delete l;
			return;
		}
	}
}

/// Select the named layer
void App::selectLayer(std::string name){
	DEBUG_FILE_AND_LINE

	for(int i=0;i<layers.size();i++){
		if (layers.get(i)->name==name){
			currentLayer = layers.get(i);
			canvasCon = currentLayer->canvasCon;
			return;
		}
	}

	// errrrror!
	std::cerr << "Error: Trying to select non-existent layer!\nContinuing...\n";
}

Layer* App::getCurrentLayer(){
	return currentLayer;
}

const TCODList<Layer*>& App::getLayers(){
	return layers;
}

Layer* App::getLayer(std::string name){
	for(int i=0;i<layers.size();i++){
		if (layers.get(i)->name==name) return layers.get(i);
	}
	return NULL;
}

void App::shiftCurrentLayerUp(){
	if (currentLayer==NULL) return;
	// else

	// get index
	int index = -1;
	for(int i=0;i<layers.size();i++)
	{
		if (layers.get(i)==currentLayer){
			index = i;
			break;
		}
	}
	if (index==-1 || index==layers.size()-1) return;

	// else
	layers.remove(currentLayer);
	layers.insertBefore(currentLayer,index+1);
}

void App::shiftCurrentLayerDown(){
	if (currentLayer==NULL) return;
	// else

	// get index
	int index = -1;
	for(int i=0;i<layers.size();i++)
	{
		if (layers.get(i)==currentLayer){
			index = i;
			break;
		}
	}
	if (index==-1 || index==0) return;

	// else
	layers.remove(currentLayer);
	layers.insertBefore(currentLayer,index-1);
}

std::string App::generateUniqueLayerName(){
	for(char c1='A';c1<='Z';c1++)
		for(char c2='A';c2<='Z';c2++)
			for(char c3='A';c3<='Z';c3++)
				for(char c4='A';c4<='Z';c4++)
				{
					const char n[] = {c1,c2,c3,c4,'\0'};
					std::string name = n;
					bool taken = false;
					// start at AAAA and go to ZZZZ
					for(int i=0;i<layers.size();i++){
						Layer* l = layers.get(i);
						if (l->name == name) {
							taken = true;
							break;
						}
					}

					if (not taken){
						return name;
					}
				}
	// impossible
	assert(true and "Something impossible happened when generating unique layer names");
}

std::string App::modifyLayerNameToBeUnique(std::string name){
	// modify from the last character back
	for(char c1='1'-1;c1<='9';c1++)
			for(char c2='1'-1;c2<='9';c2++)
				for(char c3='1'-1;c3<='9';c3++)
					for(char c4='1'-1;c4<='9';c4++)
					{
						if (c1=='1'-1){

						}

						const char n[] = {
								(c1=='1'-1)?name[0]:c1,
								(c2=='1'-1)?name[1]:c2,
								(c3=='1'-1)?name[2]:c3,
								(c4=='1'-1)?name[3]:c4,
								'\0'};
						std::string newname = n;
						bool taken = false;
						// start at AAAA and go to ZZZZ
						for(int i=0;i<layers.size();i++){
							Layer* l = layers.get(i);
							if (l->name == newname) {
								taken = true;
								break;
							}
						}

						if (not taken){
							return newname;
						}
					}
	// impossible
	assert(true and "Something impossible happened when modifying a layer name");
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
	DEBUG_FILE_AND_LINE

	if (canvasCon==NULL) return;

	canvasModified = true;
	pastImages.push_back(*getCanvasImage());
	// Clear the redo list. However not doing this can be helpful at times.
	// But most people are not use to this.
	futureImages.clear();
}

void App::doUndo() {
	DEBUG_FILE_AND_LINE

	if (canvasCon==NULL) return;

	if(pastImages.size() > 0) {
		futureImages.push_back(*getCanvasImage());
		setCanvasImage(pastImages[pastImages.size() - 1]);
		pastImages.pop_back();
	}
}

void App::doRedo() {
	DEBUG_FILE_AND_LINE

	if (canvasCon==NULL) return;

	if(futureImages.size() > 0) {
		pastImages.push_back(*getCanvasImage());
		setCanvasImage(futureImages[futureImages.size() - 1]);
		futureImages.pop_back();
	}
}


void App::copyCanvasToClipboard(int x, int y, int w, int h){
	DEBUG_FILE_AND_LINE

	if (canvasCon==NULL) return;
	clipboardImage.clear();
	Brush brush;

	clipboardWidth = w;
	clipboardHeight = h;
	for(int dx = 0; dx < w; dx++) {
		for(int dy = 0; dy < h; dy++) {
			brush.symbol = canvasCon->getChar(x+dx, y+dy);
			brush.fore = canvasCon->getCharForeground(x+dx, y+dy);
			brush.back = canvasCon->getCharBackground(x+dx, y+dy);
			brush.solid = true;
			/*
			if(solidCon->getCharBackground(x+dx, y+dy) == TCODColor(0, 0, 255))
				brush.solid = true;
			else
				brush.solid = false;
			*/

			clipboardImage.push_back(brush);
		}
	}
}

void App::applyClipboardToOverlay(int x, int y){
	DEBUG_FILE_AND_LINE

	if (clipboardImage.empty()) return;

	for(int dx = 0; dx < clipboardWidth; dx++) {
		for(int dy = 0; dy < clipboardHeight; dy++) {
			overlayCon->setChar(x+dx, y+dy, clipboardImage[dx * clipboardHeight + dy].symbol);
			overlayCon->setCharForeground(x+dx, y+dy, clipboardImage[dx * clipboardHeight + dy].fore);
			overlayCon->setCharBackground(x+dx, y+dy, clipboardImage[dx * clipboardHeight + dy].back);
			/*
			if(clipboardImage[dx * clipboardHeight + dy].solid)
				solidOverlayCon->setCharBackground(x+dx, y+dy, TCODColor(0, 0, 255));
			else
				solidOverlayCon->setCharBackground(x+dx, y+dy, TCODColor(255, 255, 255));
				*/
		}
	}
}

void App::applyClipboardToCanvas(int x, int y){
	DEBUG_FILE_AND_LINE

	if (canvasCon==NULL) return;
	if (clipboardImage.empty()) return;

	for(int dx = 0; dx < clipboardWidth; dx++) {
		for(int dy = 0; dy < clipboardHeight; dy++) {
			canvasCon->setChar(x+dx, y+dy, clipboardImage[dx * clipboardHeight + dy].symbol);
			canvasCon->setCharForeground(x+dx, y+dy, clipboardImage[dx * clipboardHeight + dy].fore);
			canvasCon->setCharBackground(x+dx, y+dy, clipboardImage[dx * clipboardHeight + dy].back);
			/*
			if(clipboardImage[dx * clipboardHeight + dy].solid)
				solidCon->setCharBackground(x+dx, y+dy, TCODColor(0, 0, 255));
			else
				solidCon->setCharBackground(x+dx, y+dy, TCODColor(255, 255, 255));
			*/
		}
	}
}

// This function applies the brush to the selected cell of the console
// con. However if the user has not set use symbol, use fore or use back
// then it will not apply that part of the brush to the console but instead
// leave that part (symbol, fore or back) the way it is
	void App::applyBrushToOverlayCell(int x, int y, Brush *brush) {
		if (canvasCon==NULL) return;

		DEBUG_FILE_AND_LINE

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

		/*
		if(gui->useSolidToggleButton->isPressed()) {
				if(brush->solid)
						solidOverlayCon->setCharBackground(x, y, TCODColor(0, 0, 255));
				else
						solidOverlayCon->setCharBackground(x, y, TCODColor(255, 255, 255));
		}
		*/
	}

void App::clearOverlay() {
	DEBUG_FILE_AND_LINE

	overlayCon->setDefaultBackground(TCODColor(1, 2, 3));
	overlayCon->clear();

	//solidOverlayCon->setDefaultBackground(TCODColor(1, 2, 3));
	//solidOverlayCon->clear();
}

void App::applyOverlayToCanvas() {
	if (currentLayer==NULL || canvasCon==NULL) return;

	TCODConsole::blit(overlayCon, 0, 0, canvasWidth, canvasHeight, canvasCon, 0, 0);
    //TCODConsole::blit(solidOverlayCon, 0, 0, canvasWidth, canvasHeight, solidCon, 0, 0);
}

void App::applyCanvasToOverlay() {
	if (currentLayer==NULL || canvasCon==NULL) return;

	canvasCon->setKeyColor(TCODColor(1,2,3));
    TCODConsole::blit(canvasCon, 0, 0, canvasWidth, canvasHeight, overlayCon, 0, 0);
    canvasCon->setKeyColor(keyColour);
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
