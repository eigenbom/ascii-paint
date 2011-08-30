#include "layer_widget.h"
#include "lbutton.h"
#include "small_button.h"
#include "operation.h"
#include "app.h"
#include "message_box.h"

#include <iostream>

class LayerEntry: public Widget, AppUser {
public:
	static LayerWidget* lw;

	LayerEntry(int w, Layer* l):Widget(0,0,w,1){
		layer = l;
		pressed = false;
	}

	~LayerEntry(){
	}

	void render(){
		con->setDefaultBackground(mouseIn ? backFocus : back);
		con->setDefaultForeground(mouseIn ? foreFocus : fore);

		if ( w > 0 && h > 0 ) con->rect(x,y,w,h,true,TCOD_BKGND_SET);
		if (app->getCurrentLayer()==layer){
			con->setDefaultForeground(foreFocus);
		}
		if (pressed)
			con->print(x+2,y,"-%s-",layer->name.c_str());
		else
			con->print(x+2,y," %s ",layer->name.c_str());

		con->setDefaultForeground(fore);
		con->putCharEx(x,y,layer->visible?7:9,fore,back);
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

			if (dx==0){
				// then pressed the visibility button
				layer->visible = !layer->visible;
			}
			else {
				// selected the layer
				app->selectLayer(layer->name);
				lw->regenerateLayerList();
			}
		}
	}

	// protected
	Layer* layer;
	bool pressed;
};

LayerWidget* LayerEntry::lw;

LayerWidget::LayerWidget(int x, int y, int w) // , widget_callback_t cbk,  void *userData)
:Container(x,y,w,5){
	LayerEntry::lw = this;

	toolBar = new SmallToolBar(x,y,w, "Layers", "Manage the layers of this image");

	controls = new HBox(x,y,0);
	controls->addWidget(new SmallButton("+","Add a new layer", newLayerCallback, this));
	controls->addWidget(new SmallButton("-","Delete the current layer", deleteLayerCallback, this));
	controls->addWidget(new SmallButton(TCOD_CHAR_ARROW_N,"Shift the current layer up", shiftLayerUpCallback, this));
	controls->addWidget(new SmallButton(TCOD_CHAR_ARROW_S,"Shift the current layer down", shiftLayerDownCallback, this));
	// controls->addWidget(new SmallButton("M","Change the compositing mode of the current layer", modifyLayerModeCallback, this));
	controls->addWidget(new SmallButton("R","Rename the current layer", renameLayerCallback, this));
	toolBar->addWidget(controls);

	// slider
	fgalpha = new Slider2(0, 0, 3, 0.0f, 255.0f, "fg", "Foreground alpha");
	fgalpha->setFormat("%.0f");
	fgalpha->setValue((float)255);
	fgalpha->setSensitivity(4);
	fgalpha->setCallback(changeAlphaCallback, this);
	toolBar->addWidget(fgalpha);

	bgalpha = new Slider2(0, 0, 3, 0.0f, 255.0f, "bg", "Background alpha");
	bgalpha->setFormat("%.0f");
	bgalpha->setValue((float)255);
	bgalpha->setSensitivity(4);
	bgalpha->setCallback(changeAlphaCallback, this);
	toolBar->addWidget(bgalpha);

	// Layers
	layerVBox = new VBox(x,y,0);
	regenerateLayerList();
	toolBar->addWidget(layerVBox);

}

LayerWidget::~LayerWidget(){
	delete toolBar;
	// delete layerVBox;
}

void LayerWidget::render(){
	con->setDefaultBackground(back);
	con->setDefaultForeground(fore);

	toolBar->render();
	Container::render();
}

void LayerWidget::update(const TCOD_key_t k){
	Widget::update(k);

	toolBar->x = x;
	toolBar->y = y;
	toolBar->update(k);
}

void LayerWidget::computeSize(){
	toolBar->computeSize();
	h = toolBar->h; //  + layerVBox->h;
}

void LayerWidget::regenerateLayerList(){
	layerVBox->clear();
	const TCODList<Layer*>& layers = app->getLayers();
	for(int i=layers.size()-1;i>=0;i--){
		layerVBox->addWidget(new LayerEntry(w,layers.get(i)));
	}

	Layer* l = app->getCurrentLayer();
	if (l!=NULL){
		fgalpha->setValue((float)l->fgalpha);
		bgalpha->setValue((float)l->bgalpha);
	}

	computeSize();
}

// protected:

void LayerWidget::onButtonClick(){}
void LayerWidget::onRButtonClick(){}

void newLayerCallback(Widget *wid, void *data){
	LayerWidget* lw = static_cast<LayerWidget*>(data);
	App::getSingleton()->addNewLayer();
	lw->regenerateLayerList();
}

void deleteLayerCallback(Widget *wid, void *data){
	LayerWidget* lw = static_cast<LayerWidget*>(data);
	App* app = App::getSingleton();
	Layer* l = app->getCurrentLayer();
	if (l!=NULL){
		MessageBox msgBox("Delete Layer", "Definitely delete layer?", 2, false);
		msgBox.show();

		// If user presses ESCAPE or cancel
		if(msgBox.getButtonPressed() == 0 || msgBox.getButtonPressed() == 2) {
			return;
		}

		if(msgBox.getButtonPressed() == 1) {
			app->deleteLayer(l->name);
		}
	}
	lw->regenerateLayerList();
}

void shiftLayerUpCallback(Widget *wid, void *data){
	LayerWidget* lw = static_cast<LayerWidget*>(data);
	App* app = App::getSingleton();
	app->shiftCurrentLayerUp();
	lw->regenerateLayerList();
}

void shiftLayerDownCallback(Widget *wid, void *data){
	LayerWidget* lw = static_cast<LayerWidget*>(data);
	App* app = App::getSingleton();
	app->shiftCurrentLayerDown();
	lw->regenerateLayerList();
}

void modifyLayerModeCallback(Widget *wid, void *data){
	LayerWidget* lw = static_cast<LayerWidget*>(data);
	App* app = App::getSingleton();
	lw->regenerateLayerList();
}

void renameLayerCallback(Widget *wid, void *data){
	LayerWidget* lw = static_cast<LayerWidget*>(data);

	App *app = AppUser::app;
	if (app->getCurrentLayer()==NULL) return;
	// else
	Layer* l = app->getCurrentLayer();

	MessageBox msgBox("Change Layer Name", "Enter new layer name (max 4 characters)", 2, true);
	msgBox.setString(l->name.c_str());
	msgBox.show();

	// If user presses ESCAPE or cancel
	if(msgBox.getButtonPressed() == 0 || msgBox.getButtonPressed() == 2) {
		return;
	}

	if(msgBox.getButtonPressed() == 1 && msgBox.getString() != NULL) {
		std::string s = msgBox.getString();
		s += "    "; // pad four spaces
		const char firstFour[] = {
				s[0],s[1],s[2],s[3],'\0'
		};
		std::string adapted = app->modifyLayerNameToBeUnique(firstFour);
		l->name = adapted;
	}

	lw->regenerateLayerList();
}

void changeAlphaCallback(Widget *wid, float val, void *data){
	LayerWidget* lw = static_cast<LayerWidget*>(data);
	App* app = App::getSingleton();

	Layer* l = app->getCurrentLayer();
	if (l!=NULL){
		l->fgalpha = atof(lw->fgalpha->getValue());
		l->bgalpha = atof(lw->bgalpha->getValue());
	}

	lw->regenerateLayerList();
}
