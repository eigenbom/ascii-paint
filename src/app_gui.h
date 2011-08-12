#ifndef ASCII_PAINT_APP_GUI_H
#define ASCII_PAINT_APP_GUI_H

#include "gui/gui.hpp"
#include "app_user.h"
#include "lradio_button.h"

class App;
class ColorBox;

class AppGui : public AppUser {
	public:
		AppGui();
		~AppGui();

		void build();
		void update();
		void draw();
		void hideAllOptions();

	public:
		TCODConsole *guiCon; 
		TCODColor backgroundColor1;
		TCODColor backgroundColor2;
		TCODColor foregroundColor1;
		TCODColor foregroundColor2;

		LRadioButton *cellOperationButton;
		LRadioButton *lineOperationButton;
		LRadioButton *rectOperationButton;
		LRadioButton *rectFOperationButton;
		LRadioButton *ovalOperationButton;
		LRadioButton *ovalFOperationButton;
		LRadioButton *fillOperationButton;
		LRadioButton *textOperationButton;
		LRadioButton *copyOperationButton;
		LRadioButton *pasteOperationButton;

		LRadioButton *pickOperationButton;
		LRadioButton *fovOperationButton;
		LRadioButton *pathOperationButton;

		/*

		Slider *redSlider1;
		Slider *greenSlider1;
		Slider *blueSlider1;
		Slider *redSlider2;
		Slider *greenSlider2;
		Slider *blueSlider2;
		Slider *redSlider3;
		Slider *greenSlider3;
		Slider *blueSlider3;
		Slider *redSlider4;
		Slider *greenSlider4;
		Slider *blueSlider4;

		ColorBox *brush1ForeImg;
		ColorBox *brush1BackImg;
		ColorBox *brush2ForeImg;
		ColorBox *brush2BackImg;

		Button *symbolButton1;
		Button *symbolButton2;
		ToggleButton *solidToggleButton1;
		ToggleButton *solidToggleButton2;
		*/

		// Options
		ToggleButton *outlineToggleButton;
		ToggleButton *filledToggleButton;
		ToggleButton *useSymbolToggleButton;
		ToggleButton *useForegroundToggleButton;
		ToggleButton *useBackgroundToggleButton;
		ToggleButton *useSolidToggleButton;
		ToggleButton *pickSymbolToggleButton;
		ToggleButton *pickForegroundToggleButton;
		ToggleButton *pickBackgroundToggleButton;
		ToggleButton *pickSolidToggleButton;
		Button *exportPngButton;
		Button *exportBmpButton;
		Button *exportTextButton;
		Button *exportForeButton;
		Button *exportBackButton;
		Button *importTextButton;
		Button *importForeButton;
		Button *importBackButton;
		Slider *fovRadiusSlider;
		ToggleButton *fovLightWallsToggleButton;
		Button *fovTypeButton;
		ToggleButton *pathDiagonalToggleButton;
		Label *pathLengthLabel;

		ToggleButton *viewImageToggleButton;
		ToggleButton *viewSolidToggleButton;
		ToggleButton *viewGridToggleButton;

		// grid width/height
		Slider *gwSlider;
		Slider *ghSlider;

		Label *currentPositionLabel;
		Label *canvasSizeLabel;
		Label *fpsLabel;

		enum HideType {
			NO_HIDE=0,
			PARTIAL_HIDE,
			TOTAL_HIDE,
		};

		int hideType;
};

void changeColorCbk(Widget *wid, float val, void *data);
void changeSolidCbk(Widget *wid, void *data);
void changeSymbolCallback(Widget *wid, void *data);
void operationCbk(Widget *wid, void *data);

void otherChangeSymbolCallback(Widget *wid, void *data);
void otherChangeColourCallback(Widget *wid, void *data);
void changeGridDimensionsCbk(Widget* wid, float val, void* data);


#endif
