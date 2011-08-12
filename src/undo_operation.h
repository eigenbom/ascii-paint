#ifndef ASCII_PAINT_UNDO_OPERATION_H
#define ASCII_PAINT_UNDO_OPERATION_H

#include "operation.h"

class UndoOperation : public Operation {
	public:
		UndoOperation();
		virtual ~UndoOperation();
		virtual void start();
		virtual void update();
		virtual void end();

	private:
};

#endif


