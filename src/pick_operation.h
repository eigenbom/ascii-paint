#ifndef ASCII_PAINT_PICK_H
#define ASCII_PAINT_PICK_H

#include "operation.h"

class PickOperation : public Operation {
	public:
		PickOperation();
		virtual ~PickOperation();
		virtual void start();
		virtual void update();
		virtual void end();

	private:
};

#endif


