#ifndef ASCII_PAINT_GET_STRING_MESSAGE_BOX_H
#define ASCII_PAINT_GET_STRING_MESSAGE_BOX_H

class GetStringMessageBox {
	public:
		GetStringMessageBox(char *message, char *title, char *defaultValuei = NULL);
		virtual ~GetStringMessageBox();

		void show();
		bool isOK();
		char* getString();
	private:
		char* input;
		char* message;
		char* title;
		char* defaultValue;
		bool ok;

};
#endif
