#ifndef ASCII_PAINT_QUIT_OPERATION_H
#define ASCII_PAINT_QUIT_OPERATION_H

#include "operation.h"

class QuitOperation : public Operation {
	public:
		QuitOperation();
		virtual ~QuitOperation();
		virtual void start();
		virtual void update();
		virtual void end();

	private:
};

#endif



