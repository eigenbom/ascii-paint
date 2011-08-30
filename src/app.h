#ifndef ASCII_PAINT_APP_H
#define ASCII_PAINT_APP_H

#include <string>
#include <vector>
#include <list>
#include <cassert>

#include "app_gui.h"
#include "app_data.h"
#include "libtcod.hpp"
#include "gui/gui.hpp"

#include "palette.h"
#include "brush.h"
#include "layer.h"
#include "color_box.h"
#include "operation.h"

class Operation;

#define GRID_MODE_BULLETS 0
#define GRID_MODE_CORNERS 1

// Basically a vector representing each cell in the canvas/image
// To access the cell at position (x, y) use:
// CanvasImage[x + y * width]
typedef std::vector<Brush> CanvasImage;

class App {
	public:
		App();
		~App();

		int exec();

		void initBrushes();
		void initCanvas();
		void initOperations();

		void resizeCanvas(int newWidth, int newHeight);

		void changeOperation(Operation *newOperation);
		bool shouldOperationUpdate();

		void setCanvasImage(CanvasImage& canvasImg);
		CanvasImage* getCanvasImage(std::string layerName="");

		void clearOverlay();
		void applyBrushToOverlayCell(int x, int y, Brush *brush);
		void applyOverlayToCanvas();
		void applyCanvasToOverlay();
        void setOverlayFade(float fade);

        // layer operations
        /// \brief Add a new layer to the top.
        /// Generated layer is returned and selected.
        Layer* addNewLayer();
        /// \brief Delete a named layer.
        void deleteLayer(std::string name);
        /// Select the named layer
        void selectLayer(std::string name);
        /// Retrieve the current layer (may be NULL)
        Layer* getCurrentLayer();
        /// Retrieve the named layer (may be NULL)
        Layer* getLayer(std::string name);
        /// Get all layers
        const TCODList<Layer*>& getLayers();

        void shiftCurrentLayerUp();
        void shiftCurrentLayerDown();

        std::string generateUniqueLayerName();
        std::string modifyLayerNameToBeUnique(std::string name);

        void setGridDimensions(int w, int h);
        void setShowGrid(bool on);

		void addUndo();
		void doUndo();
		void doRedo();

		void copyCanvasToClipboard(int x, int y, int w, int h);
		void applyClipboardToOverlay(int x, int y);
		void applyClipboardToCanvas(int x, int y);

		void resetCanvasView();

		static App* getSingleton();

	public:
		AppGui *gui;
		AppData *data;

		int windowWidth;
		int windowHeight;
		TCODColor windowBackgroundColor;
		std::string fontFilename;
		int fontLayout;
		int fontType;

		bool fullscreen;
		int fpsGoal;
		int currentFps;

		int gridMode; //
		bool showGrid;
		int gridW, gridH;

		TCODColor keyColour; // keying colour

		int canvasWidth;
		int canvasHeight;
		
		// The distance between the top-left corner of canvas and the top-left
		// corner of the screen
		int canvasOffsetX;
		int canvasOffsetY;

		// The cell of the canvas that the mouse is currently over
		int canvasMouseX;
		int canvasMouseY;

		std::string filename;
		float mapVersion;
		bool canvasModified;

		bool isCanvasBeingDragged;
		bool quit;

		Operation *currentOperation;
		Operation *previousOperation;

		std::vector<CanvasImage> pastImages; // Undo Images stored here
		std::vector<CanvasImage> futureImages; // Redo Images store here

		CanvasImage clipboardImage; // Result from a copy operation
		int clipboardWidth, clipboardHeight;

		// Layers
		TCODList<Layer*> layers;
		Layer* currentLayer;

		TCODConsole *overlayCon; // Drawn over the canvas
		TCODConsole *canvasCon;

		// Deprecated
		// TCODConsole *solidCon;
		// TCODConsole *solidOverlayCon; // This canvas holds proposed writes to solidCon

        float overlayFade;
		bool inTypingMode;

		TCOD_key_t key;
		TCOD_mouse_t mouse;

		Brush brush1;
		Brush brush2;
		Brush initBrush1;
		Brush initBrush2;

		TCODList<Palette*> palettes;
		int currentPaletteIndex;

	private:
		void handleKeyboardShortcuts();
		void handleCanvasDragging();
		void moveMouseWithKeyBoard();

		static App* instance;

};


#endif
