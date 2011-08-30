#ifndef ASCIIPAINT_LAYERWIDGET_H
#define ASCIIPAINT_LAYERWIDGET_H

#include "libtcod.hpp"
#include "gui/gui.hpp"

#include "small_toolbar.h"
#include "slider2.h"
#include "layer.h"
#include "app_user.h"

class LayerWidget: public Container, AppUser {
public:
	LayerWidget(int x, int y, int w);
	~LayerWidget();

	void render();
	void update(const TCOD_key_t k);
	void computeSize();

	void regenerateLayerList();
// protected:

	void onButtonClick();
	void onRButtonClick();

	SmallToolBar* toolBar;

	VBox* layerVBox; // list of layers
	HBox* controls; // the controls

	Slider2* fgalpha;
	Slider2* bgalpha;
};

void newLayerCallback(Widget *wid, void *data);
void deleteLayerCallback(Widget *wid, void *data);
void shiftLayerUpCallback(Widget *wid, void *data);
void shiftLayerDownCallback(Widget *wid, void *data);
void modifyLayerModeCallback(Widget *wid, void *data);
void renameLayerCallback(Widget *wid, void *data);
void changeAlphaCallback(Widget* wid, float val, void* data);


#endif
