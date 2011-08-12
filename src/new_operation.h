#ifndef ASCII_PAINT_NEW_OPERATION_H
#define ASCII_PAINT_NEW_OPERATION_H

#include "operation.h"

class NewOperation : public Operation {
	public:
		NewOperation();
		virtual ~NewOperation();
		virtual void start();
		virtual void update();
		virtual void end();

	private:
};

#endif



