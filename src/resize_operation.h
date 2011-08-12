#ifndef ASCII_PAINT_RESIZE_OPERATION_H
#define ASCII_PAINT_RESIZE_OPERATION_H

#include "operation.h"

class ResizeOperation : public Operation {
	public:
		ResizeOperation();
		virtual ~ResizeOperation();
		virtual void start();
		virtual void update();
		virtual void end();

	private:
};

#endif
