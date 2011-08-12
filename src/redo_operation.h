#ifndef ASCII_PAINT_REDO_OPERATION_H
#define ASCII_PAINT_REDO_OPERATION_H

#include "operation.h"

class RedoOperation : public Operation {
	public:
		RedoOperation();
		virtual ~RedoOperation();
		virtual void start();
		virtual void update();
		virtual void end();

	private:
};

#endif



