#ifndef ASCII_PAINT_FILL_H
#define ASCII_PAINT_FILL_H

#include "operation.h"

class FillOperation : public Operation {
	public:
		FillOperation();
		virtual ~FillOperation();
		virtual void start();
		virtual void update();
		virtual void end();

	private:
		void doFill(int x, int y, Brush *oldBrush, Brush *newBrush);
		bool cellUsesBrush(int x, int y, Brush* brush);
		TCOD_mouse_t oldMouse;
		int oldMouseX;
		int oldMouseY;
		bool justCleared;

};

#endif
