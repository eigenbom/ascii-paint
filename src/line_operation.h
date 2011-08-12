#ifndef ASCII_PAINT_LINE_OPERATION_H
#define ASCII_PAINT_LINE_OPERATION_H

#include "operation.h"

class LineOperation : public Operation {
	public:
		LineOperation();
		virtual ~LineOperation();
		virtual void start();
		virtual void update();
		virtual void end();

	private:
		void setStraightLineEndPoint(int* lineEndX, int* lineEndY);
		int lineStartX;
		int lineStartY;
		bool justCleared;

};

#endif
