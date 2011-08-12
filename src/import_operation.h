#ifndef ASCII_PAINT_IMPORT_OPERATION_H
#define ASCII_PAINT_IMPORT_OPERATION_H

#include "operation.h"

class ImportOperation : public Operation {
	public:
		ImportOperation();
		virtual ~ImportOperation();
		virtual void start();
		virtual void update();
		virtual void end();

	private:
};

void doImportTextCbk(Widget *wid, void *data);
void doImportForeCbk(Widget *wid, void *data);
void doImportBackCbk(Widget *wid, void *data);

#endif





