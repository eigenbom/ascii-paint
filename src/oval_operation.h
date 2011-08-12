#ifndef ASCII_PAINT_OVAL_OPERATION_H
#define ASCII_PAINT_OVAL_OPERATION_H

#include "operation.h"

class OvalOperation : public Operation {
	public:
		OvalOperation();
		virtual ~OvalOperation();
		virtual void start();
		virtual void update();
		virtual void end();

	protected:
		void setCircleEndPoint(int* ovalEndX, int* ovalEndY);
		int ovalStartX;
		int ovalStartY;
		bool justCleared;

};

class OvalFOperation: public OvalOperation {
public:
	OvalFOperation():OvalOperation(){}
	virtual void start();
};
#endif

