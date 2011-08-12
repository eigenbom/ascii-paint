#include "libtcod.hpp"
#include "gui/gui.hpp"
#include "app.h"
#include "undo_operation.h"

UndoOperation::UndoOperation() {
}

UndoOperation::~UndoOperation() {
}

void UndoOperation::start() {
	app->doUndo();
	app->changeOperation(app->previousOperation);
}

void UndoOperation::update() {

}

void UndoOperation::end() {

}


