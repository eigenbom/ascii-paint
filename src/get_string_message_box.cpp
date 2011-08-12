#include <cstdio>
#include <cstring>
#include "gui/gui.hpp"
#include "get_string_message_box.h"

GetStringMessageBox::GetStringMessageBox(char* theMessage, char* theTitle, char *theDefaultValue) {
	/*message = new char[1024];
	strcpy(message, theMessage);

	title = new char[1024];
	strcpy(title, theTitle);

	if(defaultValue != NULL) {
		defaultValue = new char[1024];
		strcpy(defaultValue, theDefaultValue);
	}*/
	message = theMessage;
	title = theTitle;
	defaultValue = theDefaultValue;
}

GetStringMessageBox::~GetStringMessageBox() {

}

void GetStringMessageBox::show() {
	Widget::setConsole(TCODConsole::root);

	Label *messageLabel = new Label(0, 0, message);
	TextBox *inputBox = new TextBox(0, 0, 30, 1000, NULL, defaultValue);

	VBox *vbox = new VBox(0, 0, 2);
	vbox->addWidget(messageLabel);
	vbox->addWidget(inputBox);

	ToolBar *msgBox = new ToolBar(20, 20, title);
	msgBox->addWidget(vbox);

	Widget::renderWidgets();
	TCODConsole::root->flush();

	TCOD_key_t key;

	do {
		key = TCODConsole::checkForKeypress();
		Widget::updateWidgets(key);
		Widget::renderWidgets();
		TCODConsole::root->flush();
	} while(key.vk != TCODK_ESCAPE);
	delete msgBox;

}

bool GetStringMessageBox::isOK() {
	return ok;
}

char* GetStringMessageBox::getString() {
	return input;
}
