#ifndef ASCII_PAINT_RECT_OPERATION_H
#define ASCII_PAINT_RECT_OPERATION_H

#include "operation.h"

class RectOperation : public Operation {
	public:
		RectOperation();
		virtual ~RectOperation();
		virtual void start();
		virtual void update();
		virtual void end();

	protected:
		void setSquareEndPoint(int* rectEndX, int* rectEndY);
		int rectStartX;
		int rectStartY;
		bool justCleared;

};

class FilledRectOperation: public RectOperation {
public:
	FilledRectOperation():RectOperation(){}
	virtual void start();
};

#endif

