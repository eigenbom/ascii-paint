#ifndef ASCII_PAINT_SAVE_OPERATION_H
#define ASCII_PAINT_SAVE_OPERATION_H

#include "operation.h"

class SaveOperation : public Operation {
	public:
		SaveOperation();
		virtual ~SaveOperation();
		virtual void start();
		virtual void update();
		virtual void end();

	private:
};

bool doSave(App *app);

#endif




