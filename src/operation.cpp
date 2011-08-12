#include "operation.h"

Operation::Operation() {
	active = false;
}

Operation::~Operation() {

}

App* Operation::getApp() {
	return app;
}

void Operation::start() {

}

void Operation::update() {

}

void Operation::end() {

}

bool Operation::isActive() {
	return active;
}
