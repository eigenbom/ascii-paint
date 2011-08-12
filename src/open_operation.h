#ifndef ASCII_PAINT_OPEN_OPERATION_H
#define ASCII_PAINT_OPEN_OPERATION_H

#include "operation.h"

class OpenOperation : public Operation {
	public:
		OpenOperation();
		virtual ~OpenOperation();
		virtual void start();
		virtual void update();
		virtual void end();

	private:
};

#endif





