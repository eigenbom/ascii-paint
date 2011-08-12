#ifndef ASCII_PAINT_PATH_H
#define ASCII_PAINT_PATH_H

#include "libtcod.hpp"
#include "operation.h"

class PathOperation : public Operation {
	public:
		PathOperation();
		virtual ~PathOperation();
		virtual void start();
		virtual void update();
		virtual void end();

	private:
		bool allowDiagonal;

		int pointX;
		int pointY;

		TCODMap *map;
		TCODPath *path;
};

#endif

