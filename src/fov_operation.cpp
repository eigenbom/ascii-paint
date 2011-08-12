#include <cstring>
#include "libtcod.hpp"
#include "gui/gui.hpp"
#include "app.h"
#include "brush.h"
#include "fov_operation.h"

TCOD_fov_algorithm_t FovOperation::fovType = FOV_BASIC;

FovOperation::FovOperation() {
}

FovOperation::~FovOperation() {
}

void FovOperation::start() {
    app->gui->fovRadiusSlider->setVisible(true);
    app->gui->fovLightWallsToggleButton->setVisible(true);
    app->gui->fovTypeButton->setVisible(true);

    map = new TCODMap(app->canvasWidth, app->canvasHeight);
    for(int x = 0; x < app->canvasWidth; x++) {
        for(int y = 0; y < app->canvasHeight; y++) {
            if(app->solidCon->getCharBackground(x, y) == TCODColor(255, 255, 255)) // No Solid
                map->setProperties(x, y, true, true);
            else
                map->setProperties(x, y, false, false);
        }
    }

    app->setOverlayFade(0.7);
}

void FovOperation::update() {
    TCOD_mouse_t mouse = app->mouse;
    int mouseX = app->canvasMouseX;
    int mouseY = app->canvasMouseY;

    int radius = atoi(app->gui->fovRadiusSlider->getValue());
    bool lightWalls = app->gui->fovLightWallsToggleButton->isPressed();

    // If the mouse is inside the canvas area...
    if(mouseX >= 0 && mouseX < app->canvasWidth &&
            mouseY >= 0 && mouseY < app->canvasHeight) {

        map->computeFov(mouseX, mouseY, radius, lightWalls, fovType);

        app->clearOverlay();
        Brush losBrush;
        losBrush.fore = TCODColor(0, 0, 0);
        losBrush.back = TCODColor(255, 255, 0);
        losBrush.symbol = ' ';

        for(int x = 0; x < app->canvasWidth; x++) {
            for(int y = 0; y < app->canvasHeight; y++) {
                if(map->isInFov(x, y))
                    app->applyBrushToOverlayCell(x, y, &losBrush);
            }
        }

        losBrush.back = TCODColor(128, 128, 0);
        app->applyBrushToOverlayCell(mouseX, mouseY, &losBrush);
    }

}

void FovOperation::end() {
    delete map;
    app->setOverlayFade(1.0);
}

void fovTypeButtonCbk(Widget *wid, void *data) {
    static int type = 0;

    Button *button = reinterpret_cast<Button *>(wid);
    if(type == 0) {
        button->setLabel("Diamond");
        FovOperation::fovType = FOV_DIAMOND;
    } else if(type == 1) {
        button->setLabel("Shadow");
        FovOperation::fovType = FOV_SHADOW;
    } else if(type == 2) {
        button->setLabel("Restrictive");
        FovOperation::fovType = FOV_RESTRICTIVE;
    } else if(type == 3) {
        button->setLabel("Basic");
        FovOperation::fovType = FOV_BASIC;
    }


    type++;
    type = type % 4; // So that the type goes back to 0 when it goes over 3
}
