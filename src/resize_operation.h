#ifndef ASCII_PAINT_RESIZE_OPERATION_H
#define ASCII_PAINT_RESIZE_OPERATION_H

#include "operation.h"

struct ResizeInfo; // forward decl

class ResizeOperation : public Operation {
	public:
		ResizeOperation();
		virtual ~ResizeOperation();
		virtual void start();
		virtual void update();
		virtual void end();

		void setResizeInfo(ResizeInfo* ri); // takes ownership
	private:
		ResizeInfo* resizeInfo;
};

#endif
