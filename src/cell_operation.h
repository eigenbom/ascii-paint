#ifndef ASCII_PAINT_CELL_H
#define ASCII_PAINT_CELL_H

#include "operation.h"

class CellOperation : public Operation {
	public:
		CellOperation();
		virtual ~CellOperation();
		virtual void start();
		virtual void update();
		virtual void end();

	private:
		int oldMouseX;
		int oldMouseY;
		
		bool justCleared;

};

#endif
