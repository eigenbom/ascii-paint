#ifndef ASCII_PAINT_TEXT_H
#define ASCII_PAINT_TEXT_H

#include "operation.h"
#include <vector>

class TextOperation : public Operation {
	public:
		TextOperation();
		virtual ~TextOperation();
		virtual void start();
		virtual void update();
		virtual void end();

	private:
		bool positionSelected;
		int textX;
		int textY;
		std::vector<char> text;
		Brush *brush;
};

#endif

