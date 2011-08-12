#include "libtcod.hpp"
#include "gui/gui.hpp"
#include "app.h"
#include "redo_operation.h"

RedoOperation::RedoOperation() {
}

RedoOperation::~RedoOperation() {
}

void RedoOperation::start() {
	app->doRedo();
	app->changeOperation(app->previousOperation);
}

void RedoOperation::update() {

}

void RedoOperation::end() {

}



