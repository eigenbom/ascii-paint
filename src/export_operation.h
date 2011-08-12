#ifndef ASCII_PAINT_EXPORT_OPERATION_H
#define ASCII_PAINT_EXPORT_OPERATION_H

#include "operation.h"

class ExportOperation : public Operation {
	public:
		ExportOperation();
		virtual ~ExportOperation();
		virtual void start();
		virtual void update();
		virtual void end();

	private:
};

void doExportPngCbk(Widget *wid, void *data);
void doExportBmpCbk(Widget *wid, void *data);
void doExportTextCbk(Widget *wid, void *data);
void doExportForeCbk(Widget *wid, void *data);
void doExportBackCbk(Widget *wid, void *data);

#endif





