#ifndef ASCII_PAINT_OPERATION_H
#define ASCII_PAINT_OPERATION_H

#include "libtcod.hpp"
#include "gui/gui.hpp"
#include "app_user.h"

class App;

class Operation : public AppUser {
	public:
		Operation();
		virtual ~Operation();
		virtual void start();
		virtual void update();
		virtual void end();
		virtual bool isActive();

		App* getApp();


	protected:
		bool active;

};

#endif
