#ifndef ASCIIPAINT_COPYOPERATION_H
#define ASCIIPAINT_COPYOPERATION_H

#include "operation.h"

class CopyOperation : public Operation {
	public:
		CopyOperation();
		virtual ~CopyOperation();
		virtual void start();
		virtual void update();
		virtual void end();

	protected:
		int rectStartX;
		int rectStartY;
		bool justCleared;
};

#endif
