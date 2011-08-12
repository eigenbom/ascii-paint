#ifndef ASCIIPAINT_PASTEOPERATION_H
#define ASCIIPAINT_PASTEOPERATION_H

#include "operation.h"

class PasteOperation : public Operation {
	public:
	PasteOperation();
	virtual ~PasteOperation();
	virtual void start();
	virtual void update();
	virtual void end();

	protected:
	bool justCleared;
};

#endif
