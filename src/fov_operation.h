#ifndef ASCII_PAINT_LOS_H
#define ASCII_PAINT_LOS_H

#include "libtcod.hpp"
#include "operation.h"

// Fov means 'Field Of View'

class FovOperation : public Operation {
	public:
		FovOperation();
		virtual ~FovOperation();
		virtual void start();
		virtual void update();
		virtual void end();

		static TCOD_fov_algorithm_t fovType;

	private:
		TCODMap *map;

};

void fovTypeButtonCbk(Widget *wid, void *data);

#endif

